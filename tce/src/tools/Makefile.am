
lib_LTLIBRARIES = libtcetools.la
libtcetools_la_SOURCES = Exception.cc CmdLineOptions.cc CmdLineOptionParser.cc \
	Environment.cc Application.cc XMLSerializer.cc ObjectState.cc \
	FileSystem.cc DOMBuilderErrorHandler.cc TextGenerator.cc \
	PluginTools.cc Conversion.cc StringTools.cc DataObject.cc SimValue.cc \
	ConfigurationFile.cc ProcessorConfigurationFile.cc Listener.cc \
	Informer.cc Options.cc OptionValue.cc CmdLineParser.cc MathTools.cc \
	BitMatrix.cc TCEString.cc 

BUILT_SOURCES = tce_version_string.h

if HAVE_SQLITE
  libtcetools_la_SOURCES += SQLiteConnection.cc RelationalDBQueryResult.cc \
	SQLiteQueryResult.cc SQLite.cc RelationalDBConnection.cc RelationalDB.cc
  libtcetools_la_LDFLAGS = ${SQLITE_LD_FLAGS} ${BOOST_LDFLAGS}
endif

PROJECT_ROOT = $(top_srcdir)
DOXYGEN_CONFIG_FILE = ${PROJECT_ROOT}/tools/Doxygen/doxygen.config
AM_CPPFLAGS = -I${PROJECT_ROOT} -I${PROJECT_ROOT}/src/base/memory
AM_CXXFLAGS = -UNDEBUG

include_HEADERS = Application.hh ObjectState.hh Exception.hh Exception.icc BaseType.hh \
	SimValue.hh SimValue.icc Serializable.hh ObjectState.icc Conversion.hh \
	Conversion.icc MathTools.hh MathTools.icc TCEString.hh

dist-hook:
	rm -rf $(distdir)/CVS $(distdir)/.deps $(distdir)/Makefile

.PHONY: clean doc 

doc:
	doxygen ${DOXYGEN_CONFIG_FILE}

MAINTAINERCLEANFILES = *~ *.gcov *.bbg *.bb *.da

REVNO=$(shell which bzr > /dev/null && bzr revno || echo "Unknown")
TCE_VERSION_STRING=@PACKAGE_VERSION@-r$(REVNO)

tce_version_string.h: ../../../.bzr/branch/last-revision
	@echo "Setting TCE_VERSION_STRING to $(TCE_VERSION_STRING)"
	@echo '#define TCE_VERSION_STRING "$(TCE_VERSION_STRING)"' > $@

# A hack, we'll see if something breaks.
../../../.bzr/branch/last-revision:
	@echo "Not compiling in a bazaar branch"
