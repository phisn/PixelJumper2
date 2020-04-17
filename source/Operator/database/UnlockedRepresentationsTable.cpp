#include "UnlockedRepresentationsTable.h"

const Database::TableDefinition Operator::UnlockedRepresentationsTable::definition
{
	"unlockedrepresentations",
	"pid",
	"rid"
};

namespace Operator
{
	bool Operator::UnlockRepresentation(UserID userID, Resource::RepresentationID representationID)
	{
		Log::Section section{ L"unlocking representation",
				userID, L"userID",
				representationID, L"representationID" };

		UnlockedRepresentationsTable table;

		table.primary.userID = userID;
		table.primary.representationID = representationID;

		Database::ConditionResult result = table.exists(
			{
				UnlockedRepresentationsTable::Column::PlayerID,
				UnlockedRepresentationsTable::Column::RepresentationID
			});

		switch (result)
		{
		case Database::ConditionResult::Error:
			Log::Error(L"failed to identify weather a representation is unlocked",
				userID, L"userID",
				representationID, L"representationID");

			return false;
		case Database::ConditionResult::Found:
			Log::Warning(L"representation was already unlocked");

			return true;
		}

		if (!table.create())
		{
			Log::Error(L"failed to create representation unlock entry in database");

			return false;
		}

		return true;
	}
}
