#pragma once

#include <SFML/Graphics.hpp>

namespace Framework
{
	struct Scene
	{
		virtual ~Scene() { }
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

		void PushScene(Scene* const scene);
		void PopScene();

		void PushChildScene(
			Scene* const scene,
			const bool haltParentScene = false);
		void PopChildScene();
		void FallbackChildScene();

		template <typename T, class... Args>
		inline void PushScene(Args... args)
		{
			T* const value = new T(std::forward<Args>(args)...);
			PushScene(value);
		}

		template <typename T, class... Args>
		inline void PushChildScene(Args... args)
		{
			T* const value = new T(std::forward<Args>(args)...);
			PushChildScene(value);
		}
	}

	const sf::Font& GetFont();

	inline sf::FloatRect MakeViewRect(const sf::View& view)
	{
		return
		{
			view.getCenter().x - view.getSize().x / 2.f,
			view.getCenter().y - view.getSize().y / 2.f,
			view.getSize().x,
			view.getSize().y
		};
	}
}
