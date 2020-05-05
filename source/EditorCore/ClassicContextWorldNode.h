#pragma once

#include "ClassicContextNode.h"
#include "ClassicContextConnectionNode.h"
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
				closeWindow();
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
		ClassicContextWorldNode(
			ClassicContextWindowAccess* access,
			ClassicWorldDataset* world)
			:
			access(access),
			world(world),
			worldDatasetListener(*world, [this]()
				{
					reconstructConnections();
					reconstructNodeContent();
				})
		{
			name.setFont(Framework::GetFont());
			name.setCharacterSize(30);

			setStyle(ClassicContextNodeStyle::Classic);

			reconstructNodeContent();
		}

		void addConnection(
			ClassicContextWorldNode* worldNode,
			ClassicContextConnectionNode* node)
		{
			Connection& connection = connections.emplace_back();

			// use dataset to indirectly change out
			connection.node = node;
			connection.worldNode = worldNode;

			Log::Information(L"created new one", worldNode, L"world", node, L"node");

			for (int i = 0; i < connections.size(); ++i)
				Log::Information(L"list", i, L"i", connections[i].worldNode, L"world", connections[i].node, L"node");
		}

		void draw(sf::RenderTarget* target)
		{
			target->draw(rect);
			target->draw(name);
		}

		Framework::IndependentPopupWindow* createPopupWindow() override
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

		sf::Vector2f getSize() const
		{
			return rect.getSize();
		}

		sf::FloatRect getGlobalBounds() override
		{
			return rect.getGlobalBounds();
		}

		bool contains(sf::Vector2f point) const override
		{
			return rect.getGlobalBounds().contains(point);
		}

		ClassicWorldDataset* getWorld()
		{
			return world;
		}

	private:
		ClassicContextWindowAccess* access;
		ClassicWorldDataset* world;

		ClassicWorldDataset::ListenerContainer worldDatasetListener;

		sf::RectangleShape rect;
		sf::Text name;

		struct Connection
		{
			bool out = false;

			ClassicContextWorldNode* worldNode;
			ClassicContextConnectionNode* node;
		};

		std::vector<Connection> connections;

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

			// reconstruct positions only for this node and
			// notify other nodes directly because a calling chain
			// is not needed when just our positions changes
			reconstructConnectionsPosition();

			for (Connection& connection : connections)
				connection.worldNode->reconstructConnectionsPosition();
		}

		void reconstructConnectionsPosition()
		{
			struct ConnectionContainer
			{
				// less is more important
				float value;
				Connection* connection;
			};

			enum Side
			{
				Top,
				Left,
				Bottom,
				Right,

				Invalid
			};

			std::vector<ConnectionContainer> process_connections[4];
			std::vector<Connection*> final_connections[4];

			// also increased for every not used connection to
			// prevent extensive search for unused
			int final_connection_count = 0;

			sf::FloatRect src_gb = getGlobalBounds();
			for (Connection& connection : connections)
			{
				sf::FloatRect tgt_gb = connection.worldNode->getGlobalBounds();

				Side vertical_side = Invalid;
				Side horizont_side = Invalid;

				if (src_gb.top > tgt_gb.top + tgt_gb.height)
				{
					vertical_side = Top;
				}
				else if (src_gb.top + src_gb.height < tgt_gb.top)
				{
					vertical_side = Bottom;
				}

				if (src_gb.left > tgt_gb.left + tgt_gb.width)
				{
					horizont_side = Left;
				}
				else if (src_gb.left + src_gb.width < tgt_gb.left)
				{
					horizont_side = Right;
				}

				if (vertical_side == Invalid)
				{
					++final_connection_count;

					if (horizont_side != Invalid)
					{
						final_connections[horizont_side].push_back(&connection);
					}

					continue;
				}
				else if (horizont_side == Invalid)
				{
					++final_connection_count;
					final_connections[vertical_side].push_back(&connection);

					continue;
				}

				sf::Vector2f src_p = src_gb.getPosition() + src_gb.getSize() / 2.f;
				sf::Vector2f tgt_p = tgt_gb.getPosition() + tgt_gb.getSize() / 2.f;

				float side0 = (tgt_p.y - src_p.y) - (tgt_p.x - src_p.x);
				float side1 = (tgt_p.y - src_p.y) + (tgt_p.x - src_p.x);

				float value0 = get_distance_to_line(
					src_p,
					tgt_p,
					{ 1,  1 });
				float value1 = get_distance_to_line(
					src_p,
					tgt_p,
					{ 1, -1 });

				float value_primary, side_primary;
				if (value0 < value1)
				{
					value_primary = value0;
					side_primary = side0;
				}
				else
				{
					value_primary = value1;
					side_primary = -side1;
				}

				ConnectionContainer& connection_vertical = process_connections[vertical_side].emplace_back();
				ConnectionContainer& connection_horizont = process_connections[horizont_side].emplace_back();

				connection_vertical.connection = &connection;
				connection_horizont.connection = &connection;

				connection_vertical.value = value_primary *
					((vertical_side == Top) == (side_primary < 0) == (value0 < value1)
						? -1
						:  1);
				connection_horizont.value = value_primary *
					((horizont_side == Left) == (side_primary > 0)
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
						[](const ConnectionContainer& p0, const ConnectionContainer& p1)
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
							final_connections[side].size() < final_connections[min_value].size() ||
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
								[&final_connections, min_value](const ConnectionContainer p0)
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
					[side](Connection* p0, Connection* p1)
					{
						return (side == Top || side == Bottom)
							? p0->worldNode->getPosition().x < p1->worldNode->getPosition().x
							: p0->worldNode->getPosition().y < p1->worldNode->getPosition().y;
					});

			for (int side = 0; side < 4; ++side)
				for (int i = 0; i < final_connections[side].size(); ++i)
					if (side == Top || side == Bottom)
					{
						final_connections[side][i]->node->setEndpointPosition(
							world,
							Editor::ConnectionSide::Top,
							sf::Vector2f
							{
								src_gb.left + (i + 1) * src_gb.width / (final_connections[side].size() + 1),
								src_gb.top + (side == Top ? 0 : src_gb.height)
							});
					}
					else
					{
						final_connections[side][i]->node->setEndpointPosition(
							world,
							Editor::ConnectionSide::Left,
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

		void reconstructConnections()
		{
			for (ClassicWorldDataset* transitive : this->world->transitive)
			{
				enforceConnection(transitive);
			}
		}

		void enforceConnection(ClassicWorldDataset* world)
		{
			for (Connection& connection : connections)
				if (connection.worldNode->world == world)
				{
					if (!connection.out)
					{
						connection.out = true;
						connection.node->setEndpointOut(world, true);
					}

					return;
				}

			// window handles node creation
			// if target wants to connect too he
			// can do it later on event
			access->createLink(this->world, world);
		}
	};
}
