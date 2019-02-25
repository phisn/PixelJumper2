#pragma once

#include <Client/source/resource/pipes/PipeBase.h>

#include <filesystem>
#include <fstream>

#include <Client/source/logger/Logger.h>

#define FILE_BUFFER_SIZE 1024 * 16

namespace Resource
{
	struct FileDefinition
	{
		std::filesystem::path path;
		uintmax_t size;

		bool resetSize()
		{
			try 
			{
				size = std::filesystem::file_size(path);
			}
			catch (const std::filesystem::filesystem_error error)
			{
				Log::Error(_wcserror(error.code));

				return false;
			}

			return true;
		}

		bool doesExists() const
		{
			try
			{
				return std::filesystem::exists(path);
			}
			catch (const std::filesystem::filesystem_error error)
			{
				Log::Error(_wcserror(error.code));

				return false;
			}
		}
	};

	class FileWritePipe
		:
		public WritePipe
	{
	public:
		FileWritePipe(
			FileDefinition* const fileDefinition)
			:
			definition(fileDefinition),
			buffer(new char[FILE_BUFFER_SIZE]),
			file(
				fileDefinition->path, 
				std::ios::out | std::ios::binary
			)
		{
			memset(buffer, 0xaa, FILE_BUFFER_SIZE);
		}

		~FileWritePipe() override
		{
			if ( isValid() )
			{
				flushBuffer();
			}

			delete[] buffer;
		}

		void writeContent(
			const char* const buffer,
			const sf::Uint64 size) override
		{
			if (consumed > 0)
			{
				const sf::Uint64 storageRemain = FILE_BUFFER_SIZE - consumed;

				fillBuffer(
					buffer,
					storageRemain < size ? storageRemain - size : size
				);

				if (storageRemain >= size)
				{
					definition->size += size;

					return;
				}

				flushBuffer();
			}

			sf::Uint64 remain = size;
			while (remain >= FILE_BUFFER_SIZE)
			{
				enforceContent(
					buffer + size - remain,
					FILE_BUFFER_SIZE);

				remain -= FILE_BUFFER_SIZE;
			}

			if (remain)
			{
				fillBuffer(
					buffer + size - remain,
					remain);
			}

			definition->size += size;
		}

		void setPosition(
			const sf::Uint64 position) override
		{
			WritePipe::setPosition(position);
			file.seekp(position);
		}

		sf::Uint64 getSize() const override
		{
			return definition->size;
		}

		void reserveSize(
			const sf::Uint64 size) override
		{
		}

		bool isValid() const
		{
			return file.good();
		}

	private:
		void fillBuffer(
			const char* const buffer,
			const sf::Uint64 size)
		{
			memcpy(
				this->buffer + consumed,
				buffer,
				size);

			consumed += size;
		}

		void flushBuffer()
		{
			enforceContent(
				buffer,
				consumed);

			consumed = 0;
		}

		void enforceContent(
			const char* const buffer,
			const sf::Uint64 size)
		{
			file.write(
				buffer,
				size);
		}

		FileDefinition* const definition;
		std::ofstream file;

		char* buffer;
		sf::Uint64 consumed = 0;
	};

	class FileReadPipe
		:
		public ReadPipe
	{
	public:
		FileReadPipe(
			const FileDefinition* const fileDefinition)
			:
			definition(fileDefinition),
			file(fileDefinition->path, std::ios::in | std::ios::binary)
		{
			buffer = new char[FILE_BUFFER_SIZE];
		}

		~FileReadPipe() override
		{
			delete[] buffer;
		}

		sf::Uint64 getSize() const override
		{
			return definition->size;
		}
		
		sf::Int64 readContent(
			char *buffer, 
			const sf::Uint64 size) override
		{
			sf::Uint64 position = 0;

			if (filled > 0)
			{
				if (filled < size)
				{
					position = filled;
					readBuffer(
						buffer,
						filled
					);
				}
				else
				{
					readBuffer(
						buffer,
						size
					);

					return size;
				}
			}

			while (size - position > FILE_BUFFER_SIZE)
			{
				const sf::Uint64 result = readFile(
					buffer + position,
					FILE_BUFFER_SIZE);

				if (result != FILE_BUFFER_SIZE)
				{
					return position + result;
				}

				position += FILE_BUFFER_SIZE;
			}

			fillBuffer();

			if (size - position != 0)
			{
				readBuffer(
					buffer,
					size - position
				);
			}

			return size;
		}

		bool isValid() const
		{
			return file.good();
		}

	private:
		void fillBuffer()
		{
			filled = readFile(
				buffer,
				FILE_BUFFER_SIZE);
			position = 0;
		}

		sf::Uint64 readFile(
			char* buffer,
			sf::Uint64 size)
		{
			const sf::Uint64 remainingFile = definition->size - file.tellg();

			if (remainingFile < size)
			{
				size = remainingFile;
			}

			file.read(
				buffer,
				size);

			return size;
		}

		void readBuffer(
			char* buffer,
			const sf::Uint64 size)
		{
			memcpy(
				buffer,
				this->buffer + position,
				size
			);

			filled -= size;
			position += size;
		}

		const FileDefinition* definition;
		std::ifstream file;

		char* buffer;
		sf::Uint64 filled, position;
	};
}
