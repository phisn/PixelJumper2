#pragma once

#include <Operator/source/database/TableBase.h>
#include <Operator/source/database/StatementBase.h>

namespace Device::Database
{
	bool Initialize();
	void Unintialize();

	enum class ExtractionResult
	{
		Found,
		NotFound,
		Error
	};

	bool BeginTransaction();
	bool EndTransaction();
	bool RollbackTransaction();

	bool Exists(::Database::TableBase* const table);
	bool Insert(::Database::TableBase* const table);
	bool Edit(::Database::TableBase* const table);

	ExtractionResult Extract(::Database::TableBase* const table);

	bool FinalizeStatement(sqlite3_stmt* const statement);

	sqlite3* GetConnection();
	std::wstring GenerateErrorMessage();
}
