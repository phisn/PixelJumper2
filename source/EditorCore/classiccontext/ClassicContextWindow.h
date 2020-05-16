#pragma once

#include "ClassicContextWorldNode.h"
#include "ClassicContextConnectionNode.h"

#include "EditorCore/EditorWindow.h"

#include "FrameworkCore/FrameworkCore.h"
#include "FrameworkCore/BezierArrow.h"
#include "FrameworkCore/ArrowShape.h"
#include "FrameworkCore/imgui/ImGuiGridWindow.h"
#include "FrameworkCore/imgui/ImGuiModalWindow.h"

#include "imgui/imgui_stdlib.h"

namespace Editor::ClassicContext
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

	class MouseConnection
		:
		public Connection
	{
	public:
		MouseConnection()
			:
			dummy(this)
		{
			arrow.setMode(arrow.Default);
		}

		void draw(sf::RenderTarget* target)
		{
			target->draw(arrow);
		}

		void setTarget(sf::Vector2f target)
		{
			dummy.setPosition(target, ConnectionSide::Top);
		}

		void setElement(ConnectionElement* element)
		{
			dummy.setElement(element);
			this->element = element;
		}

		ConnectionElement* getDummy()
		{
			return &dummy;
		}

	private:
		void setEndpointPosition(
			ConnectionElement* element,
			ConnectionSide side,
			sf::Vector2f position) override
		{
			if (element == &dummy)
			{
				arrow.setTarget(position);
			}
			else
			{
				elementSide = side;
				arrow.setSource(position);
			}

			updateBezierPoints();
		}

		void setEndpointOut(
			ConnectionElement* world,
			bool out) override
		{
		}

		void updateBezierPoints()
		{
			sf::Vector2f p0;

			sf::Vector2f source = arrow.getSource();
			sf::Vector2f target = arrow.getTarget();

			switch (elementSide)
			{
			case ConnectionSide::Top:
				p0 = sf::Vector2f{ 0, target.y - source.y };

				break;
			case ConnectionSide::Left:
				p0 = sf::Vector2f{ target.x - source.x, 0 };

				break;
			case ConnectionSide::Bottom:
				p0 = sf::Vector2f{ 0, source.y - target.y };

				break;
			case ConnectionSide::Right:
				p0 = sf::Vector2f{ target.x - source.x, 0 };

				break;
			}

			sf::FloatRect src_gb = element->getGlobalBounds();
			sf::Vector2f p1 = src_gb.getPosition() + src_gb.getSize() / 2.f - target;

			arrow.setP0(source + p0 * 0.5f);
			arrow.setP1(target + p1 * 0.5f);
		}

		Framework::BezierArrow arrow;

		DummyConnectionElement dummy;
		ConnectionElement* element;

		ConnectionSide elementSide;
	};

	// context is a grid that contains nodes
	// these nodes are stored as types
	// nodes should be stored as abstract node to
	// provide easy check for hover, click, ...
	class ClassicContextWindow
		:
		public Framework::ImGuiGridWindow,
		public EditorWindow,
		public WindowAccess
	{
		const sf::Color MouseMarkerColor = sf::Color::Color(60, 100, 180, 150);
		const sf::Color MouseMarkerBorderColor = sf::Color::Color(50, 90, 180);
		const float MouseMarkerBorderThickness = 2;

		const sf::Time DoubleClickTime = sf::milliseconds(500);

	public:
		ClassicContextWindow()
			:
			dataset(dataset)
		{
			title = "ClassicContext";

			mouseMarkingRect.setOutlineThickness(MouseMarkerBorderThickness);
			mouseMarkingRect.setFillColor(MouseMarkerColor);
			mouseMarkingRect.setOutlineColor(MouseMarkerBorderColor);
			
			WorldNode* node0 = new WorldNode{ this, dataset.classicContext->getDataset().worlds[0].getDataset() };

			nodes.push_back(node0);
			worlds.push_back(node0);

			WorldNode* node1 = new WorldNode{ this, dataset.classicContext->getDataset().worlds[1].getDataset() };

			nodes.push_back(node1);
			worlds.push_back(node1);

			WorldNode* node2 = new WorldNode{ this, dataset.classicContext->getDataset().worlds[2].getDataset() };

			nodes.push_back(node2);
			worlds.push_back(node2);
		}

		~ClassicContextWindow()
		{
			for (Node* node : nodes)
				delete node;
		}

		// imgui specific process
		void onProcess() override
		{
			process();
		}

		void onDatabaseEvent(DatabaseEvent event) override
		{

		}

		void onEvent(sf::Event event) override
		{
			if (windowFocused)
			{
				ImGuiGridWindow::onEvent(event);

				switch (event.type)
				{
				case sf::Event::KeyPressed:
					if (event.key.code == sf::Keyboard::Z &&
						event.key.control)
					{
						Log::Information(L"undo");

						break;
					}

					if (event.key.code == sf::Keyboard::Y &&
						event.key.control)
					{
						Log::Information(L"redo");

						break;
					}

					break;
				case sf::Event::EventType::MouseMoved:
					if (!mouseMoved)
						mouseMoved = true;

					switch (mouseMode)
					{
					case MouseMode::Connect:
						mouseConnection.setTarget(pixelToCoords(
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

						for (Node* node : nodeSelected)
						{
							node->setStyle(NodeStyle::Classic);
						}

						nodeSelected.clear();

						for (Node* node : nodes)
							if (doesRectContainRect(mouseMarkingRect.getGlobalBounds(), node->getGlobalBounds()))
							{
								nodeSelected.push_back(node);
								node->setStyle(NodeStyle::Selected);
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
								if (WorldNode* node = findWorldNodeByPoint(event.mouseButton.x, event.mouseButton.y); node)
								{
									createConnection(connectWorldSource->getWorld(), node->getWorld());
								}

								mouseMode = MouseMode::None;
								connectWorldSource->removeTemporaryConnection(&mouseConnection);

								break;
							default:
								if (Node* node = findNodeByPoint(event.mouseButton.x, event.mouseButton.y); node)
								{
									node->setStyle(NodeStyle::Selected);

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

									for (Node* node : nodeSelected)
										mouseNodeBegin.push_back(node->getPosition());
								}
								else
								{
									for (Node* node : nodeSelected)
										node->setStyle(NodeStyle::Classic);

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
		}

		void beginLink(ClassicWorldDataset* dataset) override
		{
			mouseMode = MouseMode::Connect;
			connectWorldSource = findWorldNodeByDataset(dataset);

			assert(connectWorldSource != NULL);

			sf::Vector2f mouseCursor = pixelToCoords(
				ImGui::GetIO().MousePos.x,
				ImGui::GetIO().MousePos.y);

			connectWorldSource->addTemporaryConnection(
				mouseConnection.getDummy(),
				&mouseConnection);

			mouseConnection.setElement(connectWorldSource);
			mouseConnection.setTarget(mouseCursor);
		}

		// mouse specific
	private:
		Node* nodeHovered = NULL;

		std::vector<sf::Vector2f> mouseNodeBegin;
		std::vector<Node*> nodeSelected;

		sf::Clock doubleClickClock;
		sf::RectangleShape mouseMarkingRect;

		WorldNode* connectWorldSource;
		MouseConnection mouseConnection;

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
				Node* new_hover = findNodeByPoint(x, y);
				if (new_hover != nodeHovered && nodeHovered)
				{
					if (isNodeSelected(nodeHovered))
					{
						nodeHovered->setStyle(NodeStyle::Selected);
					}
					else
					{
						nodeHovered->setStyle(NodeStyle::Classic);
					}
				}

				if (new_hover)
				{
					new_hover->setStyle(NodeStyle::Hover);
				}

				nodeHovered = new_hover;
			}
		}

	private:
		WindowDataset dataset;

		Framework::IndependentPopupWindow* popupWindow = NULL;

		std::list<Node*> nodes;

		std::vector<ConnectionNode*> connections;
		std::vector<WorldNode*> worlds;

		void onMovementClick(sf::Vector2f point) override
		{
			for (Node* node : nodes)
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

			for (ConnectionNode* connection : connections)
				connection->draw(target);

			for (WorldNode* world : worlds)
				world->draw(target);

			switch (mouseMode)
			{
			case MouseMode::Mark:
				target->draw(mouseMarkingRect);

				break;
			case MouseMode::Connect:
				mouseConnection.draw(target);

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

		void removeLink(ConnectionNode* node) override
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

			WorldNode* sourceNode = findWorldNodeByDataset(source);
			WorldNode* targetNode = findWorldNodeByDataset(target);

			if (sourceNode->findTransitiveToWorld(targetNode) == NULL)
			{
				ConnectionNode* node = new ConnectionNode{ this, sourceNode, targetNode };

				sourceNode->addTransitiveConnection(targetNode, node);
				targetNode->addTransitiveConnection(sourceNode, node);

				connections.push_back(node);
			}

			source->transitive.push_back(target);
			source->notify();
		}

		// utility
	private:
		WorldNode* findWorldNodeByDataset(ClassicWorldDataset* dataset) const
		{
			for (WorldNode* node : worlds)
				if (node->getWorld() == dataset)
				{
					return node;
				}

			return NULL;
		}

		Node* findNodeByPoint(int x, int y) const
		{
			sf::Vector2f coord = pixelToCoords(x, y);
			for (Node* node : nodes)
				if (node->contains(coord))
				{
					return node;
				}

			return NULL;
		}

		WorldNode* findWorldNodeByPoint(int x, int y) const
		{
			sf::Vector2f coord = pixelToCoords(x, y);
			for (WorldNode* node : worlds)
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

		bool isNodeSelected(Node* node) const
		{
			for (Node* node_iter : nodeSelected)
				if (node_iter == node)
				{
					return true;
				}

			return false;
		}
	};
}

