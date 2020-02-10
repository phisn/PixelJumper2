#include "UserTable.h"

const Database::TableDefinition Database::UserTable::definition
{
	"users",
	"id",
	"username",
	"hash",
	"salt",
	"token"
};
