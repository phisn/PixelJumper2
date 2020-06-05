#pragma once

#include "ClassicContextWorldNode.h"
#include "TransitiveContextNode.h"

#include "EditorCore/WindowManager.h"
#include "EditorCore/EditorFailureScene.h"

namespace Editor::ClassicContext
{
	class ClassicContextManager
	{
	public:
		ClassicContextManager(Resource::ContextID contextID)
			:
			contextID(contextID)
		{
			// error handling is already done inside
			initializeContextContent() &&
				initializeWorldNodes() &&
				initializeTransitiveConnectionNodes();
		}

		~ClassicContextManager()
		{
			for (Node* node : nodes)
				delete node;
		}

		void draw(sf::RenderTarget* target)
		{
			for (ConnectionNode* transitive : transitives)
				transitive->draw(target);
			for (WorldNode* world : worlds)
				world->draw(target);
		}

		bool createWorldNode(Resource::WorldID worldID)
		{
			typedef std::tuple<SQLiteString> CreateWorldTuple;

			CreateWorldTuple createWorld;
			if (!Database::Statement<CreateWorldTuple>(
					EditorDatabase::Instance(),
					"SELECT name FROM world WHERE id = ?", 
					worldID).execute(createWorld))
			{
				Framework::Core::PushChildScene(new EditorFailureScene(
					"Failed to retrive information to create world in contextwindow '" + std::to_string(worldID) + "'"));

				return false;
			}

			WorldNode* node = new WorldNode(worldID, std::get<0>(createWorld));

			nodes.push_back(node);
			worlds.push_back(node);

			return true;
		}

		bool createTransitiveNode(Resource::WorldEntryID entryID)
		{
			typedef std::tuple<SQLiteInt, SQLiteInt, SQLiteString> CreateTransitiveTuple;

			CreateTransitiveTuple createTransitive;
			if (!Database::Statement<CreateTransitiveTuple>(
					EditorDatabase::Instance(),
					"SELECT outputid, inputid, name FROM transitive WHERE id = ?", 
					entryID).execute(createTransitive))
			{
				Framework::Core::PushChildScene(new EditorFailureScene(
					"Failed to retrive information to create transitive in contextwindow '" + std::to_string(entryID) + "'"));

				return false;
			}

			WorldNode* outputNode = findWorldNodeByID(std::get<0>(createTransitive));
			WorldNode* inputNode = findWorldNodeByID(std::get<1>(createTransitive));

			if (outputNode == NULL || inputNode == NULL)
			{
				Framework::Core::PushChildScene(new EditorFailureScene(
					"Failed to find nodes to create transitive in contextwindow '" + std::to_string(entryID) + "'"));

				return false;
			}

			TransitiveNode* node = new TransitiveNode(
				outputNode,
				inputNode,
				entryID,
				std::get<2>(createTransitive));

			outputNode->addTransitiveConnection(inputNode, node);
			inputNode->addTransitiveConnection(outputNode, node);

			outputNode->notifyBoundsChanged();
			inputNode->notifyBoundsChanged();

			nodes.push_back(node);
			transitives.push_back(node);

			return true;
		}

		bool removeWorldNode(Resource::WorldID worldID)
		{
			decltype(worlds)::iterator iterator = std::find_if(
				worlds.begin(),
				worlds.end(),
				[worldID](WorldNode* node)
				{
					return node->getID() == worldID;
				});

			if (iterator == worlds.end())
				return false;

			worlds.erase(iterator);

			return true;
		}

		bool removeTransitiveNode(Resource::WorldEntryID entryID)
		{
			decltype(transitives)::iterator iterator = std::find_if(
				transitives.begin(),
				transitives.end(),
				[entryID](TransitiveNode* node)
				{
					return node->getID() == entryID;
				});

			TransitiveNode* node = *iterator;

			if (node == NULL)
				return false;

			node->getSource()->removeTransitiveConnection(node);
			node->getTarget()->removeTransitiveConnection(node);

			transitives.erase(iterator);

			return true;
		}

		Node* findNodeByPoint(sf::Vector2f coord) const
		{
			for (Node* node : nodes)
				if (node->contains(coord))
				{
					return node;
				}

			return NULL;
		}

		WorldNode* findWorldNodeByPoint(sf::Vector2f coord) const
		{
			for (WorldNode* node : worlds)
				if (node->contains(coord))
				{
					return node;
				}

			return NULL;
		}

		WorldNode* findWorldNodeByID(Resource::WorldID id) const
		{
			for (WorldNode* node : worlds)
				if (node->getID() == id)
				{
					return node;
				}

			return NULL;
		}
		
		TransitiveNode* findTransitiveNodeByID(Resource::WorldEntryID id) const
		{
			for (TransitiveNode* node : transitives)
				if (node->getID() == id)
				{
					return node;
				}

			return NULL;
		}

		void findCollidingNodes(sf::FloatRect rect, std::vector<Node*>& collidingNodes)
		{
			for (Node* node : nodes)
				if (doesRectContainRect(rect, node->getGlobalBounds()))
				{
					collidingNodes.push_back(node);
				}
		}

		Resource::ContextID getContextID() const
		{
			return contextID;
		}

	private:
		Resource::ContextID contextID;
		std::string name, description;

		std::list<Node*> nodes;

		std::vector<TransitiveNode*> transitives;
		std::vector<WorldNode*> worlds;

		bool initializeContextContent()
		{
			typedef std::tuple<Database::SQLiteString, Database::SQLiteString> ContextNameTuple;

			ContextNameTuple thisTuple;
			if (!Database::Statement<ContextNameTuple>(
				EditorDatabase::Instance(),
				"SELECT name, description FROM context WHERE id = ?", contextID).execute(thisTuple))
			{
				Framework::Core::PushChildScene(new EditorFailureScene(
					"Failed to load context information '" + std::to_string(contextID) + "'"));

				return false;
			}

			name = std::move(std::get<0>(thisTuple));
			description = std::move(std::get<1>(thisTuple));

			return true;
		}

		bool initializeWorldNodes()
		{
			typedef std::tuple<SQLiteInt, SQLiteString> NodeWorldTuple;

			Database::Statement<NodeWorldTuple> findWorlds(
				EditorDatabase::Instance(),
				"SELECT id, name FROM world WHERE contextid = ?", contextID);

			for (const NodeWorldTuple& tuple : findWorlds)
			{
				WorldNode* node = new WorldNode(
					std::get<0>(tuple),
					std::get<1>(tuple));

				nodes.push_back(node);
				worlds.push_back(node);
			}

			if (!findWorlds)
			{
				Framework::Core::PushChildScene(new EditorFailureScene(
					"Failed to load worlds for context '" + std::to_string(contextID) + "'",
					[this]() -> bool
					{
						return removeContextFromDatabase();
					}));

				return false;
			}

			return true;
		}

		bool initializeTransitiveConnectionNodes()
		{
			typedef std::tuple<SQLiteInt, SQLiteString, SQLiteInt, SQLiteInt> NodeTransitiveTuple;

			Database::Statement<NodeTransitiveTuple> findTransitives(
				EditorDatabase::Instance(),
				"SELECT id, name, outputid, inputid FROM transitive WHERE contextid = ?", contextID);

			for (const NodeTransitiveTuple& tuple : findTransitives)
			{
				Resource::WorldEntryID entryID = std::get<0>(tuple);

				WorldNode* outputNode = findWorldNodeByID(std::get<2>(tuple));
				WorldNode* inputNode = findWorldNodeByID(std::get<3>(tuple));

				if (outputNode == NULL || inputNode == NULL)
				{
					Framework::Core::PushChildScene(new EditorFailureScene(
						"Got invalid worldID in transitive '" + std::to_string(entryID) + "'",
						[entryID]() -> bool
						{
							return Database::Statement<>(
								EditorDatabase::Instance(),
								"DELETE FROM transitive WHERE id = ?", entryID).execute();
						}));

					return false;
				}

				TransitiveNode* node = new TransitiveNode(
					outputNode,
					inputNode,
					std::get<0>(tuple),
					std::get<1>(tuple));

				outputNode->addTransitiveConnection(inputNode, node);
				inputNode->addTransitiveConnection(outputNode, node);
				
				outputNode->notifyBoundsChanged();
				inputNode->notifyBoundsChanged();

				nodes.push_back(node);
				transitives.push_back(node);
			}

			if (!findTransitives)
			{
				Framework::Core::PushChildScene(new EditorFailureScene(
					"Failed to load transitives for context '" + std::to_string(contextID) + "'",
					[this]() -> bool
					{
						return removeContextFromDatabase();
					}));

				return false;
			}

			return true;
		}

		bool removeContextFromDatabase()
		{
			return Database::Statement<>(
				EditorDatabase::Instance(),
				"DELETE FROM context WHERE id = ?", contextID).execute();
		}

		bool doesRectContainRect(sf::FloatRect rect1, sf::FloatRect rect2) const
		{
			return (rect1.left < rect2.left + rect2.width)
				&& (rect1.top < rect2.top + rect2.height)
				&& (rect2.left < rect1.left + rect1.width)
				&& (rect2.top < rect1.top + rect1.height);
		}
	};
}
