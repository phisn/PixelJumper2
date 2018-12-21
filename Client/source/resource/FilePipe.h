#pragma once

#include <Client/source/resource/PipeBase.h>

#include <filesystem>
#include <fstream>

#define FILE_BUFFER_SIZE 1024 * 16

namespace Resource
{
	struct FileDefinition
	{
		FileDefinition(
			const std::filesystem::path path)
			:
			path(path)
		{
			size = std::filesystem::file_size(path);
		}

		std::filesystem::path path;
		uintmax_t size;
	};

	class FileWritePipe
		:
		public WritePipe
	{
	public:
		FileWritePipe(
			const FileDefinition fileDefinition)
			:
			definition(fileDefinition),
			buffer(new char[FILE_BUFFER_SIZE])
		{
		}

		~FileWritePipe() override
		{
			delete[] buffer;
		}

		void writeContent(
			char* buffer,
			const sf::Uint64 size) override
		{
			if (filled > 0)
			{
				if (FILE_BUFFER_SIZE - filled <= size)
				{
					fillBuffer(
						buffer,
						size);
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
		}

		void setPosition(
			const sf::Uint64 position) override
		{
			WritePipe::setPosition(position);
			file.seekp(position);
		}

		sf::Uint64 getSize() const override
		{
			return definition.size;
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
			char* buffer,
			const sf::Uint64 size)
		{
			memcpy(
				this->buffer,
				buffer + filled,
				size);

			filled += size;
		}

		void flushBuffer(
			const sf::Uint64 size = FILE_BUFFER_SIZE)
		{
			enforceContent(
				buffer,
				size);

			filled = 0;
		}

		void enforceContent(
			char* buffer,
			const sf::Uint64 size)
		{
			file.write(
				buffer,
				size);
		}

		FileDefinition definition;
		std::ofstream file;

		char* buffer;
		int filled = 0;
	};

	class FileReadPipe
		:
		public ReadPipe
	{
	public:
		FileReadPipe(
			const FileDefinition fileDefinition)
			:
			definition(fileDefinition)
		{
		}

		~FileReadPipe() override
		{
		}

		sf::Uint64 getSize() const override
		{
			return definition.size;
		}
		
		void readContent(
			char *buffer, 
			const sf::Uint64 size)
		{

		}

		bool isValid() const
		{
			return file.good();
		}

	private:
		FileDefinition definition;
		std::ifstream file;
	};
}
