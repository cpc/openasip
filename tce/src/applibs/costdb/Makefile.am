lib_LTLIBRARIES = libcostdb.la

libcostdb_la_SOURCES = CostDatabase.cc \
	CostDBEntry.cc CostDBEntryKey.cc \
	CostDBEntryStats.cc CostDBEntryStatsFU.cc CostDBEntryStatsRF.cc \
	CostDBTypes.cc \
	EntryKeyData.cc EntryKeyField.cc \
	EntryKeyFieldProperty.cc EntryKeyProperty.cc \
	FilterSearch.cc Matcher.cc MatchType.cc \
	ExactMatch.cc Interpolation.cc SubSet.cc SuperSet.cc \
	CostDatabaseRegistry.cc

PROJECT_ROOT = $(top_srcdir)
DOXYGEN_CONFIG_FILE = ${PROJECT_ROOT}/tools/Doxygen/doxygen.config

AM_CPPFLAGS = -I${PROJECT_ROOT}/src/tools \
	      -I${PROJECT_ROOT}/src/applibs/hdb \
	      -I${PROJECT_ROOT}/src/base/mach
AM_CXXFLAGS = -UNDEBUG

dist-hook:
	rm -rf $(distdir)/CVS $(distdir)/.deps $(distdir)/Makefile

.PHONY: clean doc

MAINTAINERCLEANFILES = *~ *.gcov *.bbg *.bb *.da

