#pragma once

#include <Client/source/resource/TileResource.h>
#include <Client/source/resource/ResourceBase.h>

#include <SFML/Main.hpp>

#include <string>
#include <vector>

#define WORLD_MAGIC 0xbb'22'cc'33

namespace Resource
{
	struct World
		:
		public Base
	{
		struct
		{
			struct
			{
				sf::Uint32 magic = WORLD_MAGIC;
				sf::Uint32 worldId;
			} begin;

			struct
			{
				std::wstring author;
				std::wstring name;
			} auth;

			struct
			{
				float width;
				float height;

				sf::Int32 tileCount;
				sf::Int32 contentSize;
			} prop;
		} header;

		struct
		{
			std::vector<Tile> tileResources;

		} content;

		bool writeToBuffer(
			RESOURCE::ByteWriter* buffer) override
		{
			if (!setupWriteData())
			{
				return false;
			}

			if (!validateHeader() ||
				!validateContent())
			{
				return false;
			}

			writeHeader(buffer);

			if (!writeContent(buffer))
			{
				return false;
			}

			return true;
		}

		bool readFromBuffer(
			ByteReader* buffer) override
		{
			if (!readHeader(buffer))
			{
				return false;
			}

			if (!validateHeader())
			{
				return false;
			}

			if (!readContent(buffer))
			{
				return false;
			}

			return validateContent();
		}

		bool validateHeader() const
		{
			if (header.auth.author.length() > 0xff ||
				header.auth.name.length() > 0xff)
			{
				return false;
			}

			return header.begin.magic == WORLD_MAGIC;
		}

		bool validateContent() const
		{
			return true;
		}
	private:
		bool readHeader(
			ByteReader* buffer)
		{
			if (!buffer->readValue( &header.begin ))
			{
				return false;
			}

			if (!buffer->readString(&header.auth.author) ||
				!buffer->readString(&header.auth.name))
			{
				return false;
			}

			return buffer->readValue(&header.prop);
		}

		bool readContent(
			ByteReader* buffer)
		{
			for (int i = 0; i < header.prop.tileCount; ++i)
			{
				content.tileResources.emplace_back();

				if (!content.tileResources.back().readFromBuffer(
						buffer))
				{
					return false;
				}
			}

			return true;
		}

		bool setupWriteData()
		{
			header.begin.magic = WORLD_MAGIC;

			header.prop.contentSize;
			header.prop.tileCount = content.tileResources.size();

			return true;
		}

		void writeHeader(
			ByteWriter* buffer)
		{
			buffer->writeValue( &header.begin );

			buffer->write(header.auth.author);
			buffer->write(header.auth.name);

			buffer->writeValue( &header.prop );
		}

		bool writeContent(
			RESOURCE::ByteWriter* buffer)
		{
			for (Tile& resource : content.tileResources)
				if (!resource.writeToBuffer(
						buffer))
				{
					return false;
				}

			return true;
		}
	};
}
