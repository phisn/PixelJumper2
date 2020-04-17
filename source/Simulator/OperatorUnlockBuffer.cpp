#include "OperatorUnlockBuffer.h"
#include "ManagedUnlockRequest.h"

#include "Common/RandomModule.h"
#include "OperatorClient/OperatorClient.h"
#include "OperatorClient/request/ClassicHostRequest.h"
#include "ResourceCore/pipes/FilePipe.h"

#include <map>

namespace
{
	Resource::OperatorUnlockBufferResource unlockBuffer;
}

const Resource::ResourceTypeDefinition Resource::UnlockBufferResourceDefinition
{
	L"unlock buffer",
	L"",
	L".ulb"
};

namespace Simulator
{
	void SaveUnlockBuffer();
	Operator::Client::PushRequestFailure ProcessUnlock(const Resource::OperatorUnlockResource& unlock);

	bool UnlockBuffer::Initialize()
	{
		if (!Resource::Interface::LoadResource(
				SIMULATOR_UNLOCK_BUFFER_FILENAME,
				&unlockBuffer,
				Resource::UnlockBufferResourceDefinition))
		{
			Log::Error(L"failed to load unlockbuffer");
			return false;
		}

		return true;
	}

	void UnlockBuffer::UnlockWorld(
		Operator::UserID userID,
		Resource::WorldID worldID)
	{
		Resource::UnlockID unlockID = Module::Random::MakeRandom<Resource::UnlockID>();
		Resource::OperatorUnlockResource& resource = unlockBuffer.unlocks.emplace_back();

		resource.content.type = Resource::OperatorUnlockType::World;
		resource.content.unlockID = unlockID;
		resource.content.userID = userID;
		resource.content.worldID = worldID;

		SaveUnlockBuffer();

		Operator::Client::PushRequestFailure result = ProcessUnlock(unlockBuffer.unlocks.back());

		if (result != Operator::Client::PushRequestFailure::Success)
		{
			Log::Error(L"failed to process unlock",
				result, L"result");
		}
	}

	void UnlockBuffer::UnlockRepresentation(
		Operator::UserID userID,
		Resource::RepresentationID representationID)
	{
		Resource::UnlockID unlockID = Module::Random::MakeRandom<Resource::UnlockID>();
		Resource::OperatorUnlockResource& resource = unlockBuffer.unlocks.emplace_back();

		resource.content.type = Resource::OperatorUnlockType::Representation;
		resource.content.unlockID = unlockID;
		resource.content.userID = userID;
		resource.content.representationID = representationID;

		SaveUnlockBuffer();

		Operator::Client::PushRequestFailure result = ProcessUnlock(unlockBuffer.unlocks.back());

		if (result != Operator::Client::PushRequestFailure::Success)
		{
			Log::Error(L"failed to process unlock",
				result, L"result");
		}
	}

	void UnlockBuffer::ReleaseUnlock(Resource::UnlockID unlockID)
	{
		decltype(unlockBuffer.unlocks)::iterator unlock = std::find_if(
			unlockBuffer.unlocks.begin(),
			unlockBuffer.unlocks.end(),
			[unlockID](const Resource::OperatorUnlockResource& unlock)
			{
				return unlock.content.unlockID == unlockID;
			});

		if (unlock == unlockBuffer.unlocks.end())
		{
			Log::Warning(L"tried to release invalid unlock",
				unlockID, L"unlockID",
				unlockBuffer.unlocks.size(), L"active_unlocks");
		}
		else
		{
			unlockBuffer.unlocks.erase(unlock);
			SaveUnlockBuffer();
		}
	}

	void UnlockBuffer::OnOperatorConnected()
	{
		decltype(unlockBuffer.unlocks)::iterator iterator = unlockBuffer.unlocks.begin();

		while (iterator != unlockBuffer.unlocks.end())
		{
			Operator::Client::PushRequestFailure result = ProcessUnlock(*iterator);

			if (result != Operator::Client::PushRequestFailure::Success)
			{
				Log::Error(L"failed to push pending unlock request",
					result, L"result",
					iterator->content.unlockID, L"unlockID",
					iterator->content.userID, L"userID",
					iterator->content.type, L"type");

				switch (result)
				{
				case Operator::Client::PushRequestFailure::Unauthenticated:
				case Operator::Client::PushRequestFailure::Authenticating:
				case Operator::Client::PushRequestFailure::ConnectingFailed:
					
					// unable to process other unlocks because they will fail
					// the same way
					return;
				case Operator::Client::PushRequestFailure::SendingFailed:
					iterator = unlockBuffer.unlocks.erase(iterator);

					break;
				default:
					assert(false);

					break;
				}
			}
			else ++iterator;
		}
	}

	void SaveUnlockBuffer()
	{
		if (!Resource::Interface::SaveResource(
				SIMULATOR_UNLOCK_BUFFER_FILENAME,
				&unlockBuffer,
				Resource::UnlockBufferResourceDefinition))
		{
			// there is nothing that we can do about it than just
			// hope that it is send to the operator
			Log::Error(L"unlock buffer failed to save",
				unlockBuffer.unlocks.size());
		}
	}

	Operator::Client::PushRequestFailure ProcessUnlock(const Resource::OperatorUnlockResource& unlock)
	{
		Net::MessageID messageID;
		Operator::RequestInterface* request = NULL;
		Net::NetworkMessage* message;

		switch (unlock.content.type)
		{
		case Resource::OperatorUnlockType::World:
			messageID = Net::Client::OperatorClassicHostID::UnlockWorld;
			request = new Operator::ManagedWorldUnlockRequest{ unlock.content.unlockID };

			{
				Net::Client::OperatorClassicHost::UnlockWorldMessage* unlock_message =
					new Net::Client::OperatorClassicHost::UnlockWorldMessage;

				message = unlock_message;
				unlock_message->content.worldID = unlock.content.worldID;
			}

			break;
		case Resource::OperatorUnlockType::Representation:
			messageID = Net::Client::OperatorClassicHostID::UnlockRepresentation;
			request = new Operator::ManagedRepresentationUnlockRequest{ unlock.content.unlockID };

			{
				Net::Client::OperatorClassicHost::UnlockRepresentationMessage* unlock_message =
					new Net::Client::OperatorClassicHost::UnlockRepresentationMessage;

				message = unlock_message;
				unlock_message->content.representationID = unlock.content.representationID;
			}

			break;
		default:
			Log::Error(L"got invalid type in unlockbuffer");
			return Operator::Client::PushRequestFailure::SendingFailed;
		}

		Operator::Client::PushRequestFailure result = Operator::Client::PushRequest(
			messageID,
			message,
			request);

		if (result == Operator::Client::PushRequestFailure::Authenticating ||
			result == Operator::Client::PushRequestFailure::Unauthenticated)
		{
			delete request;
		}

		return result;
	}
}
