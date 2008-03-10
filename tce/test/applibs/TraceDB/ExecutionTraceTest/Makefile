TOP_SRCDIR = ../../../..

include ${TOP_SRCDIR}/test/Makefile_configure_settings 

DIST_OBJECTS = ExecutionTrace.o InstructionExecution.o
TOOL_OBJECTS = Exception.o SQLite.o RelationalDB.o SQLiteConnection.o \
		RelationalDBConnection.o SQLiteQueryResult.o \
		RelationalDBQueryResult.o Application.o DataObject.o \
		Conversion.o StringTools.o SimValue.o

EXTRA_LINKER_FLAGS = ${SQLITE_LD_FLAGS}

INITIALIZATION = cleanup

include ${TOP_SRCDIR}/test/Makefile_test.defs

cleanup:
	@mkdir -p data
	@rm -f data/new.tdb
