#pragma once

#include "ClassicContextConnection.h"
#include "ClassicContextNode.h"
#include "EditorFailureScene.h"

#include "FrameworkCore/FrameworkCore.h"
#include "FrameworkCore/imgui/ImGuiContextWindow.h"
#include "ResourceCore/WorldResource.h"

#include <SFML/Graphics.hpp>

namespace Editor::ClassicContext
{
	class WorldPopup
		:
		public Framework::ContextWindow
	{
	public:
		WorldPopup(WindowAccess* access, WorldNode* worldNode)
			:
			access(access),
			worldNode(worldNode)
		{
			width = 100;
		}

	private:
		WindowAccess* access;
		WorldNode* worldNode;

		bool makeWindow() override
		{
			return Framework::IndependentPopupWindow::makeWindow();
		}

		void onContent() override;
	};

	class WorldNode
		:
		public Node,
		public ConnectionElement
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

		const Style styles[(int)NodeStyle::_Length] =
		{
			Style { FontColor, sf::Color::Color(160, 160, 160), sf::Color::Color(90, 90, 90), 3.f },
			Style { FontColor, sf::Color::Color(120, 120, 120), sf::Color::Color(190, 190, 190), 3.f },
			Style { FontColor, sf::Color::Color(80, 80, 80), sf::Color::Color(210, 210, 210), 3.f }
		};

		struct ConnectionContainer
		{
			Connection* node;
			ConnectionElement* element;

			bool out = false;
		};

		typedef std::pair<WorldNode*, ConnectionContainer*> WorldNodeConnectionPair;

	public:
		WorldNode(
			Resource::WorldID worldID,
			std::string name)
			:
			id(worldID),
			name(std::move(name), Framework::GetFont(), 30)
		{
			setStyle(NodeStyle::Classic);
			reconstructNodeContent();
		}

		void addTransitiveConnection(
			WorldNode* worldNode,
			Connection* node)
		{
			ConnectionContainer& connection = connections.emplace_back();
			transitiveConnections.push_back(std::make_pair(worldNode, &connection));

			// use dataset to indirectly change out
			connection.node = node;
			connection.element = worldNode;


		}

		void addTemporaryConnection(
			ConnectionElement* element,
			Connection* node)
		{
			ConnectionContainer& connection = connections.emplace_back();

			connection.node = node;
			connection.element = element;
		}

		void removeTransitiveConnection(Connection* node)
		{
			decltype(transitiveConnections)::iterator removal = std::find_if(
				transitiveConnections.begin(),
				transitiveConnections.end(),
				[node](WorldNodeConnectionPair& pair)
				{
					return pair.second->node == node;
				});

			if (transitiveConnections.end() != removal)
			{
				transitiveConnections.erase(removal);

				decltype(connections)::iterator removal = std::find_if(
					connections.begin(),
					connections.end(),
					[node](ConnectionContainer& connection)
					{
						return connection.node == node;
					});

				assert(connections.end() != removal);
				connections.erase(removal);

				notifyBoundsChanged();
			}
		}

		// trusting caller that node is actually
		// a temoporary connection
		void removeTemporaryConnection(Connection* node)
		{
			decltype(connections)::iterator removal = std::find_if(
				connections.begin(),
				connections.end(),
				[node](ConnectionContainer& connection)
				{
					return connection.node == node;
				});

			if (connections.end() != removal)
				connections.erase(removal);

			notifyBoundsChanged();
		}

		void update()
		{
			typedef std::tuple<SQLiteString> WorldUpdateTuple;

			WorldUpdateTuple worldUpdate;
			if (!Database::Statement<WorldUpdateTuple>(
					EditorDatabase::Instance(),
					"SELECT name FROM worlds WHERE id = ?", id).execute(worldUpdate))
			{
				Framework::Core::PushScene(new EditorFailureScene(
					"Failed to update world in contextwindow '" + std::to_string(id) + "'"));
				return;
			}

			name.setString(std::get<0>(worldUpdate));
			reconstructNodeContent();
		}

		void draw(sf::RenderTarget* target) override
		{
			target->draw(rect);
			target->draw(name);
		}

		bool make(Resource::ReadPipe* const pipe) override
		{
			return pipe->readValue(&rect.getPosition());
		}

		bool save(Resource::WritePipe* const pipe) override
		{
			if (sf::Vector2f position; pipe->writeValue(&position))
			{
				setPosition(position);
				return true;
			}

			return false;
		}

		void notifyBoundsChanged() override
		{
			reconstructConnectionsPosition();
		}

		Framework::IndependentPopupWindow* createPopupWindow(WindowAccess* access) override
		{
			return new WorldPopup(access, this);
		}

		void setStyle(NodeStyle styleType) override
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

		bool contains(sf::Vector2f point) const override
		{
			return rect.getGlobalBounds().contains(point);
		}

		sf::Vector2f getSize() const
		{
			return rect.getSize();
		}

		Resource::WorldID getID() const
		{
			return id;
		}

		sf::FloatRect getGlobalBounds() const override
		{
			return rect.getGlobalBounds();
		}

		sf::Vector2f getPosition() const override
		{
			return rect.getPosition();
		}

	private:
		Resource::WorldID id;

		sf::RectangleShape rect;
		sf::Text name;

		// needs to be list to not invalidate pointer
		std::list<ConnectionContainer> connections;
		std::vector<WorldNodeConnectionPair> transitiveConnections;

		bool ignoreDoubleConnection = false;

		void reconstructNodeContent()
		{
			// assuming information are already
			// applied
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

			// reconstruct positions only for this node and
			// notify other nodes directly because a calling chain
			// is not needed when just our positions changes
			reconstructConnectionsPosition();

			for (ConnectionContainer& connection : connections)
				connection.element->notifyBoundsChanged();
		}

		void reconstructConnectionsPosition()
		{
			struct ConnectionWithValue
			{
				// less is more important
				float value;
				ConnectionContainer* connection;
			};

			enum Side
			{
				Top,
				Left,
				Bottom,
				Right,

				Invalid
			};

			std::vector<ConnectionWithValue> process_connections[4];
			std::vector<ConnectionContainer*> final_connections[4];

			// also increased for every not used connection to
			// prevent extensive search for unused
			int final_connection_count = 0;

			sf::FloatRect src_gb = getGlobalBounds();
			for (ConnectionContainer& connection : connections)
			{
				sf::FloatRect tgt_gb = connection.element->getGlobalBounds();

				Side vertical_side = Invalid;
				Side horizont_side = Invalid;

				// source and target points depend on specific side to get
				// precise values. these points do represent the corrosponding
				// corners of 
//				sf::Vector2f src_p = src_gb.getPosition() + src_gb.getSize() / 2.f;
//				sf::Vector2f tgt_p = tgt_gb.getPosition() + tgt_gb.getSize() / 2.f;

				/*
				
				
				+--------+
				|        |
				+--------+
				          \
				            +-----+
							|     |
							+-----+
				
				
				*/

				sf::Vector2f offset_cmpt_fct;

				if (src_gb.top > tgt_gb.top + tgt_gb.height)
				{
					vertical_side = Top;
					offset_cmpt_fct.y = 0;
				}
				else if (src_gb.top + src_gb.height < tgt_gb.top)
				{
					vertical_side = Bottom;
					offset_cmpt_fct.y = 0;
				}

				if (src_gb.left > tgt_gb.left + tgt_gb.width)
				{
					horizont_side = Left;
					offset_cmpt_fct.x = 0;
				}
				else if (src_gb.left + src_gb.width < tgt_gb.left)
				{
					horizont_side = Right;
					offset_cmpt_fct.x = 0;
				}

				if (vertical_side != horizont_side)
				{
					if (vertical_side == Invalid)
					{
						++final_connection_count;
						final_connections[horizont_side].push_back(&connection);

						continue;
					}
					else if (horizont_side == Invalid)
					{
						++final_connection_count;
						final_connections[vertical_side].push_back(&connection);

						continue;
					}
				}

				sf::Vector2f src_center = src_gb.getPosition() + src_gb.getSize() / 2.f;
				sf::Vector2f tgt_center = tgt_gb.getPosition() + tgt_gb.getSize() / 2.f;

				sf::Vector2f src_cnn_pos =
				{
					src_center.x + offset_cmpt_fct.x * src_gb.width,
					src_center.y + offset_cmpt_fct.y * src_gb.height
				};

				sf::Vector2f tgt_cnn_pos =
				{
					tgt_center.x - offset_cmpt_fct.x * tgt_gb.width,
					tgt_center.y - offset_cmpt_fct.y * tgt_gb.height
				};

				float side0 = (tgt_cnn_pos.y - src_cnn_pos.y) - (tgt_cnn_pos.x - src_cnn_pos.x);
				float side1 = (tgt_cnn_pos.y - src_cnn_pos.y) + (tgt_cnn_pos.x - src_cnn_pos.x);

				if (vertical_side == Invalid)
				{
					++final_connection_count;

					if (side0 > 0)
					{
						final_connections[side1 > 0
							? Bottom
							: Left].push_back(&connection);
					}
					else
					{
						final_connections[side1 > 0
							? Right
							: Top].push_back(&connection);
					}

					continue;
				}

				bool is_neg_line = (vertical_side == Top) == (horizont_side == Left);

				float side = is_neg_line ? side0 : -side1;

				sf::Vector2f line = is_neg_line
					? sf::Vector2f{ 1, -1 }
					: sf::Vector2f{ 1,  1 };

				float value = get_distance_to_line(
					src_cnn_pos,
					tgt_cnn_pos,
					line);

				ConnectionWithValue& connection_vertical = process_connections[vertical_side].emplace_back();
				ConnectionWithValue& connection_horizont = process_connections[horizont_side].emplace_back();

				connection_vertical.connection = &connection;
				connection_horizont.connection = &connection;

				connection_vertical.value = value *
					((vertical_side == Top) == (side < 0) == is_neg_line
						? -1
						:  1);
				connection_horizont.value = value *
					((horizont_side == Left) == (side > 0)
						? -1
						:  1);
			}

			if (final_connection_count != connections.size())
			{
				// sort connections by vlaue
				for (int i = 0; i < 4; ++i)
					std::sort(
						process_connections[i].begin(),
						process_connections[i].end(),
						[](const ConnectionWithValue& p0, const ConnectionWithValue& p1)
						{
							return p0.value > p1.value;
						});

				while (true)
				{
					Side min_value = Invalid;

					// find starting element
					for (int side = 0; side < 4; ++side)
						if (process_connections[side].size() > 0)
						{
							min_value = (Side)side;
							break;
						}

					assert(min_value != Invalid);

					for (int side = min_value + 1; side < 4; ++side)
						if (process_connections[side].size() > 0 && (
							ignoreDoubleConnection &&
							final_connections[min_value].size() <= 1 &&
							final_connections[side].size() <= 1
							?	process_connections[side].back().value < process_connections[min_value].back().value
							:	final_connections[side].size() < final_connections[min_value].size() ||
								final_connections[side].size() == final_connections[min_value].size() &&
	 							process_connections[side].back().value < process_connections[min_value].back().value))
						{
							min_value = (Side)side;
						}

					final_connections[min_value].push_back(process_connections[min_value].back().connection);

					if (++final_connection_count >= connections.size())
						break;

					for (int side = 0; side < 4; ++side)
						if (process_connections[side].size() > 0)
						{
							auto removal = std::find_if(
								process_connections[side].begin(),
								process_connections[side].end(),
								[&final_connections, min_value](const ConnectionWithValue p0)
								{
									return p0.connection == final_connections[min_value].back();
								});

							if (process_connections[side].end() != removal)
								process_connections[side].erase(removal);
						}
				}
			}

			for (int side = 0; side < 4; ++side)
				std::sort(
					final_connections[side].begin(),
					final_connections[side].end(),
					[side](ConnectionContainer* p0, ConnectionContainer* p1)
					{
						sf::FloatRect p0_gb = p0->element->getGlobalBounds();
						sf::FloatRect p1_gb = p1->element->getGlobalBounds();

						return (side == Top || side == Bottom)
							? p0_gb.left + p0_gb.width / 2.f < p1_gb.left + p1_gb.width / 2.f
							: p0_gb.top + p0_gb.height / 2.f < p1_gb.top + p1_gb.height / 2.f;
					});

			for (int side = 0; side < 4; ++side)
				for (int i = 0; i < final_connections[side].size(); ++i)
					if (side == Top || side == Bottom)
					{
						final_connections[side][i]->node->setEndpointPosition(
							this,
							ConnectionSide::Top,
							sf::Vector2f
							{
								src_gb.left + (i + 1) * src_gb.width / (final_connections[side].size() + 1),
								src_gb.top + (side == Top ? 0 : src_gb.height)
							});
					}
					else
					{
						final_connections[side][i]->node->setEndpointPosition(
							this,
							ConnectionSide::Left,
							sf::Vector2f
							{
								src_gb.left + (side == Left ? 0 : src_gb.width),
								src_gb.top + (i + 1) * src_gb.height / (final_connections[side].size() + 1)
							});
					}
		}

		// m: line source
		// n: point from distance
		// g: line factor
		float get_distance_to_line(sf::Vector2f m, sf::Vector2f n, sf::Vector2f g) const
		{
			float temp0 = ((n.y - m.y) * g.x - (n.x - m.x) * g.y) / (2 * g.x * g.y);
			float temp1 = g.x * g.x + g.y * g.y;
			return sqrtf(temp0 * temp0 * temp1);
		}
	};
}
