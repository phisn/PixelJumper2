#pragma once

#include <Client/source/resource/PlayerResource.h>

namespace Game::Net
{
	constexpr size_t NetMovementCaptureTime = 10; // 10 ms
	constexpr size_t NetMovementPushCount = 100; // every 1s

	// movement is sent in one per 10 ms
	// allowing to be fluid up to 100 fps
	class NetPlayerMovement
		:
		public Resource::ResourceBase
	{
	public:
		struct Content
		{
			Resource::PlayerID playerID;
			Resource::WorldId worldID;

		} content;

		std::vector<sf::Vector2f> positions;

		bool make(Resource::ReadPipe* const pipe) override
		{
			return pipe->readValue(&content)
				&& pipe->readVector(&positions);
		}

		bool save(Resource::WritePipe* const pipe) override
		{
			return pipe->writeValue(&content)
				&& pipe->writeVector(&positions);
		}

		bool setup() override
		{
			return true;
		}

		bool validate() override
		{
			return true;
		}
	};
}
