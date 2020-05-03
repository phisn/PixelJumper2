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
			typedef std::pair<float, Connection&> ConnectionOffsetPair;

			enum Side
			{
				Top,
				Left,
				Bottom,
				Right
			};

			sf::FloatRect src_gb = getGlobalBounds();
			sf::Vector2f offset_side[4] = 
			{
				sf::Vector2f{ src_gb.left / 2.f, 0 },
				sf::Vector2f{ 0, src_gb.top / 2.f },
				sf::Vector2f{ src_gb.left / 2.f, src_gb.top },
				sf::Vector2f{ src_gb.left, src_gb.top / 2.f }
			};

			std::vector<ConnectionOffsetPair> connections_per_side[4];

			for (int i = 0; i < 4; ++i)
				connections_per_side[i].reserve(connections.size());

			// sort connections to possible sides
			for (Connection& connection : connections)
			{
				sf::Vector2f offset = connection.worldNode->rect.getPosition() + rect.getPosition();
				sf::FloatRect tgt_gb = connection.worldNode->getGlobalBounds();

				if (src_gb.top > tgt_gb.top + tgt_gb.height)
				{
					sf::Vector2f top_offset = offset - offset_side[Top];
					connections_per_side[Top].emplace_back(
						sqrtf(top_offset.x * top_offset.x + top_offset.y * top_offset.y),
						connection);
				}
				else if (src_gb.top + src_gb.height < tgt_gb.top)
				{
					sf::Vector2f bottom_offset = offset - offset_side[Bottom];
					connections_per_side[Bottom].emplace_back(
						sqrtf(bottom_offset.x * bottom_offset.x + bottom_offset.y * bottom_offset.y),
						connection);
				}

				if (src_gb.left > tgt_gb.left + tgt_gb.width)
				{
					sf::Vector2f left_offset = offset - offset_side[Left];
					connections_per_side[Left].emplace_back(
						sqrtf(left_offset.x * left_offset.x + left_offset.y * left_offset.y),
						connection);
				}
				else if (src_gb.left + src_gb.width < tgt_gb.left)
				{
					sf::Vector2f right_offset = offset - offset_side[Right];
					connections_per_side[Right].emplace_back(
						sqrtf(right_offset.x * right_offset.x + right_offset.y * right_offset.y),
						connection);
				}
			}

			// sort for all sides nodes by distance
			for (int i = 0; i < 4; ++i)
				std::sort(
					connections_per_side[i].begin(),
					connections_per_side[i].end(),
					[](ConnectionOffsetPair& p0, ConnectionOffsetPair& p1)
					{
						return p0.first > p1.first;
					});

			int index_counter[4] = { };

			// filter double connections
			while (true)
			{
				int connections_sum = 0;
				for (int side = 0; side < 4; ++side)
					connections_sum += connections_per_side[side].size();

				if (connections_sum == connections.size())
					break;

				assert(connections_sum > connections.size());

				for (int side = 0; side < 4; ++side)
					if (index_counter[side] < connections_per_side[side].size())
					{
						Connection& connection = connections_per_side[side][index_counter[side]++].second;

						// connection can only exist one time per side
						for (int side_r = 0; side_r < 4; ++side_r)
							if (side_r != side)
							{
								auto rem = std::find_if(
									connections_per_side[side_r].begin(),
									connections_per_side[side_r].end(),
									[&connection](ConnectionOffsetPair& pair)
									{
										return pair.second.node == connection.node;
									});

								if (rem != connections_per_side[side_r].end())
									connections_per_side[side_r].erase(rem);
							}
					}
			}

			// sort all nodes by position
			for (int side = 0; side < 4; ++side)
				std::sort(
					connections_per_side[side].begin(),
					connections_per_side[side].end(),
					[side](ConnectionOffsetPair& p0, ConnectionOffsetPair& p1)
					{
						return side == Top || side == Bottom
							? p0.second.node->getPosition().x < p1.second.node->getPosition().x
							: p0.second.node->getPosition().y < p1.second.node->getPosition().y;
					});

			for (int i = 0; i < connections_per_side[Top].size(); ++i)
			{
				ConnectionOffsetPair& pair = connections_per_side[Top][i];

				pair.second.node->setEndpointPosition(
					world,
					Editor::ConnectionSide::Top,
					sf::Vector2f
					{
						src_gb.left + (i + 1) * src_gb.width / (connections_per_side[Top].size() + 1),
						src_gb.top
					});
			}

			for (int i = 0; i < connections_per_side[Left].size(); ++i)
			{
				ConnectionOffsetPair& pair = connections_per_side[Left][i];

				pair.second.node->setEndpointPosition(
					world,
					Editor::ConnectionSide::Left,
					sf::Vector2f
					{
						src_gb.left,
						src_gb.top + (i + 1) * src_gb.height / (connections_per_side[Left].size() + 1)
					});
			}

			for (int i = 0; i < connections_per_side[Bottom].size(); ++i)
			{
				ConnectionOffsetPair& pair = connections_per_side[Bottom][i];

				pair.second.node->setEndpointPosition(
					world,
					Editor::ConnectionSide::Bottom,
					sf::Vector2f
					{
						src_gb.left + (i + 1) * src_gb.width / (connections_per_side[Bottom].size() + 1),
						src_gb.top + src_gb.height
					});
			}

			for (int i = 0; i < connections_per_side[Right].size(); ++i)
			{
				ConnectionOffsetPair& pair = connections_per_side[Right][i];

				pair.second.node->setEndpointPosition(
					world,
					Editor::ConnectionSide::Right,
					sf::Vector2f
					{
						src_gb.left + src_gb.width,
						src_gb.top + (i + 1) * src_gb.height / (connections_per_side[Right].size() + 1)
					});
			}
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
