#pragma once

#include <Client/source/menu/MenuLabel.h>

namespace Menu
{
	template <int PRECISION = 20>
	class FPSLabel
		:
		public Label
	{
		using Label::text;

	public:
		using Label::Label;

		void onLogic(const sf::Time time) override
		{
			Label::onLogic(time);

			timeSum -= timeMemory[currentTime];
			timeSum += time;

			timeMemory[currentTime] = time;
			(++currentTime) %= PRECISION;

			text.setValue(std::to_wstring(
				(int) getCurrentFPS()
			));
		}

		float getCurrentFPS() const
		{
			return std::round(
				(PRECISION * 1'000'000) / timeSum.asMicroseconds()
			);
		}

	private:
		sf::Time timeSum;
		sf::Time timeMemory[PRECISION] = { };
		size_t currentTime = 0;
	};
}
