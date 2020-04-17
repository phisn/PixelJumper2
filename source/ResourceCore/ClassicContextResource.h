#pragma once

#include "ResourceInterface.h"
#include "WorldResource.h"

#include <map>
#include <vector>

namespace Resource
{
	extern const Resource::ResourceTypeDefinition ClassicContextResourceDefinition;

	// does not conain actual world
	class ClassicWorldResource
		:
		public ResourceBase
	{
	public:
		struct Content
		{
			WorldID worldID;

		} content;

		std::wstring name, description;

		std::vector<WorldID> transitive;
		std::vector<WorldID> unlock;

		bool make(ReadPipe* const pipe) override
		{
			if (!pipe->readValue(&content))
			{
				return false;
			}

			if (!pipe->readString(&name) ||
				!pipe->readString(&description))
			{
				return false;
			}

			if (!pipe->readVector(&transitive) ||
				!pipe->readVector(&unlock))
			{
				return false;
			}

			return true;
		}

		bool save(WritePipe* const pipe) override
		{
			if (!pipe->writeValue(&content))
			{
				return false;
			}

			if (!pipe->writeString(&name) ||
				!pipe->writeString(&description))
			{
				return false;
			}

			if (!pipe->writeVector(&transitive) ||
				!pipe->writeVector(&unlock))
			{
				return false;
			}

			return true;
		}
	};

	class ClassicStageResource
		:
		public ResourceBase
	{
	public:
		std::wstring name, description;

		std::vector<ClassicWorldResource> worlds;

		bool make(ReadPipe* const pipe) override
		{
			if (!pipe->readString(&name) ||
				!pipe->readString(&description))
			{
				return false;
			}

			if (!pipe->readVectorResource(&worlds))
			{
				return false;
			}

			return true;
		}

		bool save(WritePipe* const pipe) override
		{
			if (!pipe->writeString(&name) ||
				!pipe->writeString(&description))
			{
				return false;
			}

			if (!pipe->writeVectorResource(&worlds))
			{
				return false;
			}

			return true;
		}
	};

	class ClassicContextResource
		:
		public ResourceBase
	{
	public:
		std::vector<Resource::WorldID> initialUnlockedWorlds;
		std::vector<Resource::RepresentationID> initialUnlockedRepresentations;

		std::vector<ClassicStageResource> stages;
		std::vector<ClassicWorldResource> stagelessWorlds;

		bool make(ReadPipe* const pipe) override
		{
			if (!pipe->readVector(&initialUnlockedWorlds) ||
				!pipe->readVectorResource(&stages) ||
				!pipe->readVectorResource(&stagelessWorlds))
			{
				return false;
			}

			return true;
		}

		bool save(WritePipe* const pipe) override
		{
			if (!pipe->writeVector(&initialUnlockedWorlds) ||
				!pipe->writeVectorResource(&stages) ||
				!pipe->writeVectorResource(&stagelessWorlds))
			{
				return false;
			}

			return true;
		}
	};

	typedef std::map<WorldID, ClassicWorldResource*> ClassicWorldContainer;
}
