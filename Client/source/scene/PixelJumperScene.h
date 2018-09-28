#pragma once

#include <Client/source/PixelJumper/PixelJumperContext.h>
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
			Base(Type::SFML),
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
		void onDraw() override;
	private:
		sf::RectangleShape shape;
		// direct access on context (friend)
		PJ::Context* context;
	};
}
