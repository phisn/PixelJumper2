#pragma once

#include <Client/source/logger/Logger.h>

#include <Client/source/resource/pipes/PipeBase.h>

#include <filesystem>
#include <fstream>

namespace Resource
{
	struct FileDefinition
	{
		FileDefinition() = default;
		FileDefinition(const std::filesystem::path path)
			:
			path(path)
		{
		}

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
				Log::Error(_wcserror(error.code().value()));

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
				Log::Error(_wcserror(error.code().value()));

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
			file(
				fileDefinition->path, 
				std::ios::out | std::ios::binary
			)
		{
		}

		~FileWritePipe() override
		{
		}

		void writeContent(
			const char* const buffer,
			const sf::Uint64 size) override
		{
			file.write(buffer, size);
		}

		void setPosition(
			const sf::Uint64 position) override
		{
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
		FileDefinition* const definition;
		std::ofstream file;
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
		}

		~FileReadPipe() override
		{
		}

		sf::Uint64 getSize() const override
		{
			return definition->size;
		}
		
		sf::Int64 readContent(
			char *buffer, 
			const sf::Uint64 size) override
		{
			const sf::Uint64 diff = definition->size - file.tellg();

			if (diff < size)
			{
				file.read(buffer, diff);
				return diff;
			}
			else
			{
				file.read(buffer, size);
				return size;
			}
		}

		bool isValid() const
		{
			return file.good();
		}

	private:
		const FileDefinition* definition;
		std::ifstream file;
	};
}
