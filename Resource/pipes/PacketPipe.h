#pragma once

#include "PipeBase.h"

#include <SFML/Network/Packet.hpp>

namespace Resource
{
	class PacketWritePipe
		:
		public WritePipe
	{
	public:
		void reset()
		{
			packet.clear();
			position = 0;
		}

		sf::Uint64 getSize() const override
		{
			return packet.getDataSize();
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
			const char* const buffer, 
			const sf::Uint64 size) override
		{
			packet.append(buffer, size);
			position += size;
		}

		void reserveSize(const sf::Uint64 size) override
		{
		}

		sf::Packet& getPacket()
		{
			return packet;
		}

	protected:
		sf::Packet packet;

	private:
		sf::Uint64 position = 0;
	};

	class PacketReadPipe
		:
		public ReadPipe
	{
	public:
		void reset()
		{
			packet.clear();
			position = 0;
		}

		sf::Uint64 getSize() const override
		{
			return packet.getDataSize();
		}

		sf::Uint64 getPosition() override
		{
			return position;
		}

		bool isValid() const override
		{
			return true;
		}

		sf::Int64 readContent(char* const buffer, const sf::Uint64 size) override
		{
			const sf::Int64 realSize = size <= packet.getDataSize() - position
				? size
				: packet.getDataSize() - position;

			memcpy(buffer, (char*) packet.getData() + position, realSize);
			position += size;

			return realSize;
		}

		sf::Packet& getPacket()
		{
			return packet;
		}

	protected:
		sf::Packet packet;

	private:
		sf::Uint64 position = 0;
	};
}
