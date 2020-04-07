#pragma once

#include <SFML/Graphics.hpp>

namespace Framework
{
	struct Scene
	{
		virtual ~Scene() { }

		// oncreate can fail but can not call frameworkcore
		// initialize cant fail but can call frameworkcore
		virtual bool onCreate() = 0;
		virtual void initialize() { };

		virtual void onEvent(const sf::Event event) = 0;
		virtual void onLogic(const sf::Time time) = 0;
		virtual void onDraw(sf::RenderTarget* const target) = 0;

		virtual void onShow() { };
		virtual void onHide() { };
	};

	namespace Core
	{
		bool Initialize();
		void Uninitialize();

		bool IsRunning();
		void Shutdown();

		void ProcessEvent(const sf::Event event);
		void ProcessLogic(const sf::Time time);
		void ProcessDraw(sf::RenderTarget* const target);

		bool PushScene(Scene* const scene);
		void PopScene();

		bool PushTemporaryScene(
			Scene* const scene,
			const bool haltMainScene = false);
		void PopTemporaryScene();
		void FallbackTemporaryScene();

		template <typename T, class... Args>
		inline bool PushScene(Args... args)
		{
			T* const value = new T(std::forward(args)...);

			if (!PushScene(value))
			{
				delete value;
				return false;
			}

			return true;
		}

		template <typename T, class... Args>
		inline bool PushTemporaryScene(Args... args)
		{
			T* const value = new T(std::forward(args)...);

			if (!PushTemporaryScene(value))
			{
				delete value;
				return false;
			}

			return true;
		}
	}
}
