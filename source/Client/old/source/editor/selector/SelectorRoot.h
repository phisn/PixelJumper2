#pragma once

#include <Client/source/editor/manipulator/EditorWorld.h>
#include <Client/source/editor/manipulator/Executor.h>

#include <Client/source/editor/manipulator/tasks/TilePlace.h>

#include <Client/source/editor/selector/SelectorMarker.h>
#include <Client/source/editor/selector/SelectorView.h>

#include <Client/source/menu/MenuRootBase.h>

namespace Editor
{
	class SelectorRoot
		:
		public Menu::MenuRootBase
	{
	public:
		SelectorRoot()
			:
			MenuRootBase(),
			selectorMarker(view),
			selectorView(view)
		{
		}

		bool build() override
		{

			return true;
		}

		void onDraw(sf::RenderTarget* const target) const override
		{
			MenuRootBase::onDraw(target);

			for (TileBase* const tile : Manipulator::GetWorld()->getTiles())
			{
				tile->draw(target);
			}

			selectorMarker.draw(target);
			selectorView.draw(target);
		}

		void onEvent(const sf::Event event) override
		{
			MenuRootBase::onEvent(event);

			switch (event.type)
			{
			case sf::Event::MouseWheelMoved:
				selectorView.zoom(1.f - 0.1f / (float) event.mouseWheel.delta);
				mouse.right = false; // bug by view movement and cursor handling

				break;
			case sf::Event::MouseButtonPressed:
				mouse.samePosition = true;

				if (event.mouseButton.button == sf::Mouse::Right)
				{
					mouse.right = true;
					selectorView.beginMouseMovement(
						sf::Vector2i(event.mouseButton.x, event.mouseButton.y)
					);

					break;
				}

				if (event.mouseButton.button == sf::Mouse::Left)
				{
					mouse.left = true;
					selectorMarker.beginMouseMovement(
						sf::Vector2i(event.mouseButton.x, event.mouseButton.y)
					);
					selectorMarker.nextMouseMovement(
						sf::Vector2i(event.mouseButton.x, event.mouseButton.y)
					);

					break;
				}

				break;
			case sf::Event::MouseMoved:
				mouse.samePosition = false;
				old_mouse.samePosition = false;

				if (mouse.right)
				{
					selectorView.nextMouseMovement(
						sf::Vector2i(event.mouseMove.x, event.mouseMove.y)
					);
				}

				if (mouse.left)
				{
					selectorMarker.nextMouseMovement(
						sf::Vector2i(event.mouseMove.x, event.mouseMove.y)
					);
				}

				break;
			case sf::Event::MouseButtonReleased:
				if (event.mouseButton.button == sf::Mouse::Right)
				{
					mouse.right = false;
					break;
				}

				if (event.mouseButton.button == sf::Mouse::Left)
				{
					if (mouse.samePosition && old_mouse.samePosition && old_mouse.left )
					{
						if (!Manipulator::GetExecutor()->execute<TilePlace>())
						{
							Log::Error(L"Failed to quick-place (inside selection) tiles");
						}
					}
					else
					{
						old_mouse = mouse;
					}


					mouse.left = false;
					break;
				}

				break;
			}

		}

	private:
		struct Mouse
		{
			bool samePosition;
			bool left, right;

		} mouse = { }, old_mouse = { };

		SelectorView selectorView;
		SelectorMarker selectorMarker;
	};
}
