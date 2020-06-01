#pragma once

#include "ClassicContextManager.h"

#include "FrameworkCore/imgui/ImGuiGridWindow.h"

namespace Editor::ClassicContext
{
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

	// context is a grid that contains nodes
	// these nodes are stored as types
	// nodes should be stored as abstract node to
	// provide easy check for hover, click, ...
	class ClassicContextWindow
		:
		public WindowAccess,
		public EditorWindow,
		public Framework::ImGuiGridWindow
	{
		enum class MouseMode
		{
			None,
			Drag,
			Mark,
			Connect
		};

		const sf::Color MouseMarkerColor = sf::Color::Color(60, 100, 180, 150);
		const sf::Color MouseMarkerBorderColor = sf::Color::Color(50, 90, 180);
		const float MouseMarkerBorderThickness = 2;

		const sf::Time DoubleClickTime = sf::milliseconds(500);

	public:
		ClassicContextWindow(Resource::ContextID contextID)
			:
			manager(contextID)
		{
			setTitle("Context Window #n");

			mouseMarkingRect.setOutlineThickness(MouseMarkerBorderThickness);
			mouseMarkingRect.setFillColor(MouseMarkerColor);
			mouseMarkingRect.setOutlineColor(MouseMarkerBorderColor);
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
					onMouseMoved(event);

					break;
				case sf::Event::EventType::MouseButtonPressed:
					onMousePressed(event);

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

		void onDatabaseEvent(DatabaseEvent event) override
		{
			switch (event.table)
			{
			case DatabaseTable::Transitive:
				if (event.data.transitive.contextID == manager.getContextID())
				{
					onDatabaseTransitiveEvent(event.type, event.data.transitive);
				}

				break;
			case DatabaseTable::World:
				if (event.data.world.contextID == manager.getContextID())
				{
					onDatabaseWorldEvent(event.type, event.data.world);
				}

				break;
			}
		}

		void beginLink(WorldNode* node) override
		{
			mouseMode = MouseMode::Connect;
			connectWorldSource = node;

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

	private:
		ClassicContextManager manager;
		Framework::IndependentPopupWindow* popupWindow = NULL;

		// utility
		bool mouseMoved = true;
		MouseMode mouseMode = MouseMode::None;

		Node* nodeHovered = NULL;
		sf::Clock clickClock;

		// mouse movement
		sf::Vector2i mouseBegin;
		std::vector<sf::Vector2f> mouseNodeBegin;

		// selection
		sf::RectangleShape mouseMarkingRect;
		std::vector<Node*> nodeSelected;

		// connecting
		MouseConnection mouseConnection;
		WorldNode* connectWorldSource;

		void onMovementClick(sf::Vector2f point) override
		{
			if (Node* node = manager.findNodeByPoint(point); node)
			{
				popupWindow = node->createPopupWindow(this);
			}
			else
			{
				// create common popup window
#pragma message todo
			}
		}

		void onDraw(sf::RenderTarget* target) override
		{
			ImGuiGridWindow::onDraw(target);

		}

		void processContent() override
		{
			ImGuiGridWindow::processContent();

			if (popupWindow && !popupWindow->process())
			{
				popupWindow = NULL;
			}


		}

		void changeWindowIndex(int index) override
		{
			setTitle("Classic Context " + std::to_string(index));
		}

		void onMouseMoved(sf::Event& event)
		{
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

				manager.findCollidingNodes(mouseMarkingRect.getGlobalBounds(), nodeSelected);
				for (Node* node : nodeSelected)
					node->setStyle(NodeStyle::Selected);

				break;
			case MouseMode::None:
				handleMouseHover(event.mouseMove.x, event.mouseMove.y);

				break;
			default:
				assert(false);
				break;
			}

		}

		void onMousePressed(sf::Event& event)
		{
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
						if (WorldNode* node = manager.findWorldNodeByPoint(
								pixelToCoords(event.mouseButton.x, event.mouseButton.y)
							); node)
						{
							// createConnection(connectWorldSource->getWorld(), node->getWorld());
						}

						mouseMode = MouseMode::None;
						connectWorldSource->removeTemporaryConnection(&mouseConnection);

						break;
					default:
						if (Node* node = manager.findNodeByPoint(
								pixelToCoords(event.mouseButton.x, event.mouseButton.y)
							); node)
						{
							node->setStyle(NodeStyle::Selected);

							mouseMode = MouseMode::Drag;

							if (mouseNodeBegin.size() > 0)
								mouseNodeBegin.clear();

							if (isNodeSelected(node))
							{
								if (!mouseMoved && clickClock.restart() <= DoubleClickTime)
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
		}

		void onDatabaseWorldEvent(
			DatabaseEventType type, 
			const WorldEvent& event)
		{
			switch (type)
			{
			case DatabaseEventType::Change:
			{
				WorldNode* node = manager.findWorldNodeByID(event.worldID);
				assert(node != NULL);
				node->update();
			}
				break;
			case DatabaseEventType::Create:
				manager.createWorldNode(event.worldID);

				break;
			case DatabaseEventType::Remove:
				manager.removeWorldNode(event.worldID);

				break;
			}
		}

		void onDatabaseTransitiveEvent(
			DatabaseEventType type,
			const TransitiveEvent& event)
		{
			switch (type)
			{
			case DatabaseEventType::Change:
			{
				TransitiveNode* node = manager.findTransitiveNodeByID(event.entryID);
				assert(node != NULL);
				node->update();
			}
				break;
			case DatabaseEventType::Create:
				manager.createWorldNode(event.entryID);

				break;
			case DatabaseEventType::Remove:
				manager.removeTransitiveNode(event.entryID);

				break;
			}
		}

		void handleMouseHover(int x, int y)
		{
			if (affectsWindow(x, y))
			{
				Node* new_hover = manager.findNodeByPoint(pixelToCoords(x, y));
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
	/* find connections

		SELECT input.id, input.outputid, output.id, output.inputid
		FROM world 
			JOIN transitive input ON world.id = input.inputid 
			JOIN transitive output ON world.id = output.inputid 
		WHERE world.id = ?
	*/
	/*void removeLink(ConnectionNode* node) override
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
	}*/
}

