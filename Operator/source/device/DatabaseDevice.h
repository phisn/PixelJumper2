#pragma once

#include <Operator/source/database/TableBase.h>

namespace Device::Database
{
	bool Initialize();
	void Unintialize();

	bool Extract(::Database::TableBase* const table);
	bool Insert(::Database::TableBase* const table);
	bool Edit(::Database::TableBase* const table);

	sqlite3* GetConnection();
}
