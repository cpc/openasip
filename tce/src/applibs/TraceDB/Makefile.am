lib_LTLIBRARIES = libtracedb.la
libtracedb_la_SOURCES = ExecutionTrace.cc InstructionExecution.cc

SIM_APPLIBS_DIR = ../Simulator

AM_CPPFLAGS = -I${PROJECT_ROOT}/src/tools -I${SIM_APPLIBS_DIR} \
				${SQLITE_INCLUDES} 
libtracedb_la_LDFLAGS = ${SQLITE_LIBDIR} ${SQLITE_LD_FLAGS}
PROJECT_ROOT = ../../..

MAINTAINERCLEANFILES = *~ *.gcov *.bbg *.bb *.da
