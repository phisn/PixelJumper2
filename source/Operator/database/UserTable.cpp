#include "UserTable.h"

const Database::TableDefinition Operator::UserTable::definition
{
	"users",
	"id",
	"username",
	"hash",
	"salt",
	"token"
};
