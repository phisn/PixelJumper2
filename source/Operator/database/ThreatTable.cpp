#include "ThreatTable.h"

// "tdb." prefix because of attached database
const Database::TableDefinition Operator::ThreatTable::definition
{
	"db.threats",
	"pid",
	"id",
	"message",
	"level",
};
