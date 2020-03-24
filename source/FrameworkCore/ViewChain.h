#pragma once

#include "ScreenDevice.h"

#include <cassert>
#include <list>

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
		ViewChain(const size_t initialCount)
		{
			resizeViews(initialCount);
		}

		void remakeViewViewports()
		{
			std::vector<sf::FloatRect> windowViewRects = getWindowViewRects();

			const sf::Vector2f window = sf::Vector2f(Device::Screen::GetWindow()->getSize());

			int view_index = 0;
			for (ViewChainView& view : views)
			{
				const sf::Vector2f view_max_size_as_viewport =
				{
					window.x * viewport.width * windowViewRects[view_index].width,
					window.y * viewport.height * windowViewRects[view_index].height
				};

				const float view_max_ratio =
					view_max_size_as_viewport.x / view_max_size_as_viewport.y;

				const sf::Vector2f view_size = view.getSize();

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
						(double)view_max_size_as_viewport.y / (double)view_size.y;

					const float view_final_size_x_as_viewport = (float)(view_size_to_viewport * (double)view_size.x);
					sf::FloatRect view_final_viewport =
					{
						(view_max_size_as_viewport.x - view_final_size_x_as_viewport) / 2.f,
						0,
						view_final_size_x_as_viewport,
						view_max_size_as_viewport.y
					};

					// convert window coord to viewport coord
					view_final_viewport.left /= window.x;
					view_final_viewport.top /= window.y;
					view_final_viewport.width /= window.x;
					view_final_viewport.height /= window.y;

					// convert relative to real position
					view_final_viewport.left += windowViewRects[view_index].left * viewport.width + viewport.left;
					view_final_viewport.top += windowViewRects[view_index].top * viewport.height + viewport.top;

					view.getView().setViewport(view_final_viewport);
				}
				else
				{
					const double view_size_to_viewport =
						(double)view_max_size_as_viewport.x / (double)view_size.x;

					const float view_final_size_y_as_viewport = (float)(view_size_to_viewport * (double)view_size.y);
					sf::FloatRect view_final_viewport =
					{
						0,
						(view_max_size_as_viewport.y - view_final_size_y_as_viewport) / 2.f,
						view_max_size_as_viewport.x,
						view_final_size_y_as_viewport,
					};

					// convert window coord to viewport coord
					view_final_viewport.left /= window.x;
					view_final_viewport.top /= window.y;
					view_final_viewport.width /= window.x;
					view_final_viewport.height /= window.y;

					// convert relative to real position
					view_final_viewport.left += windowViewRects[view_index].left * viewport.width + viewport.left;
					view_final_viewport.top += windowViewRects[view_index].top * viewport.height + viewport.top;

					view.getView().setViewport(view_final_viewport);
				}

				++view_index;
			}
		}

		void resizeViews(const size_t count)
		{
			assert(count <= 4 && count != 0);

			if (count < views.size())
			{
				for (int i = 0; i < count - views.size(); ++i)
					views.pop_back();
			}
			else
			{
				for (int i = 0; i < count - views.size(); ++i)
					views.emplace_back(this);
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

		// should only be used to obtain a reference
		// and not called repetivly because of the underlying
		// data structure
		Device::View& getView(const size_t index)
		{
			return *std::next(views.begin(), index);
		}

		size_t getViewCount() const
		{
			return views.size();
		}

	private:
		// has to be list to ensure validation of references
		// after change in size
		std::list<ViewChainView> views;
		sf::FloatRect viewport = { 0.f, 0.f, 1.f, 1.f };

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

			int view_index = 0;
			for (ViewChainView& view : views)
			{
				const sf::Vector2f view_max_size_as_viewport =
				{
					viewport.width * windowViewRects[view_index].width,
					viewport.height * windowViewRects[view_index].height
				};

				const float view_max_ratio =
					view_max_size_as_viewport.x / view_max_size_as_viewport.y;

				const sf::Vector2f view_size = view.getSize();

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
					view_final_viewport.left += windowViewRects[view_index].left * viewport.width + viewport.left;
					view_final_viewport.top += windowViewRects[view_index].top * viewport.height + viewport.top;

					view.getView().setViewport(view_final_viewport);
				}
				else
				{
					// view is too wide
				}

				++view_index;
			}
		}
*/