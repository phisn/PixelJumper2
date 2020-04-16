#include "EmptyKeyStatement.h"

namespace Operator
{
	int GetEmptyKeys(std::vector<Operator::RegistrationKey>& keys)
	{
		EmptyKeysStatement emptyKeys;
		return Database::ExecuteStatement<EmptyKeysStatement, OperatorDatabaseStatement>(
			OperatorDatabase::GetSQLiteDatabase(),
			emptyKeys,
			[&emptyKeys, &keys]()
			{
				keys.emplace_back();

				memcpy(
					keys.back().content,
					emptyKeys.key.content,
					OPERATOR_KEY_SIZE);
			});
	}
}