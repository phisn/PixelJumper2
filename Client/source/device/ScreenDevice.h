#pragma once

#include <Client/source/resource/ResourceBase.h>

#include <SFML/Graphics.hpp>

namespace Device
{
	struct ScreenResource
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

		} Settings;

	private:
		bool make(Resource::ReadPipe* const pipe) override
		{
			return pipe->readValue(&Settings);
		}

		bool save(Resource::WritePipe* const pipe) override
		{
			return pipe->writeValue(&Settings);
		}
	};

	namespace Screen
	{
		bool Initialize();
		void Uninitialize();

		bool RecreateWindow();
		bool ReloadSettings();
		bool ApplySettings(ScreenResource* const resource);

		void CloseWindow();

		void CloseWindow();
		void CreateWindow();

		const sf::RenderWindow* GetWindow();

		void BeginDraw();
		void EndDraw();
		void Draw(const sf::Drawable& drawable);
	}

	/*
	
		TODO: rewirte completely
	
	*/

	const sf::Int32 fullscreen = sf::Style::Fullscreen;
	const sf::Int32 windowed = sf::Style::Close | sf::Style::Titlebar;

	const struct ScreenSettings
	{
		sf::Int32 style;

		sf::VideoMode videoMode;
		sf::ContextSettings settings;
	} defaultScreenSettings =
	{
		windowed,
		sf::VideoMode::getDesktopMode(),
		sf::ContextSettings::ContextSettings(0U, 0U, 0U) // For AA
	};

	class Screen
	{
	public:
		bool initalize();

		void onShow();
		void onHide();

		void onDraw(
			const sf::Drawable* drawable)
		{
			window.draw(*drawable);
		}

		void applyView(
			const sf::View* view)
		{
			window.setView(*view);
		}

		void resetView()
		{
			window.setView(
				window.getDefaultView()
			);
		}

		sf::RenderWindow* getWindow()
		{
			return &window;
		}

		sf::Font* getFont()
		{
			return &font;
		}

	private:
		sf::RenderWindow window;
		sf::Font font;

		ScreenSettings settings;
	};
}
