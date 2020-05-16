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
			return open(Filename)
				&& Database::Statement(this, Definition).execute();
		}

	private:
		constexpr static char Filename[] = "editor.db";

		constexpr static char Definition[] = R"_(
BEGIN TRANSACTION;
CREATE TABLE IF NOT EXISTS "world" (
	"worldID"	INTEGER,
	"name"	TEXT,
	"parent"	INTEGER,
	PRIMARY KEY("worldID"),
	FOREIGN KEY("parent") REFERENCES "context"("id")
);
CREATE TABLE IF NOT EXISTS "context" (
	"id"	INTEGER,
	"name"	TEXT,
	"description"	TEXT,
	PRIMARY KEY("id")
);
CREATE TABLE IF NOT EXISTS "tile" (
	"tileID"	INTEGER,
	"worldID"	INTEGER,
	FOREIGN KEY("worldID") REFERENCES "world"("worldID")
);
CREATE TABLE IF NOT EXISTS "transitive" (
	"entryID"	INTEGER,
	"name"	INTEGER,
	"outputID"	INTEGER,
	"inputID"	INTEGER,
	FOREIGN KEY("outputID") REFERENCES "world"("worldID"),
	FOREIGN KEY("inputID") REFERENCES "world"("worldID")
);
COMMIT;
)_";
	};
}
