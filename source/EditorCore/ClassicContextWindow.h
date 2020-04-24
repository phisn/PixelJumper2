#pragma once

#include "EditorDataset.h"
#include "EditorWindow.h"

#include "FrameworkCore/FrameworkCore.h"
#include "FrameworkCore/imgui/ImGuiGridWindow.h"
#include "FrameworkCore/imgui/ImGuiModalWindow.h"

namespace Editor
{
	class ClassicContextPopup
		:
		public Framework::IndependentPopupWindow
	{
	public:
		void open(int x, int y)
		{
			Framework::IndependentPopupWindow::open();
		}

	private:
		void onContent() override
		{
			ImGui::Button("hello world");
		}
	};

	enum class ClassicContextNodeStyle
	{
		Classic,
		Hover,
		Selected,

		_Length
	};

	// nodes do not have to have a rect form
	// nodes are worlds, arrows, groups, ...
	struct ClassicContextNode
	{
		virtual ~ClassicContextNode()
		{
		}

		virtual void onOpenPopup() = 0;
		virtual void setStyle(ClassicContextNodeStyle style) = 0;
		virtual bool contains(sf::Vector2f point) const = 0;
	};

	class ClassicContextWorldNode
		:
		public ClassicContextNode
	{
		const sf::Vector2f Padding = { 10, 10 };
		const sf::Vector2f MinSize = { 100, 100 };

		const sf::Color FontColor = sf::Color::Color(220, 220, 220);

		struct Style
		{
			sf::Color fontColor;
			sf::Color rectColor;
			sf::Color borderColor;
			float borderThickness;
		};

		const Style styles[(int) ClassicContextNodeStyle::_Length] =
		{
			Style { FontColor, sf::Color::Color(130, 130, 130), sf::Color::Color(150, 150, 150), 1.f },
			Style { FontColor, sf::Color::Color(90, 90, 90), sf::Color::Color(190, 190, 190), 2.f },
			Style { FontColor, sf::Color::Color(50, 50, 50), sf::Color::Color(210, 210, 210), 2.f }
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

		void setPosition(sf::Vector2f position)
		{
			rect.setPosition(position);
			reconstructNodePosition();
		}

		void setName(std::string name)
		{
			world->name = name;
			reconstructNode();
		}

		void onOpenPopup() override
		{
			Log::Information(L"open popup");
		}

		void setStyle(ClassicContextNodeStyle styleType) override
		{
			const Style& style = styles[(int) styleType];

			name.setFillColor(style.fontColor);
			rect.setFillColor(style.rectColor);
			rect.setOutlineColor(style.borderColor);
			rect.setOutlineThickness(style.borderThickness);
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
				std::max(MinSize.y, name.getLocalBounds().height + Padding.y * 2)
			};

			rect.setSize(size);
			reconstructNodePosition();
		}

		void reconstructNodePosition()
		{
			name.setPosition(rect.getPosition() + Padding);
		}
	};

	struct ClassicContextWindowDataset
	{
		ClassicContext* classicContext;
	};

	// context is a grid that contains nodes
	// these nodes are stored as types
	// nodes should be stored as abstract node to
	// provide easy check for hover, click, ...
	class ClassicContextWindow
		:
		public Framework::ImGuiGridWindow,
		public EditorWindow
	{
	public:
		ClassicContextWindow(const ClassicContextWindowDataset& dataset)
			:
			dataset(dataset)
		{
			title = "ClassicContext";

			ClassicContextWorldNode* node = new ClassicContextWorldNode{ dataset.classicContext->worlds.back() };

			nodes.push_back(node);
			worlds.push_back(node);
		}

		~ClassicContextWindow()
		{
			for (ClassicContextNode* node : nodes)
				delete node;
		}

		// imgui specific process
		void onProcess() override
		{
			process();
		}

		void onEvent(sf::Event event) override
		{
			ImGuiGridWindow::onEvent(event);

			switch (event.type)
			{
			case sf::Event::EventType::MouseMoved:
				if (affectsWindow(event.mouseMove.x, event.mouseMove.y))
				{
					sf::Vector2f point = pixelToCoords(event.mouseMove.x, event.mouseMove.y);

					ClassicContextNode* new_hover = NULL;
					for (ClassicContextNode* node : nodes)
						if (node->contains(point))
						{
							new_hover = node;
							break;
						}

					if (new_hover != hovered && hovered)
					{
						hovered->setStyle(ClassicContextNodeStyle::Classic);
					}

					if (new_hover)
					{
						new_hover->setStyle(ClassicContextNodeStyle::Hover);
					}

					hovered = new_hover;
				}

				break;
			}
		}

	private:
		ClassicContextWindowDataset dataset;

		std::list<ClassicContextNode*> nodes;
		std::vector<ClassicContextWorldNode*> worlds;

		ClassicContextNode* hovered = NULL;
		ClassicContextNode* selected = NULL;

		void onMovementClick(sf::Vector2f point) override
		{
			for (ClassicContextNode* node : nodes)
				if (node->contains(point))
				{
					node->onOpenPopup();
				}
		}

		void onDraw(sf::RenderTarget* target) override
		{
			ImGuiGridWindow::onDraw(target);

			for (ClassicContextWorldNode* world : worlds)
				world->draw(target);
		}

		void processContent() override
		{
			ImGuiGridWindow::processContent();
		}
	};
}

