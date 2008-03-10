/** 
 * @file RelationalDBTest.hh
 * 
 * A test suite for RelationalDB.
 *
 * @author Pekka J‰‰skel‰inen 2004 (pekka.jaaskelainen@tut.fi)
 * @note rating: red
 */

#ifndef RELATIONAL_DB_TEST_HH
#define RELATIONAL_DB_TEST_HH

#include <string>
using std::string;

#include <TestSuite.h>

#include "SQLite.hh"
#include "RelationalDBQueryResult.hh"
#include "FileSystem.hh"
#include "DataObject.hh"

class RelationalDBTest : public CxxTest::TestSuite {
public:
    RelationalDBTest();
    virtual ~RelationalDBTest();

    void setUp();
    void tearDown();

    void testCreateNewDatabase();
    void testCreateTable();
    void testInsertsWithTransaction();
    void testQuery();
    void testQueryThatReturnsOneRow();
    void testQueryThatReturnsFourRows();
    void testQueryThatReturnsNothing();
    void testIllegalQueries();
    void testNullObject();
    void testDelete();
    void testClose();

private:
    RelationalDB* db_;
    RelationalDBConnection* connection_;
};

const string NONEXISTING_FILE = "data/new.db";
const string NONWRITABLE_FILE = "data";

const string tableQuery = 
"CREATE TABLE movies_seen (id INTEGER PRIMARY KEY, title VARCHAR(512), " \
"grade INTEGER);";


const int POPULATE_DATA_SIZE = 6;
const string movies[] = 
{"Matrix, The", "Rock, The", "Papillon", "Jurassic Park", 
 "Something About Mary", "Fight Club"};

const string populateData[] =
{"INSERT INTO movies_seen VALUES(NULL, '" + movies[0] + "', 9);",
 "INSERT INTO movies_seen VALUES(NULL, '" + movies[1] + "', 8);",
 "INSERT INTO movies_seen VALUES(NULL, '" + movies[2] + "', 8);",
 "INSERT INTO movies_seen VALUES(NULL, '" + movies[3] + "', 6);",
 "INSERT INTO movies_seen VALUES(NULL, '" + movies[4] + "', 7);",
 "INSERT INTO movies_seen VALUES(NULL, '" + movies[5] + "', 9);"};

/**
 * Constructor.
 */
RelationalDBTest::RelationalDBTest() : 
    db_(new SQLite()), connection_(NULL)  {
}

/**
 * Destructor.
 */
RelationalDBTest::~RelationalDBTest() {
}

/**
 * Called before each test.
 */
void
RelationalDBTest::setUp() {
}


/**
 * Called after each test.
 */
void
RelationalDBTest::tearDown() {
}

#include <iostream>
#define PRINT_EXCEPTION(_X_) \
   try { _X_; } catch (const Exception& e) { \
   std::cerr << e.errorMessage() << std::endl; }

/**
 * Tests connecting to a new SQLite database.
 *
 */
void 
RelationalDBTest::testCreateNewDatabase() {
    
    // try opening a database file that cannot be opened
    TS_ASSERT_THROWS(db_->connect(NONWRITABLE_FILE), RelationalDBException);

    TS_ASSERT_THROWS_NOTHING(connection_ = &db_->connect(NONEXISTING_FILE));
    
    // try reconnecting (should close the old connection automatically)
    TS_ASSERT_THROWS_NOTHING(connection_ = &db_->connect(NONEXISTING_FILE));
    TS_ASSERT(FileSystem::fileExists(NONEXISTING_FILE)); 
}

/**
 * Tests creating a new table to the new database.
 */
void 
RelationalDBTest::testCreateTable() {
    TS_ASSERT_THROWS_NOTHING(connection_->DDLQuery(tableQuery));
}

/**
 * Tests INSERTing data into the new table using a transaction.
 */
void 
RelationalDBTest::testInsertsWithTransaction() {
    
    TS_ASSERT_THROWS_NOTHING(connection_->beginTransaction());

    for (int i = 0; i < POPULATE_DATA_SIZE;  ++i) {
        int updateCount = 0;
        TS_ASSERT_THROWS_NOTHING(
            updateCount = connection_->updateQuery(populateData[i]));
        TS_ASSERT_EQUALS(updateCount, 1);
    }

    TS_ASSERT_THROWS_NOTHING(connection_->commit());
}

/**
 * Tests a successfull SELECT query.
 * 
 * Also tests asking the column names, column count and row count.
 */
void 
RelationalDBTest::testQuery() {
    RelationalDBQueryResult* result = NULL;
    TS_ASSERT_THROWS_NOTHING(
        result = connection_->query("SELECT * FROM movies_seen;"));

    TS_ASSERT_EQUALS(result->columns(), 3);

    TS_ASSERT_EQUALS(result->columnName(0), "id");
    TS_ASSERT_EQUALS(result->columnName(1), "title");
    TS_ASSERT_EQUALS(result->columnName(2), "grade");
    TS_ASSERT_EQUALS(result->columnName(3), "");

    int resultCount = 0;
    while (result->hasNext()) {
        result->next();
        TS_ASSERT_EQUALS(result->data(1).stringValue(), movies[resultCount]);
        ++resultCount;
    }

    TS_ASSERT_EQUALS(resultCount, 6);
    delete result;
    result = NULL;
}

/**
 * Tests a SELECT query that returns only one row.
 */
void
RelationalDBTest::testQueryThatReturnsOneRow() {

    RelationalDBQueryResult* result = NULL;

    TS_ASSERT_THROWS_NOTHING(
        result = connection_->query(
            "SELECT * FROM movies_seen WHERE id = 1;"));

    TS_ASSERT_EQUALS(result->columnName(0), "id");
    TS_ASSERT_EQUALS(result->columnName(1), "title");
    TS_ASSERT_EQUALS(result->columnName(2), "grade");
    TS_ASSERT_EQUALS(result->columnName(3), "");
    TS_ASSERT_EQUALS(result->hasNext(), true);

    int resultCount = 0;
    while (result->hasNext()) {
        result->next();
        TS_ASSERT_EQUALS(result->data(1).stringValue(), movies[0]);
        ++resultCount;
    }

    TS_ASSERT_EQUALS(result->hasNext(), false);
    TS_ASSERT_EQUALS(resultCount, 1);
    delete result;
    result = NULL;
}

/**
 * Tests a SELECT query that returns two rows.
 */
void
RelationalDBTest::testQueryThatReturnsFourRows() {

    RelationalDBQueryResult* result = NULL;

    TS_ASSERT_THROWS_NOTHING(
        result = connection_->query(
            "SELECT * FROM movies_seen " \
            "WHERE id = 1 OR id = 2 OR id = 3 OR id = 4;"));

    TS_ASSERT_EQUALS(result->columnName(0), "id");
    TS_ASSERT_EQUALS(result->columnName(1), "title");
    TS_ASSERT_EQUALS(result->columnName(2), "grade");
    TS_ASSERT_EQUALS(result->columnName(3), "");
    TS_ASSERT_EQUALS(result->hasNext(), true);

    int resultCount = 0;

    // hasNext() is broken! 
    while (result->hasNext()) {
        result->next();
        TS_ASSERT_EQUALS(result->data(1).stringValue(), movies[resultCount]);
        ++resultCount;
    }

    TS_ASSERT_EQUALS(result->hasNext(), false);
    TS_ASSERT_EQUALS(resultCount, 4);
    delete result;
    result = NULL;
}


/**
 * Tests a SELECT query that returns no rows.
 */
void
RelationalDBTest::testQueryThatReturnsNothing() {

    RelationalDBQueryResult* result = NULL;

    // test a query that returns no rows
    TS_ASSERT_THROWS_NOTHING(
        result = connection_->query(
            "SELECT * FROM movies_seen WHERE title LIKE('foobar');"));

    TS_ASSERT_EQUALS(result->columnName(0), "id");
    TS_ASSERT_EQUALS(result->columnName(1), "title");
    TS_ASSERT_EQUALS(result->columnName(2), "grade");
    TS_ASSERT_EQUALS(result->columnName(3), "");
    TS_ASSERT_EQUALS(result->hasNext(), false);

    int resultCount = 0;
    while (result->next()) {
        ++resultCount;
    }

    // index out of bounds should return NullDataObject instance
    TS_ASSERT_EQUALS(&result->data(10), &NullDataObject::instance());
    
    TS_ASSERT_EQUALS(resultCount, 0);
    delete result;
    result = NULL;
}

/**
 * Tests illegal SELECT queries.
 */
void
RelationalDBTest::testIllegalQueries() {

    RelationalDBQueryResult* result = NULL;

    // test illegal queries
    TS_ASSERT_THROWS(
        result = connection_->query(
            "SELECT * FROMX movies_seen WHERE title LIKE('foobar');"),
        RelationalDBException);

    TS_ASSERT_THROWS(
        result = connection_->query("SELECT * FROM Emovies_seen;"),
        RelationalDBException);
}

/**
 * Tests that NullDataObject is returned in case of NULL value.
 */
void
RelationalDBTest::testNullObject() {

    RelationalDBQueryResult* result = NULL;


    connection_->updateQuery("INSERT INTO movies_seen VALUES(NULL, NULL, 1);");

    // test illegal queries
    result = connection_->query(
        "SELECT * FROM movies_seen WHERE grade = 1;");
    TS_ASSERT(result->hasNext());
    TS_ASSERT(!result->next());
    TS_ASSERT(!result->hasNext());
    TS_ASSERT_EQUALS(result->columns(), 3);
    TS_ASSERT_EQUALS(result->data(2).integerValue(), 1);
    TS_ASSERT(result->data(1).isNull());
    delete result;
    result = NULL;

}


/**
 * Tests DELETE data from the table.
 */
void 
RelationalDBTest::testDelete() {
    TS_ASSERT_THROWS_NOTHING(connection_->beginTransaction());
    TS_ASSERT_THROWS_NOTHING(
        connection_->updateQuery("DELETE FROM movies_seen;"));
    TS_ASSERT_THROWS_NOTHING(connection_->rollback());
    TS_ASSERT_THROWS_NOTHING(connection_->beginTransaction());
    TS_ASSERT_THROWS_NOTHING(
        connection_->updateQuery("DELETE FROM movies_seen;"));
    TS_ASSERT_THROWS_NOTHING(connection_->commit());
    
}

/**
 * Tests closing the database connection.
 */
void 
RelationalDBTest::testClose() {
    TS_ASSERT_THROWS_NOTHING(db_->close(*connection_));

    // multiple close() invocations should do no harm
    TS_ASSERT_THROWS_NOTHING(db_->close(*connection_));

    delete db_;
}


#endif
