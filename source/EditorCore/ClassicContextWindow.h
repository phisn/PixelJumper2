#pragma once

#include "EditorDataset.h"
#include "EditorWindow.h"

#include "FrameworkCore/FrameworkCore.h"
#include "FrameworkCore/imgui/ImGuiGridWindow.h"
#include "FrameworkCore/imgui/ImGuiModalWindow.h"

namespace Editor
{
	// popup when no node was selected?
	// needs to aquire classic context
	class ClassicContextPopup
		:
		public Framework::IndependentPopupWindow
	{
	public:
		void open()
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

		virtual sf::Vector2f getPosition() const = 0;
		virtual void setPosition(sf::Vector2f position) = 0;

		// approx. max space this node can use to disable
		// drawing of unneeded nodes
		virtual sf::FloatRect getGlobalBounds() = 0;

		virtual bool contains(sf::Vector2f point) const = 0;
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

		const Style styles[(int) ClassicContextNodeStyle::_Length] =
		{
			Style { FontColor, sf::Color::Color(160, 160, 160), sf::Color::Color(90, 90, 90), 3.f },
			Style { FontColor, sf::Color::Color(120, 120, 120), sf::Color::Color(190, 190, 190), 3.f },
			Style { FontColor, sf::Color::Color(80, 80, 80), sf::Color::Color(210, 210, 210), 3.f }
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
		const sf::Color MouseMarkerColor = sf::Color::Color(30, 30, 180, 150);
		const sf::Color MouseMarkerBorderColor = sf::Color::Color(20, 20, 180);
		const sf::Time DoubleClickTime = sf::milliseconds(500);

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
				if (mousePressed)
				{
					mouseMoved = true;

					if (mouseMarking)
					{
					}
					else
					{
						assert(nodeSelected.size() == mouseNodeBegin.size());

						for (int i = 0; i < nodeSelected.size(); ++i)
							nodeSelected[i]->setPosition(mouseNodeBegin[i] - makeMouseGridOffset(
								mouseBegin,
								event.mouseMove.x,
								event.mouseMove.y));
					}
				}
				else
				{
					handleMouseHover(event.mouseMove.x, event.mouseMove.y);
				}

				break;
			case sf::Event::EventType::MouseButtonPressed:
				if (affectsWindow(event.mouseButton.x, event.mouseButton.y))
				{
					switch (event.mouseButton.button)
					{
					case sf::Mouse::Button::Left:
						if (ClassicContextNode* node = findNodeByPoint(event.mouseButton.x, event.mouseButton.y); node)
						{
							node->setStyle(ClassicContextNodeStyle::Selected);

							if (mouseNodeBegin.size() > 0)
								mouseNodeBegin.clear();

							mousePressed = true;
							mouseMoved = false;
							mouseBegin = { event.mouseButton.x, event.mouseButton.y };

							if (isNodeSelected(node))
							{
								if (!mouseMoved && doubleClickClock.restart() <= DoubleClickTime)
								{
									Log::Information(L"double click");
								}
							}
							else
							{
								if (nodeSelected.size() > 0)
									nodeSelected.clear();

								nodeSelected.push_back(node);
							}

							for (ClassicContextNode* node : nodeSelected)
								mouseNodeBegin.push_back(node->getPosition());
						}
						else
						{
							for (ClassicContextNode* node : nodeSelected)
								node->setStyle(ClassicContextNodeStyle::Classic);

							nodeSelected.clear();
							mouseNodeBegin.clear();

							mouseMarking = true;
						}

						break;
					}
				}

				break;
			case sf::Event::EventType::MouseButtonReleased:
				if (event.mouseButton.button == sf::Mouse::Button::Left)
				{
					mousePressed = false;
					mouseMarking = false;

					handleMouseHover(event.mouseButton.x, event.mouseButton.y);
				}

				break;
			}
		}

		// drag, click and mark
	private:
		ClassicContextNode* nodeHovered = NULL;

		std::vector<sf::Vector2f> mouseNodeBegin;
		std::vector<ClassicContextNode*> nodeSelected;

		sf::Clock doubleClickClock;

		bool mouseMoved = true;
		bool mouseMarking = false;

		sf::Vector2i mouseBegin;
		
		bool mousePressed = false;

		void handleMouseHover(int x, int y)
		{
			if (affectsWindow(x, y))
			{
				ClassicContextNode* new_hover = findNodeByPoint(x, y);
				if (new_hover != nodeHovered && nodeHovered)
				{
					if (isNodeSelected(nodeHovered))
					{
						nodeHovered->setStyle(ClassicContextNodeStyle::Selected);
					}
					else
					{
						nodeHovered->setStyle(ClassicContextNodeStyle::Classic);
					}
				}

				if (new_hover)
				{
					new_hover->setStyle(ClassicContextNodeStyle::Hover);
				}

				nodeHovered = new_hover;
			}
		}

		bool isNodeSelected(ClassicContextNode* node) const
		{
			for (ClassicContextNode* node_iter : nodeSelected)
				if (node_iter == node)
				{
					return true;
				}

			return false;
		}

	private:
		ClassicContextWindowDataset dataset;

		std::list<ClassicContextNode*> nodes;
		std::vector<ClassicContextWorldNode*> worlds;

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

		ClassicContextNode* findNodeByPoint(int x, int y) const
		{
			sf::Vector2f coord = pixelToCoords(x, y);
			for (ClassicContextNode* node : nodes)
				if (node->contains(coord))
				{
					return node;
				}

			return NULL;
		}
	};
}

