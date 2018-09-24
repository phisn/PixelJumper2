#pragma once

#include <Client/source/scene/PixelJumper/PixelJumperContext.h>
#include <Client/source/scene/SceneBase.h>

namespace Scene
{
	class PixelJumper
		:
		public Base
	{
	public:
		PixelJumper(
			PixelJumperContext context);
		~PixelJumper();
	};
}
