#pragma once

#include "GameCore/net/ClassicSessionMessage.h"
#include "NetCore/RequestHandler.h"

namespace Game
{
	struct ClientClassicSessionHandlerCallback
	{
	};

	struct ClientClassicSessionHandlerArguments
	{
		ClientClassicSessionHandlerCallback* callback;
	};

	class ClientClassicSessionHandler
		:
		public ::Net::RequestHandler
	{
	public:
		struct Player
		{
			std::string username;
			Operator::UserID userID;
		};

		ClientClassicSessionHandler(const ClientClassicSessionHandlerArguments& arguments)
			:
			callback(arguments.callback)
		{
		}

		void update() override
		{
		}

		bool onMessage(
			const Net::MessageID messageID,
			Resource::ReadPipe* const pipe) override
		{
			switch (messageID)
			{
			case Net::Host::ClassicSessionMessageID::InitializeSession:
				if (Net::Host::ClassicSessionMessage::InitializeSession message; loadMessage(messageID, &message, pipe))
				{
					onInitializeSession(message);
				}

				return true;
			case Net::Host::ClassicSessionMessageID::AddPlayer:
			{
				Net::Host::ClassicSessionMessage::AddPlayer message;
				// dynamic player because player will be stored
				// in players
				message.player = new Resource::PlayerResource;

				if (loadMessage(messageID, &message, pipe))
				{
					onAddPlayer(message);
				}

				return true;
			}
			case Net::Host::ClassicSessionMessageID::RemovePlayer:
				if (Net::Host::ClassicSessionMessage::RemovePlayer message; loadMessage(messageID, &message, pipe))
				{
					onRemovePlayer(message);
				}

				return true;
			}

			return false;
		}

		const std::vector<Resource::PlayerResource*>& getPlayers() const
		{
			return players;
		}

		const SimulatorSettings& getSimulatorSettings() const
		{
			return simulatorSettings;
		}

	private:
		ClientClassicSessionHandlerCallback* const callback;
		SimulatorSettings simulatorSettings;

		std::vector<Resource::PlayerResource*> players;

		void onInitializeSession(const Net::Host::ClassicSessionMessage::InitializeSession& message)
		{
			simulatorSettings = message.content.settings;
			players.reserve(simulatorSettings.maxClients);

			for (Resource::PlayerResource* player : message.players)
				players.push_back(player);
		}

		void onAddPlayer(const Net::Host::ClassicSessionMessage::AddPlayer& message)
		{
			players.push_back(message.player);
		}

		void onRemovePlayer(const Net::Host::ClassicSessionMessage::RemovePlayer& message)
		{
			decltype(players)::iterator player = std::find_if(
				players.begin(),
				players.end(),
				[&message](Resource::PlayerResource* player)
				{
					return player->content.playerID == message.userID;
				});

			if (player != players.end())
			{
				players.erase(player);
			}
			else
			{
				Log::Warning(L"got removeplayer in clientclassicsessionhandler invalid userID",
					message.userID, L"userID");
			}
		}
	};
}