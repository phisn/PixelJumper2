#pragma once

#include <Client/source/resource/pipes/PipeBase.h>

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

	private:
		std::vector<char> buffer;
		sf::Uint64 position = 0;
	};

	class MemoryReadPipe
		:
		public ReadPipe
	{
	public:
		MemoryReadPipe(
			const char* const buffer,
			const int length)
			:
			buffer(buffer),
			length(length)
		{
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

	private:
		const char* const buffer;
		const int length;

		sf::Uint64 position = 0;
	};
}
