BEGIN TRANSACTION;
CREATE TABLE IF NOT EXISTS "transitive" (
	"id"	INTEGER,
	"name"	INTEGER,
	"outputid"	INTEGER,
	"inputid"	INTEGER,
	"contextid"	INTEGER,
	FOREIGN KEY("outputid") REFERENCES "world"("id"),
	FOREIGN KEY("inputid") REFERENCES "world"("id"),
	FOREIGN KEY("contextid") REFERENCES "context"("id")
);
CREATE TABLE IF NOT EXISTS "world" (
	"id"	INTEGER,
	"name"	TEXT,
	"contextid"	INTEGER,
	FOREIGN KEY("contextid") REFERENCES "context"("id"),
	PRIMARY KEY("id")
);
CREATE TABLE IF NOT EXISTS "tile" (
	"id"	INTEGER,
	"worldid"	INTEGER,
	FOREIGN KEY("worldid") REFERENCES "world"("id")
);
CREATE TABLE IF NOT EXISTS "context" (
	"id"	INTEGER,
	"name"	TEXT,
	"description"	TEXT,
	PRIMARY KEY("id")
);
COMMIT;
