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

		void setPosition(sf::Vector2f position)
		{
			view.setCenter(sf::Vector2f(
				position.x,
				position.y
			) + sf::Vector2f{ view.getSize().x, view.getSize().y } * 0.5f);
		}

		void setRotation(const float angle)
		{
			view.setRotation(angle);
		}

		virtual void setSize(const float width, const float height)
		{
			view.setSize(width, height);
		}
		
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
			return view.getCenter() - sf::Vector2f{ view.getSize().x, view.getSize().y } / 2.f;
		}

		const sf::Vector2f& getSize() const
		{
			return view.getSize();
		}

		sf::FloatRect getRect() const
		{
			return
			{
				getCenter().x - getSize().x / 2.f,
				getCenter().y - getSize().y / 2.f,
				view.getSize().x,
				view.getSize().y
			};
		}

		const sf::Vector2f& getCenter() const
		{
			return view.getCenter();
		}

		const sf::FloatRect& getViewport() const
		{
			return view.getViewport();
		}

		const sf::View& getView() const
		{
			return view;
		}

		void zoom(float factor)
		{
			view.zoom(factor);
		}

	protected:
		sf::View view;
	};
}
