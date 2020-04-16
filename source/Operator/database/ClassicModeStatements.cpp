#include "ClassicModeStatements.h"

namespace Operator
{
	int GetUnlockedWorlds(UserID userID, std::vector<Resource::WorldID>& worlds)
	{
		UnlockedWorldStatement uws;
		return Database::ExecuteStatement<UnlockedWorldStatement, OperatorDatabaseStatement>(
			OperatorDatabase::GetSQLiteDatabase(),
			uws,
			[&uws, &worlds]()
			{
				worlds.push_back(uws.worldID);

			});
	}
	int GetUnlockedRepresentations(UserID userID, std::vector<Resource::RepresentationID>& representations)
	{
		UnlockedRepresentationStatement urs;
		return Database::ExecuteStatement<UnlockedRepresentationStatement, OperatorDatabaseStatement>(
			OperatorDatabase::GetSQLiteDatabase(),
			urs,
			[&urs, &representations]()
			{
				representations.push_back(urs.representationID);

			});
	}
}