#pragma once

#include <Client/source/device/ScreenDevice.h>
#include <Client/source/menu/MenuElementBase.h>

#include <SFML/Graphics/View.hpp>

namespace Menu
{
	class MenuRootBase
		:
		protected ElementBase
	{
	public:
		void initialize() override
		{
			ElementBase::initialize();
			updateGraphics();
		}

		using ElementBase::initialize;
		using ElementBase::onLogic;
		using ElementBase::innerOffset;

		using ElementBase::size;

		using ElementBase::strongSelected;
		using ElementBase::strongSelectedChild;
		using ElementBase::weakSelected;
		using ElementBase::weakSelectedChild;

		using ElementBase::unselectStrong;
		using ElementBase::unselectWeak;
		using ElementBase::selectStrong;
		using ElementBase::selectWeak;

		MenuRootBase()
		{
			viewPort.addListener(
				[this](const sf::FloatRect oldViewPort, 
					   const sf::FloatRect newViewPort)
				{
					view.setViewport(newViewPort);
					const sf::FloatRect realView = ConvertPortToReal(newViewPort);
					
					if (virtualSize.isAutomatic())
					{
						const sf::Vector2u windowSize = Device::Screen::GetWindow()->getSize();

						sf::Vector2f result = sf::Vector2f(
							newViewPort.width * windowSize.x,
							newViewPort.height * windowSize.y
						);

						if (!virtualSize.automatic.x)
						{
							result.x = virtualSize->x;
						}

						if (!virtualSize.automatic.y)
						{
							result.y = virtualSize->y;
						}

						virtualSize.setValue(result);
					}
				});
			virtualSize.addListener(
				[this](const sf::Vector2f oldSize,
					   const sf::Vector2f newSize)
				{
					view.setSize(newSize);
					sizePreferred = newSize;
					view.setCenter(newSize / 2.f);
				});

			viewPort = { 0.f, 0.f, 1.f, 1.f };
		}

		void addElement(ElementBase* const element)
		{
			addChild(element);
		}

		Property<sf::FloatRect> viewPort;
		AutomaticProperty<sf::Vector2f> virtualSize{ sf::Vector2f(0.f, 0.f) };

		const sf::View& readView() const
		{
			return view;
		}

		void draw(sf::RenderTarget* const target) const
		{
			Device::Screen::SetView(view);
			onDraw(target);
			Device::Screen::ResetView();
		}

		void update()
		{
			updateGraphics();
		}

		void onEvent(sf::Event event) override
		{
			switch (event.type)
			{
			case sf::Event::MouseMoved:
			{
				const sf::Vector2f position = Device::Screen::GetWindow()->mapPixelToCoords(sf::Vector2i(
					event.mouseMove.x,
					event.mouseMove.y),
					view);

				event.mouseMove.x = position.x;
				event.mouseMove.y = position.y;
			}
				break;
			case sf::Event::MouseButtonPressed:
			{
				const sf::Vector2f position = Device::Screen::GetWindow()->mapPixelToCoords(sf::Vector2i(
					event.mouseButton.x,
					event.mouseButton.y),
					view);

				event.mouseButton.x = position.x;
				event.mouseButton.y = position.y;
			}
				break;
			case sf::Event::MouseButtonReleased:
			{
				const sf::Vector2f position = Device::Screen::GetWindow()->mapPixelToCoords(sf::Vector2i(
					event.mouseButton.x,
					event.mouseButton.y),
					view);

				event.mouseButton.x = position.x;
				event.mouseButton.y = position.y;
			}
				break;
			}

			ElementBase::onEvent(event);
		}

		static sf::FloatRect ConvertRealToPort(const sf::FloatRect rect)
		{
			const sf::Vector2u windowSize = Device::Screen::GetWindow()->getSize();

			return
			{
				rect.left / windowSize.x,
				rect.top / windowSize.y,
				rect.width / windowSize.x,
				rect.height / windowSize.y
			};
		}

		static sf::FloatRect ConvertPortToReal(const sf::FloatRect rect)
		{
			const sf::Vector2u windowSize = Device::Screen::GetWindow()->getSize();

			return
			{
				rect.left * windowSize.x,
				rect.top * windowSize.y,
				rect.width * windowSize.x,
				rect.height * windowSize.y
			};
		}

		// called in Scene::onCreate
		// to build the super ui
		virtual bool build() = 0;

		void viewPortCenteredByVirtualSize(const CommonElementOffset outerOffset)
		{
			const sf::Vector2u window = Device::Screen::GetWindow()->getSize();
			const float windowRatio = window.x / window.y;
			const float virtualRatio = virtualSize->x / virtualSize->y;

			if (windowRatio > virtualRatio)
			{
				/*
					w
					+----------------+
					|                |
					|                |
					+----------------+

					c
					+-------------+
					|             |
					|             |
					|             |
					+-------------+

				-> width gets smaller
				-> positionX gets higher
				*/

				const float element_real_height = window.y * (1.f - outerOffset.top - outerOffset.bottom);
				const float element_height_ratio = virtualSize->y / virtualSize->x;
				const float element_real_width = element_real_height / element_height_ratio;
				const float element_width = element_real_width / window.x;

				if (outerOffset.left == 0)
				{
					viewPort =
					{
						0.f,
						outerOffset.top,
						element_width,
						1.f - outerOffset.top - outerOffset.bottom
					};
				}
				else if (outerOffset.right == 0)
				{
					viewPort =
					{
						1.f - element_width,
						outerOffset.top,
						element_width,
						1.f - outerOffset.top - outerOffset.bottom
					};
				}
				else
				{
					const float element_distance_ratio_left = outerOffset.left / outerOffset.right;
					const float element_distance_ratio_right = outerOffset.right / outerOffset.left;

					const float element_distance_sum = 1.f - element_width;
					const float element_distance_left = (element_distance_sum
						* element_distance_ratio_left) / (1 + element_distance_ratio_left);
					const float element_distance_right = (element_distance_sum
						* element_distance_ratio_right) / (1 + element_distance_ratio_right);

					viewPort =
					{
						element_distance_left,
						outerOffset.top,
						element_width,
						1.f - outerOffset.top - outerOffset.bottom
					};
				}
			}
			else if (windowRatio < virtualRatio)
			{/*
					w
					+-------------+
					|             |
					|             |
					|             |
					+-------------+

					c
					+----------------+
					|                |
					|                |
					+----------------+

				-> height gets smaller
				-> positionY gets higher
				*/

				const float element_real_width = window.x * (1.f - outerOffset.left - outerOffset.right);
				const float element_width_ratio = virtualSize->x / virtualSize->y;
				const float element_real_height = element_real_width / element_width_ratio;
				const float element_height = element_real_height / window.y;

				if (outerOffset.top == 0)
				{
					viewPort =
					{
						outerOffset.left,
						0.f,
						1.f - outerOffset.left - outerOffset.right,
						element_height,
					};
				}
				else if (outerOffset.bottom == 0)
				{
					viewPort =
					{
						outerOffset.left,
						1.f - element_height,
						1.f - outerOffset.left - outerOffset.right,
						element_height
					};
				}
				else
				{
					const float element_distance_ratio_top = outerOffset.top / outerOffset.bottom;
					const float element_distance_ratio_bottom = outerOffset.bottom / outerOffset.top;

					const float element_distance_sum = 1.f - element_height;
					const float element_distance_top = (element_distance_sum
						* element_distance_ratio_top) / (1 + element_distance_ratio_top);
					const float element_distance_bottom = (element_distance_sum
						* element_distance_ratio_top) / (1 + element_distance_ratio_bottom);

					viewPort =
					{
						outerOffset.left,
						element_distance_top,
						1.f - outerOffset.left - outerOffset.right,
						element_height
					};
				}
			}
			else
			{
				viewPort =
				{
					outerOffset.left,
					outerOffset.top,
					1.f - outerOffset.left - outerOffset.right,
					1.f - outerOffset.top - outerOffset.bottom
				};
			}
		}

	protected:
		// virtual to real
		sf::Vector2f convertPositionVTR(const sf::Vector2f position) const
		{
			return position;
		}

		// virtual to real
		sf::Vector2f convertFullPositionVTR(const sf::Vector2f position) const
		{
			return position + sf::Vector2f(innerOffset->left, innerOffset->top);;
		}

		// real to virtual
		sf::Vector2f convertPositionRTV(const sf::Vector2f position) const
		{
			return position;
		}

		// real to virtual
		sf::Vector2f convertFullPositionRTV(const sf::Vector2f position) const
		{
			return position - sf::Vector2f(innerOffset->left, innerOffset->top);
		}

		sf::View view;
	};
}
