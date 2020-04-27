#pragma once

#include "ClassicContextNode.h"
#include "EditorDataset.h"

#include "FrameworkCore/FrameworkCore.h"

#include <SFML/Graphics.hpp>

namespace Editor
{
	class ClassicContextWorldNodePopup
		:
		public ClassicContextPopup
	{
		const int MaxNameSize = 16;
		const float MaxPopupWidth = 100;

	public:
		ClassicContextWorldNodePopup(
			ClassicContextWindowAccess* access,
			ClassicWorldDataset* dataset)
			:
			ClassicContextPopup(access),
			dataset(dataset),
			inputText(dataset->name)
		{
			inputText.resize(MaxNameSize);
			open();
		}

	private:
		ClassicWorldDataset* dataset;

		bool makeWindow() override
		{
			return Framework::IndependentPopupWindow::makeWindow();
		}

		void onContent() override
		{
			LazyDatasetChange datasetChange{ dataset };

			if (ImGui::InputTextEx("###classicContextPopup_Name", "Name", &inputText[0], inputText.length() + 1, ImVec2{ MaxPopupWidth, 0 }, 0))
			{
				// autotrim after first \0
				dataset->name = inputText.c_str();
				datasetChange.notify();
			}

			ImGui::Separator();

			if (ImGui::Button("Connect to", ImVec2{ MaxPopupWidth, 0 }))
			{
				access->beginLink(dataset);
			}
		}

		std::string inputText;
	};

	class ClassicContextWorldNode
		:
		public ClassicContextNode
	{
		const sf::Vector2f Padding = { 10, 10 };
		const sf::Vector2f MinSize = { 50, 50 };

		const sf::Color FontColor = sf::Color::Color(220, 220, 220);

		struct Style
		{
			sf::Color fontColor;
			sf::Color rectColor;
			sf::Color borderColor;
			float borderThickness;
		};

		const Style styles[(int)ClassicContextNodeStyle::_Length] =
		{
			Style { FontColor, sf::Color::Color(160, 160, 160), sf::Color::Color(90, 90, 90), 3.f },
			Style { FontColor, sf::Color::Color(120, 120, 120), sf::Color::Color(190, 190, 190), 3.f },
			Style { FontColor, sf::Color::Color(80, 80, 80), sf::Color::Color(210, 210, 210), 3.f }
		};

	public:
		ClassicContextWorldNode(ClassicWorldDataset* world)
			:
			world(world),
			worldListener(*world, [this]()
				{
					reconstructNodeContent();
				})
		{
			name.setFont(Framework::GetFont());
			name.setCharacterSize(30);

			setStyle(ClassicContextNodeStyle::Classic);

			reconstructNodeContent();
		}

		void draw(sf::RenderTarget* target)
		{
			target->draw(rect);
			target->draw(name);
		}

		Framework::IndependentPopupWindow* createPopupWindow(
			ClassicContextWindowAccess* access) override
		{
			return new ClassicContextWorldNodePopup{ access, world };
		}

		void setStyle(ClassicContextNodeStyle styleType) override
		{
			const Style& style = styles[(int)styleType];

			name.setFillColor(style.fontColor);
			rect.setFillColor(style.rectColor);
			rect.setOutlineColor(style.borderColor);
			rect.setOutlineThickness(style.borderThickness);
		}

		void setPosition(sf::Vector2f position) override
		{
			rect.setPosition(position);
			reconstructNodePosition();
		}

		sf::Vector2f getPosition() const override
		{
			return rect.getPosition();
		}

		sf::FloatRect getGlobalBounds() override
		{
			return rect.getGlobalBounds();
		}

		bool contains(sf::Vector2f point) const override
		{
			return rect.getGlobalBounds().contains(point);
		}

	private:
		ClassicWorldDataset* world;
		ClassicWorldDataset::ListenerContainer worldListener;

		sf::RectangleShape rect;
		sf::Text name;

		void reconstructNodeContent()
		{
			name.setString(world->name);
			reconstructNodeSize();
		}

		void reconstructNodeSize()
		{
			sf::Vector2f size =
			{
				std::max(MinSize.x, name.getLocalBounds().width + Padding.x * 2),
				std::max(MinSize.y, name.getLocalBounds().height + Padding.y * 4)
			};

			rect.setSize(size);
			reconstructNodePosition();
		}

		void reconstructNodePosition()
		{
			name.setPosition(sf::Vector2f(sf::Vector2i(rect.getPosition() + Padding)));
		}
	};
}
