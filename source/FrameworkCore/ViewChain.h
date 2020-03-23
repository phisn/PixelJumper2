#pragma once

#include "ScreenDevice.h"

#include <cassert>

namespace Framework
{
	class ViewChain
	{
		class ViewChainView
			:
			public Device::View
		{
		public:
			ViewChainView(ViewChain* const parent)
				:
				parent(parent)
			{
			}

			void setSize(const float width, const float height) override
			{
				view.setSize(width, height);
				parent->remakeViewViewports();
			}

			sf::View& getView()
			{
				return view;
			}

		private:
			ViewChain* const parent;
		};

	public:
		void resizeViews(const size_t count)
		{
			assert(count <= 4 && count != 0);

			if (count < views.size())
			{
				views.resize(count);
			}
			else
			{

			}

			remakeViewViewports();
		}

		void setViewport(const sf::FloatRect viewport)
		{
			this->viewport = viewport;
			remakeViewViewports();
		}

		sf::FloatRect getViewport() const
		{
			return viewport;
		}

		Device::View& getView(const size_t index)
		{
			return views[index];
		}

		size_t getViewCount() const
		{
			return views.size();
		}

	private:
		std::vector<ViewChainView> views;
		sf::FloatRect viewport = { 0.f, 0.f, 1.f, 1.f };

		void remakeViewViewports()
		{
			std::vector<sf::FloatRect> windowViewRects = getWindowViewRects();

			const sf::Vector2f window = sf::Vector2f(Device::Screen::GetWindow()->getSize());

			for (int i = 0; i < views.size(); ++i)
			{
				const sf::Vector2f view_max_size_as_viewport =
				{
					viewport.width * windowViewRects[i].width,
					viewport.height * windowViewRects[i].height
				};

				const float view_max_ratio =
					view_max_size_as_viewport.x / view_max_size_as_viewport.y;

				const sf::Vector2f view_size = views[i].getSize();

				const float view_size_ratio =
					view_size.x / view_size.y;

				if (view_max_ratio > view_size_ratio)
				{
					// view is too high

					// additionally using double for size_to_viewport to
					// prevent loss between conversion from small numbers to
					// high ones

					// conversion between view size to viewport derived
					// from view size height as full height
					// view_size_to_viewport * view_size.y == view_max_viewport.y
					const double view_size_to_viewport =
						(double)view_max_size_as_viewport.y / (double) view_size.y;

					const float view_final_size_x_as_viewport = (float) (view_size_to_viewport * (double) view_size.x);
					sf::FloatRect view_final_viewport =
					{
						(view_max_size_as_viewport.x - view_final_size_x_as_viewport) / 2.f,
						0,
						view_final_size_x_as_viewport,
						view_max_size_as_viewport.y
					};

					// convert relative to real position
					view_final_viewport.left += windowViewRects[i].left * viewport.width + viewport.left;
					view_final_viewport.top += windowViewRects[i].top * viewport.height + viewport.top;

					views[i].getView().setViewport(view_final_viewport);
				}
				else
				{
					// view is too wide
				}
			}
		}

		std::vector<sf::FloatRect> getWindowViewRects()
		{
			switch (views.size())
			{
			case 1:
				return
				{
					sf::FloatRect(0.0f, 0.0f, 1.0f, 1.0f)
				};

			case 2:
				return
				{
					sf::FloatRect(0.0f, 0.0f, 0.5f, 1.0f),
					sf::FloatRect(0.5f, 0.0f, 0.5f, 1.0f)
				};

			case 3:
				return
				{
					sf::FloatRect(0.00f, 0.0f, 0.33f, 1.0f),
					sf::FloatRect(0.33f, 0.0f, 0.34f, 1.0f),
					sf::FloatRect(0.67f, 0.0f, 0.33f, 1.0f)
				};

			case 4:
				return
				{
					sf::FloatRect(0.0f, 0.0f, 0.5f, 0.5f),
					sf::FloatRect(0.5f, 0.0f, 0.5f, 0.5f),
					sf::FloatRect(0.0f, 0.5f, 0.5f, 0.5f),
					sf::FloatRect(0.5f, 0.5f, 0.5f, 0.5f)
				};

			}
		}
	};
}
/*
void remakeViewViewports()
{
	std::vector<sf::FloatRect> windowViewRects = getWindowViewRects();

	const sf::Vector2f window = sf::Vector2f(Device::Screen::GetWindow()->getSize());

	for (int i = 0; i < views.size(); ++i)
	{
		const sf::Vector2f viewchain_view_window =
			sf::Vector2f{
				window.x * viewport.width * windowViewRects[i].width,
				window.y * viewport.height * windowViewRects[i].height
		};

		const float viewchain_window_ratio =
			viewchain_view_window.x / viewchain_view_window.y;

		const sf::Vector2f view_size = views[i].getSize();

		const float viewchain_view_ratio =
			view_size.x / view_size.y;

		if (viewchain_window_ratio > viewchain_view_ratio)
		{
			// view is too high

			// conversion ratio between view size to window size
			const float view_window_height_ratio =
				viewchain_view_window.y / view_size.y;

			const sf::Vector2f result_size =
			{
				view_window_height_ratio * view_size.x,
				viewchain_view_window.y
			};

			const sf::Vector2f result_position =
			{
				(viewchain_view_window.x - result_size.x) / 2.f,
				0
			};


		}
		else
		{
			// view is too wide
		}
	}
}
*/