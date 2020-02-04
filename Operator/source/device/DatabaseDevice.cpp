#include "DatabaseDevice.h"

bool Device::Database::Initialize()
{
	sqlite3_initialize();
}

void Device::Database::Unintialize()
{
}
