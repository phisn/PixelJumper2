#pragma once

#include <Operator/source/database/TableBase.h>
#include <Operator/source/database/StatementBase.h>

#include <string>

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

	bool FinalizeStatement(sqlite3_stmt* const statement);

	sqlite3* GetConnection();
	std::wstring GenerateErrorMessage();
}
