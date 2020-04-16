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

	Resource::UnlockID UnlockBuffer::UnlockWorld(
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

		return unlockID;
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
			Net::MessageID messageID;
			Operator::RequestInterface* request = NULL;
			Net::NetworkMessage* message;

			switch (iterator->content.type)
			{
			case Resource::OperatorUnlockType::World:
				messageID = Net::Client::OperatorClassicHostID::UnlockWorld;
				request = new Operator::ManagedWorldUnlockRequest;
				
				{
					Net::Client::OperatorClassicHost::RequestUnlockWorldMessage* unlock_message =
						new Net::Client::OperatorClassicHost::RequestUnlockWorldMessage;

					message = unlock_message;
					unlock_message->content.worldID = iterator->content.worldID;
				}

				break;
			default:
				Log::Warning(L"got invalid type in unlockbuffer");

				break;
			}

			// request can not be null when the
			// request was correctly loaded
			if (request == NULL)
			{
				iterator = unlockBuffer.unlocks.erase(iterator);
				continue;
			}


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
}
