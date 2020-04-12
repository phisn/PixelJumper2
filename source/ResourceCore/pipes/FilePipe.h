#pragma once

#include "Logger/Logger.h"

#include "PipeBase.h"

#include <filesystem>
#include <fstream>

namespace Resource
{
	class FileWritePipe
		:
		public WritePipe
	{
	public:
		FileWritePipe(std::filesystem::path path)
			:
			file(path, std::ios::out | std::ios::binary)
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
			this->size += size;
		}

		sf::Uint64 getSize() const override
		{
			return size;
		}

		sf::Uint64 getPosition() override
		{
			return file.tellp();
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
		sf::Uint64 size;
		std::ofstream file;
	};

	class FileReadPipe
		:
		public ReadPipe
	{
	public:
		FileReadPipe(std::filesystem::path path)
			:
			file(path, std::ios::in | std::ios::binary)
		{
		}

		~FileReadPipe() override
		{
		}

		sf::Uint64 getSize() const override
		{
			return size;
		}

		sf::Uint64 getPosition() override
		{
			return file.tellg();
		}

		sf::Int64 readContent(
			char* buffer,
			sf::Uint64 size) override
		{
			std::streampos position = file.tellg();
			sf::Uint64 realSize = size <= this->size - position
				? this->size
				: this->size - position;

			file.read(buffer, realSize);

			return realSize;
		}

		bool isValid() const
		{
			return file.good();
		}

	private:
		sf::Uint64 size;
		std::ifstream file;
	};
}
