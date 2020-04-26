#pragma once

#include "ClassicContextNode.h"
#include "EditorDataset.h"

#include "FrameworkCore/FrameworkCore.h"

#include <SFML/Graphics.hpp>

namespace Editor
{
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

		class ClassicContextWorldNodePopup
			:
			public Framework::IndependentPopupWindow
		{
			const int MaxNameSize = 16;
			const float MaxPopupWidth = 100;

		public:
			ClassicContextWorldNodePopup(ClassicContextWorldNode* node)
				:
				node(node),
				inputText(node->world->name)
			{
				inputText.resize(MaxNameSize);
				open();
			}

		private:
			ClassicContextWorldNode* node;

			bool makeWindow() override
			{
				return Framework::IndependentPopupWindow::makeWindow();
			}

			void onContent() override
			{
				if (ImGui::InputTextEx("###classicContextPopup_Name", "Name", &inputText[0], inputText.length() + 1, ImVec2{ MaxPopupWidth, 0 }, 0))
				{
					node->setName(inputText.c_str());
				}

				ImGui::Separator();

				if (ImGui::Button("Connect to", ImVec2{ MaxPopupWidth, 0 }))
				{
				}
			}

			std::string inputText;
		};

	public:
		ClassicContextWorldNode(ClassicWorld* world)
			:
			world(world)
		{
			name.setFont(Framework::GetFont());
			name.setCharacterSize(30);
			name.setString(world->name);

			setStyle(ClassicContextNodeStyle::Classic);

			reconstructNode();
		}

		void draw(sf::RenderTarget* target)
		{
			target->draw(rect);
			target->draw(name);
		}

		void setName(std::string name)
		{
			world->name = name;
			this->name.setString(world->name);

			reconstructNode();
		}

		Framework::IndependentPopupWindow* createPopupWindow() override
		{
			return new ClassicContextWorldNodePopup{ this };
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
		ClassicWorld* world;

		sf::RectangleShape rect;
		sf::Text name;

		void reconstructNode()
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
