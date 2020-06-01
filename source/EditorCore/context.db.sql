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
