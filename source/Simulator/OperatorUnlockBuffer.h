#pragma once

#include "ResourceCore/ResourceInterface.h"
#include "ResourceCore/TrivialResource.h"
#include "ResourceCore/WorldResource.h"

#define SIMULATOR_UNLOCK_BUFFER_FILENAME L"unlockbuffer"

namespace Resource
{
	enum class OperatorUnlockType
	{
		World,
		Representation
	};

	typedef uint64_t UnlockID;

	struct OperatorUnlockResourceContent
	{
		UnlockID unlockID;

		Operator::UserID userID;
		OperatorUnlockType type;

		union
		{
			Resource::WorldID worldID;
			Resource::RepresentationID representationID;
		};
	};

	typedef TrivialResource<OperatorUnlockResourceContent> OperatorUnlockResource;

	struct OperatorUnlockBufferResource
		:
		public ResourceBase
	{
		std::vector<OperatorUnlockResource> unlocks;

		bool make(ReadPipe* const pipe) override
		{
			return pipe->readVectorResource(&unlocks);
		}

		bool save(WritePipe* const pipe) override
		{
			return pipe->writeVectorResource(&unlocks);
		}
	};

	const Resource::ResourceTypeDefinition UnlockBufferResourceDefinition;
}

namespace Simulator::UnlockBuffer
{
	bool Initialize();

	Resource::UnlockID UnlockWorld(
		Operator::UserID userID, 
		Resource::WorldID worldID);
	void ReleaseUnlock(Resource::UnlockID unlockID);

	// used to ensure all unlocks are unlocked as
	// soon as possible. called on bootup and after
	// every reconnect
	void OnOperatorConnected();
}

