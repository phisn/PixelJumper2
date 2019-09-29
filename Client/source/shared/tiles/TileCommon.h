#pragma once

namespace Shared
{
	namespace Default
	{
		constexpr float collidable_density = 0.000027f;
		constexpr float collidable_friction = 0.000010f;
		constexpr float collidable_inputForce = 1.706649f;
	}

	enum class TileId
	{
		_Invalid = 0,
		_Begin = 1,

		TileWall = 1,

		_Length
	}; 
}

