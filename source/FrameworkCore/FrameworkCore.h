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

	template <size_t size>
	inline bool HasConvexPoint(sf::Vector2f point, sf::Vector2f(&convex)[size])
	{
		for (int i = 0; i < size - 1; ++i)
			if ((convex[i + 1].x - convex[i].x) * (point.y - convex[i].y) - (point.x - convex[i].x) * (convex[i + 1].y - convex[i].y) >= 0)
			{
				return false;
			}

		return (convex[0].x - convex[size - 1].x) * (point.y - convex[size - 1].y) 
			 - (point.x - convex[size - 1].x) * (convex[0].y - convex[size - 1].y) < 0;
	}

	inline bool HasRectPoint(sf::Vector2f point, 
		sf::Vector2f r1, 
		sf::Vector2f r2, 
		sf::Vector2f r3,
		sf::Vector2f r4)
	{
		return (r2.x - r1.x) * (point.y - r1.y) - (point.x - r1.x) * (r2.y - r1.y) < 0
			&& (r3.x - r2.x) * (point.y - r2.y) - (point.x - r2.x) * (r3.y - r2.y) < 0
			&& (r4.x - r3.x) * (point.y - r3.y) - (point.x - r3.x) * (r4.y - r3.y) < 0
			&& (r1.x - r4.x) * (point.y - r4.y) - (point.x - r4.x) * (r1.y - r4.y) < 0;
	}

	inline bool HasTriPoint(sf::Vector2f point,
		sf::Vector2f r1,
		sf::Vector2f r2,
		sf::Vector2f r3)
	{
		return (r2.x - r1.x) * (point.y - r1.y) - (point.x - r1.x) * (r2.y - r1.y) < 0
			&& (r3.x - r2.x) * (point.y - r2.y) - (point.x - r2.x) * (r3.y - r2.y) < 0
			&& (r1.x - r3.x) * (point.y - r3.y) - (point.x - r3.x) * (r1.y - r3.y) < 0;
	}
}
