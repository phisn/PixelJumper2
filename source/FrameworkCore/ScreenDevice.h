#pragma once

#include "ResourceCore/TrivialResource.h"

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

namespace Device
{
	struct ScreenResourceContent
	{
		sf::VideoMode videomode;
		sf::ContextSettings settings;

		bool windowed;

		enum Limit
		{
			VSync,
			Framelimit,
			None

		} limit;

		sf::Uint8 framelimit;
	};

	struct ScreenResource
		:
		public Resource::TrivialResource<ScreenResourceContent>
	{
		bool setup() override
		{
			return content.limit <= content.None;
		}
		
		bool validate() override
		{
			return content.limit <= content.None;
		}
	};

	namespace Screen
	{
		bool Initialize();
		void Uninitialize();

		ScreenResourceContent& ChangeScreenResource();

		void LoadScreenResource();
		bool SaveScreenResource();
		void ResetScreenResource();

		sf::RenderWindow* GetWindow();
	}

	extern const ScreenResourceContent DefaultScreenResource;
}
