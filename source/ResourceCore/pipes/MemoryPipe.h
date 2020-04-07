#pragma once

#include "PipeBase.h"

#include "Logger/Logger.h"

#include <SFML/Network/Packet.hpp>

namespace Resource
{
	class MemoryWritePipe
		:
		public WritePipe
	{
	public:
		void reset()
		{
			buffer.clear();

			// if vectors memory is bigger than
			// 512kb free memory
			if (buffer.capacity() > 1024 * 512)
			{
				buffer.shrink_to_fit();
			}

			position = 0;
		}

		sf::Uint64 getSize() const override
		{
			return buffer.size();
		}

		sf::Uint64 getPosition() override
		{
			return position;
		}

		bool isValid() const override
		{
			return true;
		}

		void writeContent(
			const char* const data,
			const sf::Uint64 size) override
		{
			buffer.insert(
				buffer.end(),
				data,
				data + size);

			position += size;
		}

		void reserveSize(const sf::Uint64 size) override
		{
			buffer.reserve(size);
		}

		void assign(std::vector<char>& target)
		{
			target = std::move(buffer);
			position = 0;
		}

		const std::vector<char>& getData() const
		{
			return buffer;
		}

		const char* getRawData() const
		{
			return &buffer[0];
		}

	private:
		std::vector<char> buffer;
		sf::Uint64 position = 0;
	};

	class MemoryReadPipe
		:
		public ReadPipe
	{
	public:
		// need to look yourself to not call
		// until memory was set
		MemoryReadPipe()
		{
		}
		
		MemoryReadPipe(
			const char* const buffer,
			const int length)
			:
			buffer(buffer),
			length(length)
		{
		}
		
		void setMemory(
			const char* const buffer,
			const int length)
		{
			this->buffer = buffer;
			this->length = length;

			position = 0;
		}

		void reset()
		{
			position = 0;
		}

		sf::Uint64 getSize() const override
		{
			return length;
		}

		sf::Uint64 getPosition() override
		{
			return position;
		}

		bool isValid() const override
		{
			return true;
		}

		sf::Int64 readContent(char* const data, const sf::Uint64 size) override
		{
			const sf::Int64 realSize = size <= length - position
				? size
				: length - position;

			memcpy(data, (const char*) buffer + position, realSize);
			position += size;

			return realSize;
		}

		void adopt(const std::vector<char>& buffer)
		{
			if (buffer.size() == 0)
			{
				Log::Error(L"got empty buffer in adopt memorypipe");
				length = 0;
				return;
			}

			this->buffer = &buffer[0];
			length = buffer.size();
		}

	private:
		const char* buffer;
		int length;

		sf::Uint64 position = 0;
	};
}
