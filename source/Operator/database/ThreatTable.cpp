#include "ThreatTable.h"

// "tdb." prefix because of attached database
const Database::TableDefinition Operator::ThreatTable::definition
{
	"tdb.threats",
	"pid",
	"id",
	"message",
	"level",
};
