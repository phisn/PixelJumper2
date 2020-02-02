#pragma once

#include <Client/source/device/NetDevice.h>

#include <Client/source/game/AuthenticationHandler.h>
#include <Client/source/game/Simulation.h>
#include <Client/source/game/VirtualPlayer.h>

namespace Game::Net
{
	class RemoteConnection
		:
		public AuthenticationHandler
	{
	public:
		struct Error
		{
			enum : sf::Uint32
			{
				_Begin = 0xa0,
				InvalidSimulationState
			};
		};

		struct Settings
		{
			// time after how many updates the current 
			// status will cause a hard timeout
			sf::Uint32 pendingTimeout = 2;
			sf::Uint32 runningTimeout = 4;

			// tickratedifference represents the
			// difference between client tickrate and
			// the server tickrate. this tickrate
			// gets adjusted to prevent  
			int minTickrateDifference;
			int maxTickrateDifference;
		};

		RemoteConnection(const Settings settings)
			:
			settings(settings)
		{
		}

		bool initialize()
		{
		}

		// called on every 
		bool processLogic()
		{
			if (simulation->getStatus() == ClassicSimulation::Running)
			{
				if (player->hasUpdate())
				{
					if (simulation->processLogic())
					{
					}
				}
			}
		}

		void update() override
		{
			if (!process())
			{
				CommonErrorMessage message;

				message.content.errorID = CommonErrorID::InvalidConnectionInClientHandlerProcess;
				message.content.messageID = 0;
				message.message = L"Internal server error";

				Resource::WritePipe* const pipe = beginMessage(CommonMessageID::Error,
					sizeof(message.content) + message.message.size() * 2 + 8);

				if (!message.save(pipe))
				{
					Log::Error(L"Failed to create error message");
					status = Disconnecting;
				}
				else
				{
					sendMessage();
				}

				status = Disconnecting;

				return;
			}

			switch (status)
			{
			case Connecting:
				AuthenticationHandler::update();

				break;
			case Connected:


				break;
			default:
				// update should neither be called with
				// disconnecting nor disconnected
				Log::Error(L"Got invalid status in update in remoteconnection");

				break;
			}

			switch (simulation->getStatus())
			{
			case ClassicSimulation::Status::Running:
			case ClassicSimulation::Status::Shutdown:
			case ClassicSimulation::Status::Error:

			default:
				// missing * cant happen in remote connection
				// results in error

				/*
				sendError(
					Log::Convert(L"Got invalid simulation status",
						(int) simulation->getStatus(), L"status"),
					Error::InvalidSimulationState);
					*/

			}
		}

	private:
		const Settings settings;

		ClassicSimulation* simulation = NULL;
		VirtualPlayer* player = NULL;

		Resource::PlayerResource* playerResource;

		void onMessage(
			const Device::Net::MessageID messageID,
			Resource::ReadPipe* const pipe) override
		{
			switch (status)
			{
			case Connecting:
				AuthenticationHandler::onMessage(messageID, pipe);

				break;
			case Connected:
				switch (messageID)
				{
				case Client::ClassicalConnectionMessageID::RequestSimulation:
					onRequestSimulation(pipe);

					break;
				case Client::ClassicalConnectionMessageID::AcceptSync:
					
					
					break;
				case Client::ClassicalConnectionMessageID::RequestSync:
					
					
					break;
				case Client::ClassicalConnectionMessageID::PushMovement:
					
					
					break;
				}

				break;
			default:
				// onMessage should neither be called with
				// disconnecting nor disconnected
				Log::Error(L"Got invalid status in onMessage in remoteconnection");

				break;
			}
		}

		void onRequestSimulation(Resource::ReadPipe* const pipe)
		{
			if (simulation == NULL)
			{
				Host::RejectSimulationRequestMessage message;
				message.reason = message.SimulationAlreadyRunning;

				safeMessageLoad(
					Host::ClassicalConnectionMessageID::RejectSimulationRequest,
					&message,
					beginMessage(Host::ClassicalConnectionMessageID::RejectSimulationRequest)
				);

				return;
			}

			Client::RequestSimulationMessage message;

			if (!message.load(pipe) && message.world != NULL)
			{
				onThreatIdentified(
					Client::ClassicalConnectionMessageID::RequestSimulation,
					L"invalid message content",
					Device::Net::ThreatLevel::Uncommon);

				Host::RejectSimulationRequestMessage message;
				message.reason = message.InvalidSimulationRequestContent;

				safeMessageLoad(
					Host::ClassicalConnectionMessageID::RejectSimulationRequest,
					&message,
					beginMessage(Host::ClassicalConnectionMessageID::RejectSimulationRequest)
				);

				return;
			}


		}

		void onThreatIdentified(
			const Device::Net::MessageID messageID,
			const wchar_t* const note,
			const Device::Net::ThreatLevel level) override
		{
			SteamNetConnectionInfo_t info;
			if (networkInterface->GetConnectionInfo(connection, &info))
			{
				Log::Warning(L"Threat identified: " + std::wstring(note),
					(int) level, L"level",
					(int) messageID,
					(int) info.m_addrRemote.GetIPv4(), L"ipv4",
					(int) info.m_addrRemote.m_port, L"port"); 
			}
			else
			{
				Log::Error(L"Invalid connection while threat is identified");
				Log::Warning(L"Threat identified: " + std::wstring(note),
					(int) level, L"level",
					(int) messageID, L"messageID",
					(int) connection, L"connection");
			}
		}

		void onClientConnected() override;
	};
}
