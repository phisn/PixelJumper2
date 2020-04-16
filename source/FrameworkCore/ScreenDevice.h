#pragma once

#include "ResourceCore/TrivialResource.h"

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

namespace Resource
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

	typedef TrivialResource<ScreenResourceContent> ScreenResource;
}

namespace Device
{
	namespace Screen
	{
		bool Initialize();
		void Uninitialize();

		Resource::ScreenResourceContent& ChangeScreenResource();

		void LoadScreenResource();
		bool SaveScreenResource();
		void ResetScreenResource();

		sf::RenderWindow* GetWindow();
	}

	extern const Resource::ScreenResourceContent DefaultScreenResource;

	// implemted in viewchain
	class View
	{
	public:
		void setCenter(const sf::Vector2f position)
		{
			view.setCenter(position);
		}
		
		void setRect(const sf::FloatRect rect)
		{
			setSize(
			{
				rect.width,
				rect.height
			});

			view.setCenter(sf::Vector2f(
				rect.left,
				rect.top
			) + view.getSize() * 0.5f);
		}

		void setRotation(const float angle)
		{
			view.setRotation(angle);
		}

		virtual void setSize(const float width, const float height) = 0;
		
		void setSize(const sf::Vector2f& size)
		{
			setSize(size.x, size.y);
		}

		void enable(sf::RenderTarget* const target) const
		{
			target->setView(view);
		}

		const sf::Vector2f& getPosition() const
		{
			return view.getCenter() - view.getSize() / 2.f;
		}

		const sf::Vector2f& getSize() const
		{
			return view.getSize();
		}

		const sf::FloatRect& getViewport() const
		{
			return view.getViewport();
		}

	protected:
		sf::View view;
	};
}
