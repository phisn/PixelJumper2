#pragma once

#include <Client/source/framework/Animation.h>

#include <functional>

#include <SFML/System/Time.hpp>

namespace Framework
{
	class AsyncAnimation
	{
	public:
		bool stopRepeating()
		{
			doesRepeat = false;
		}

		void setOnFinished(
			std::function<void()> onFinished)
		{
			this->onFinished = onFinished;
		}

		void onDraw() const
		{
			animation->onDraw();
		}

		bool onUpdate(
			const sf::Time time)
		{
			if (!animation->onUpdate(time))
			{
				if (doesRepeat)
				{
					animation->reset();
				}
				else
				{
					if (onFinished)
					{
						onFinished();
					}

					return false;
				}
			}

			return true;
		}

	private:
		Animation* animation;

		bool doesRepeat = false;
		std::function<void()> onFinished;
	};

}