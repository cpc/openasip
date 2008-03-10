DIST_OBJECTS = RelationalDBConnection.o SQLite.o \
               SQLiteConnection.o RelationalDBQueryResult.o \
	       SQLiteQueryResult.o RelationalDB.o
TOOL_OBJECTS = FileSystem.o Exception.o Application.o Environment.o \
               DataObject.o Conversion.o StringTools.o
TOP_SRCDIR = ../../..

INITIALIZATION = initialize_test_data

include ${TOP_SRCDIR}/test/Makefile_configure_settings

EXTRA_LINKER_FLAGS = ${SQLITE_LD_LFAGS} ${BOOST_LDFLAGS}

include ${TOP_SRCDIR}/test/Makefile_test.defs

initialize_test_data:
	@mkdir -p data
	@rm -f data/new.db
