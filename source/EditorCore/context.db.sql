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
