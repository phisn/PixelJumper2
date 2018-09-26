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
			::PixelJumper::Context* context)
			:
			context(context)
		{
		}

		bool onCreate() override;
		bool onRemove() override;
		void onShow() override;
		void onHide() override;
		void onEvent(
			sf::Event event) override;
		void onLogic(
			sf::Time time) override;
		void onDraw(
			::Device::Window* window) override;
	private:
		::PixelJumper::Context* context;
	};
}
