PRAGMA foreign_keys=OFF;
BEGIN TRANSACTION;
CREATE TABLE IF NOT EXISTS "threats" (
	"pid"	INTEGER NOT NULL,
	"id"	INTEGER NOT NULL,
	"message"	BLOB NOT NULL,
	"level"	INTEGER NOT NULL
);
COMMIT;
