#pragma once

#include "ClassicContextWorldNode.h"
#include "ClassicContextConnectionNode.h"

#include "EditorDataset.h"
#include "EditorWindow.h"

#include "FrameworkCore/FrameworkCore.h"
#include "FrameworkCore/BezierArrow.h"
#include "FrameworkCore/ArrowShape.h"
#include "FrameworkCore/imgui/ImGuiGridWindow.h"
#include "FrameworkCore/imgui/ImGuiModalWindow.h"

#include "imgui/imgui_stdlib.h"

namespace Editor
{
	/*
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
	*/
	struct ClassicContextWindowDataset
	{
		ClassicContextDataset* classicContext;
	};

	// context is a grid that contains nodes
	// these nodes are stored as types
	// nodes should be stored as abstract node to
	// provide easy check for hover, click, ...
	class ClassicContextWindow
		:
		public Framework::ImGuiGridWindow,
		public EditorWindow,
		public ClassicContextWindowAccess
	{
		const sf::Color MouseMarkerColor = sf::Color::Color(60, 100, 180, 150);
		const sf::Color MouseMarkerBorderColor = sf::Color::Color(50, 90, 180);
		const float MouseMarkerBorderThickness = 2;

		const sf::Time DoubleClickTime = sf::milliseconds(500);

	public:
		ClassicContextWindow(const ClassicContextWindowDataset& dataset)
			:
			dataset(dataset)
		{
			title = "ClassicContext";

			mouseMarkingRect.setOutlineThickness(MouseMarkerBorderThickness);
			mouseMarkingRect.setFillColor(MouseMarkerColor);
			mouseMarkingRect.setOutlineColor(MouseMarkerBorderColor);

			ClassicContextWorldNode* node0 = new ClassicContextWorldNode{ this, dataset.classicContext->worlds[0] };

			nodes.push_back(node0);
			worlds.push_back(node0);

			ClassicContextWorldNode* node1 = new ClassicContextWorldNode{ this, dataset.classicContext->worlds[1] };

			nodes.push_back(node1);
			worlds.push_back(node1);

			ClassicContextWorldNode* node2 = new ClassicContextWorldNode{ this, dataset.classicContext->worlds[2] };

			nodes.push_back(node2);
			worlds.push_back(node2);

			connectArrow.setMode(Framework::ArrowShapeMode::Default);
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
				if (!mouseMoved)
					mouseMoved = true;

				switch (mouseMode)
				{
				case MouseMode::Connect:
					connectArrow.setDestination(pixelToCoords(
						event.mouseMove.x,
						event.mouseMove.y));

					handleMouseHover(event.mouseMove.x, event.mouseMove.y);

					break;
				case MouseMode::Drag:
					assert(nodeSelected.size() == mouseNodeBegin.size());

					for (int i = 0; i < nodeSelected.size(); ++i)
						nodeSelected[i]->setPosition(mouseNodeBegin[i] - makeMouseGridOffset(
							mouseBegin,
							event.mouseMove.x,
							event.mouseMove.y));
					
					break;
				case MouseMode::Mark:
					mouseMarkingRect.setSize(
						pixelToCoords(event.mouseMove.x, event.mouseMove.y) - mouseMarkingRect.getPosition()
					);

					for (ClassicContextNode* node : nodeSelected)
					{
						node->setStyle(Editor::ClassicContextNodeStyle::Classic);
					}

					nodeSelected.clear();

					for (ClassicContextNode* node : nodes)
						if (doesRectContainRect(mouseMarkingRect.getGlobalBounds(), node->getGlobalBounds()))
						{
							nodeSelected.push_back(node);
							node->setStyle(Editor::ClassicContextNodeStyle::Selected);
						}
					
					break;
				case MouseMode::None:
					handleMouseHover(event.mouseMove.x, event.mouseMove.y);

					break;
				default:
					assert(false);
					break;
				}

				break;
			case sf::Event::EventType::MouseButtonPressed:
				if (affectsWindow(event.mouseButton.x, event.mouseButton.y))
				{
					switch (event.mouseButton.button)
					{
					case sf::Mouse::Button::Left:
						mouseBegin = { event.mouseButton.x, event.mouseButton.y };
						mouseMoved = false;

						switch (mouseMode)
						{
						case MouseMode::Connect:
							if (ClassicContextWorldNode* node = findWorldNodeByPoint(event.mouseButton.x, event.mouseButton.y); node)
							{
								createConnection(connectWorldSource->getWorld(), node->getWorld());
							}

							mouseMode = MouseMode::None;

							break;

						default:
							if (ClassicContextNode* node = findNodeByPoint(event.mouseButton.x, event.mouseButton.y); node)
							{
								node->setStyle(ClassicContextNodeStyle::Selected);

								mouseMode = MouseMode::Drag;

								if (mouseNodeBegin.size() > 0)
									mouseNodeBegin.clear();

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

								mouseNodeBegin.clear();
								nodeSelected.clear();

								mouseMarkingRect.setPosition(pixelToCoords(
									event.mouseButton.x,
									event.mouseButton.y));
								mouseMarkingRect.setSize({ 0, 0 });

								mouseMode = MouseMode::Mark;
							}

							break;
						}
						
						break;
					}
				}

				break;
			case sf::Event::EventType::MouseButtonReleased:
				if (event.mouseButton.button == sf::Mouse::Button::Left)
				{
					mouseMode = MouseMode::None;

					handleMouseHover(event.mouseButton.x, event.mouseButton.y);
				}

				break;
			}
		}

		void beginLink(ClassicWorldDataset* dataset) override
		{
			mouseMode = MouseMode::Connect;
			connectWorldSource = findWorldNodeByDataset(dataset);

			assert(connectWorldSource != NULL);

			connectArrow.setSource(
				connectWorldSource->getPosition() + connectWorldSource->getSize() / 2.f
			);
			connectArrow.setDestination(connectArrow.getSource());
		}

		// mouse specific
	private:
		struct ArrowConnectionElement
			:
			public ClassicContextConnectionElement
		{
			void notifyBoundsChanged() override
			{
			}

			sf::FloatRect getGlobalBounds() const override
			{
			}
		};

		ClassicContextNode* nodeHovered = NULL;

		std::vector<sf::Vector2f> mouseNodeBegin;
		std::vector<ClassicContextNode*> nodeSelected;

		sf::Clock doubleClickClock;
		sf::RectangleShape mouseMarkingRect;

		ClassicContextWorldNode* connectWorldSource;
		Framework::ArrowShape connectArrow;

		enum class MouseMode
		{
			None,
			Drag,
			Mark,
			Connect
		};

		bool mouseMoved = true;
		MouseMode mouseMode = MouseMode::None;

		sf::Vector2i mouseBegin;

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

	private:
		ClassicContextWindowDataset dataset;

		Framework::IndependentPopupWindow* popupWindow = NULL;

		std::list<ClassicContextNode*> nodes;

		std::vector<ClassicContextConnectionNode*> connections;
		std::vector<ClassicContextWorldNode*> worlds;

		void onMovementClick(sf::Vector2f point) override
		{
			for (ClassicContextNode* node : nodes)
				if (node->contains(point))
				{
					popupWindow = node->createPopupWindow();
					return;
				}

			// create common popup window
#pragma message todo
		}

		void onDraw(sf::RenderTarget* target) override
		{
			ImGuiGridWindow::onDraw(target);

			for (ClassicContextConnectionNode* connection : connections)
				connection->draw(target);

			for (ClassicContextWorldNode* world : worlds)
				world->draw(target);

			switch (mouseMode)
			{
			case MouseMode::Mark:
				target->draw(mouseMarkingRect);

				break;
			case MouseMode::Connect:
				target->draw(connectArrow);

				break;
			}
		}

		void processContent() override
		{
			ImGuiGridWindow::processContent();

			if (popupWindow && !popupWindow->process())
			{
				popupWindow = NULL;
			}
		}

		void removeLink(ClassicContextConnectionNode* node) override
		{

		}
		
		void createConnection(
			ClassicWorldDataset* source,
			ClassicWorldDataset* target) override
		{
			if (source == target)
			{
				return;
			}

			ClassicContextWorldNode* sourceNode = findWorldNodeByDataset(source);
			ClassicContextWorldNode* targetNode = findWorldNodeByDataset(target);

			if (sourceNode->findTransitiveToWorld(targetNode) == NULL)
			{
				ClassicContextConnectionNode* node = new ClassicContextConnectionNode{ this, sourceNode, targetNode };

				sourceNode->addTransitiveConnection(targetNode, node);
				targetNode->addTransitiveConnection(sourceNode, node);

				connections.push_back(node);
			}

			source->transitive.push_back(target);
			source->notify();
		}

		// utility
	private:
		ClassicContextWorldNode* findWorldNodeByDataset(ClassicWorldDataset* dataset) const
		{
			for (ClassicContextWorldNode* node : worlds)
				if (node->getWorld() == dataset)
				{
					return node;
				}

			return NULL;
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

		ClassicContextWorldNode* findWorldNodeByPoint(int x, int y) const
		{
			sf::Vector2f coord = pixelToCoords(x, y);
			for (ClassicContextWorldNode* node : worlds)
				if (node->contains(coord))
				{
					return node;
				}

			return NULL;
		}

		bool doesRectContainRect(sf::FloatRect rect1, sf::FloatRect rect2) const
		{
			return (rect1.left < rect2.left + rect2.width)
				&& (rect1.top < rect2.top + rect2.height)
				&& (rect2.left < rect1.left + rect1.width)
				&& (rect2.top < rect1.top + rect1.height);
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
	};
}

