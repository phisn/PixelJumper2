#pragma once

#include "DatabaseCore/DatabaseCore.h"

namespace Editor
{
	enum class DatabaseTable
	{
		Context,
		Transitive,
		World,
		Tile
	};

	typedef std::tuple<SQLiteInt, SQLiteString, SQLiteString> ContextTuple;

	struct ContextColumn
	{
		enum
		{
			ID,
			Name,
			Description
		};
	};

	typedef std::tuple<SQLiteInt, SQLiteString, SQLiteInt, SQLiteInt> TransitiveTuple;
	
	struct TransitiveColumn
	{
		enum
		{
			EntryID,
			Name,
			SourceID,
			TargetID
		};
	};

	typedef std::tuple<SQLiteInt, SQLiteString, SQLiteInt> WorldTuple;

	struct WorldColumn
	{
		enum
		{
			ID,
			Name,
			ContextID
		};
	};

	typedef std::tuple<SQLiteInt, SQLiteInt> TileTuple;

	struct TileColumn
	{
		enum
		{
			ID,
			WorldID
		};
	};

	class EditorDatabase
		:
		public Database::SQLiteDatabase
	{
		static EditorDatabase* instance;

	public:
		static EditorDatabase* Instance()
		{
			assert(instance != NULL);
			return instance;
		}

		EditorDatabase()
		{
			assert(instance == NULL);
			instance = this;
		}

		~EditorDatabase()
		{
			assert(instance == this);
			instance = NULL;
		}

		bool initialize()
		{
			system("dir");

			return open(Filename)
				;// && Database::Statement(this, Definition).execute();
		}

		// add some error handler

	private:
		constexpr static char Filename[] = "context.db";

		constexpr static char Definition[] = R"_(
BEGIN TRANSACTION;
CREATE TABLE IF NOT EXISTS "world" (
	"id"	INTEGER,
	"name"	TEXT,
	"contextid"	INTEGER,
	PRIMARY KEY("id"),
	FOREIGN KEY("contextid") REFERENCES "context"("id") ON DELETE CASCADE
);
CREATE TABLE IF NOT EXISTS "tile" (
	"id"	INTEGER,
	"worldid"	INTEGER,
	FOREIGN KEY("worldid") REFERENCES "world"("id") ON DELETE CASCADE
);
CREATE TABLE IF NOT EXISTS "transitive" (
	"id"	INTEGER,
	"name"	INTEGER,
	"outputid"	INTEGER,
	"inputid"	INTEGER,
	"contextid"	INTEGER,
	FOREIGN KEY("contextid") REFERENCES "context"("id") ON DELETE CASCADE,
	FOREIGN KEY("inputid") REFERENCES "world"("id") ON DELETE CASCADE,
	FOREIGN KEY("outputid") REFERENCES "world"("id") ON DELETE CASCADE
);
CREATE TABLE IF NOT EXISTS "context" (
	"id"	INTEGER,
	"name"	TEXT,
	"description"	TEXT,
	PRIMARY KEY("id")
);
COMMIT;

)_";
	};
}
