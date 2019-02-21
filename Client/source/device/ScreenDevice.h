#pragma once

#include <Client/source/resource/ResourceBase.h>

#include <SFML/Graphics.hpp>

namespace Device
{
	struct ScreenSettings
		:
		public Resource::ResourceBase
	{
	public:
		struct
		{
			// behavior
			bool hideCursor;
			bool useCustomCursor;
			bool pauseOnLeave;

			// resolution / window type
			bool useFullscreen;

			bool useDesktopVideoMode;
			sf::VideoMode customVideoMode;
			
			// frame rate
			bool useVSync;

			bool useFramelimit;
			sf::Uint8 framelimit;

			// specific context settings
			sf::Uint8 antialiasing;

		} Content;

	private:
		bool make(Resource::ReadPipe* const pipe) override
		{
			return pipe->readValue(&Content);
		}

		bool save(Resource::WritePipe* const pipe) override
		{
			return pipe->writeValue(&Content);
		}
	};

	namespace Screen
	{
		bool Initialize();
		void Uninitialize();

		bool RecreateWindow();
		bool ReloadSettings();

		void _CloseWindow();
		void _CreateWindow();

		const sf::RenderWindow* GetWindow();

		void BeginDraw();
		void EndDraw();
		void Draw(const sf::Drawable& drawable);

		bool PollEvent(sf::Event& event);
	}
}
