#pragma once

#include "UnlockedRepresentationsTable.h"
#include "UnlockedWorldsTable.h"

#include "DatabaseCore/StatementBase.h"
#include "Logger/Logger.h"

namespace Operator
{
	class UnlockedWorldStatement
		:
		public Database::StatementBase
	{
	public:
		UserID userID;
		Resource::WorldID worldID;

		int raw_execute(
			Database::SQLiteDatabase* const database,
			Database::SQLiteStatement& statement) override
		{
			std::stringstream ss;

			ss << "SELECT ";
			ss << UnlockedWorldsTable::getTableDefinition()->columns[UnlockedWorldsTable::Column::WorldID];
			ss << " FROM ";
			ss << UnlockedWorldsTable::getTableDefinition()->name;
			ss << " WHERE  ";
			ss << UnlockedWorldsTable::getTableDefinition()->columns[UnlockedWorldsTable::Column::PlayerID];
			ss << " = ";
			ss << userID;
			ss << ";";

			const std::string command = ss.str();

			Log::Information(L"Executing " + carrtowstr(command.c_str()));

			return sqlite3_prepare_v2(
				database->getSQLite(),
				command.c_str(),
				(int)command.size(),
				&statement.statement,
				NULL);
		}

	private:
		bool apply(sqlite3_stmt* const statement) override
		{
			if (sqlite3_column_type(statement, 0) == SQLITE_NULL)
			{
				Log::Error(L"Got null in apply in unlockedworlds statement");

				return false;
			}

			worldID = sqlite3_column_int64(statement, 0);

			return true;
		}
	};

	class UnlockedRepresentationStatement
		:
		public Database::StatementBase
	{
	public:
		UserID userID;
		Resource::RepresentationID representationID;

		int raw_execute(
			Database::SQLiteDatabase* const database,
			Database::SQLiteStatement& statement) override
		{
			std::stringstream ss;

			ss << "SELECT ";
			ss << UnlockedRepresentationsTable::getTableDefinition()->columns[UnlockedRepresentationsTable::Column::RepresentationID];
			ss << " FROM ";
			ss << UnlockedRepresentationsTable::getTableDefinition()->name;
			ss << " WHERE  ";
			ss << UnlockedRepresentationsTable::getTableDefinition()->columns[UnlockedRepresentationsTable::Column::PlayerID];
			ss << " = ";
			ss << userID;
			ss << ";";

			const std::string command = ss.str();

			Log::Information(L"Executing " + carrtowstr(command.c_str()));

			return sqlite3_prepare_v2(
				database->getSQLite(),
				command.c_str(),
				(int)command.size(),
				&statement.statement,
				NULL);
		}

	private:
		bool apply(sqlite3_stmt* const statement) override
		{
			if (sqlite3_column_type(statement, 0) == SQLITE_NULL)
			{
				Log::Error(L"Got null in apply in unlockedrepresentation statement");

				return false;
			}

			representationID = sqlite3_column_int64(statement, 0);

			return true;
		}
	};

	int GetUnlockedWorlds(
		UserID userID,
		std::vector<Resource::WorldID>& worlds);
	int GetUnlockedRepresentations(
		UserID userID,
		std::vector<Resource::RepresentationID>& representations);
}
