noinst_LTLIBRARIES = libtcetools.la
BUILT_SOURCES = tce_version_string.h
libtcetools_la_SOURCES = Exception.cc CmdLineOptions.cc CmdLineOptionParser.cc \
	Environment.cc Application.cc XMLSerializer.cc ObjectState.cc \
	FileSystem.cc DOMBuilderErrorHandler.cc TextGenerator.cc \
	PluginTools.cc Conversion.cc StringTools.cc DataObject.cc SimValue.cc \
	ConfigurationFile.cc ProcessorConfigurationFile.cc Listener.cc \
	Informer.cc Options.cc OptionValue.cc CmdLineParser.cc MathTools.cc \
	BitMatrix.cc TCEString.cc ${BUILT_SOURCES}

if HAVE_SQLITE
  libtcetools_la_SOURCES += SQLiteConnection.cc RelationalDBQueryResult.cc \
	SQLiteQueryResult.cc SQLite.cc RelationalDBConnection.cc RelationalDB.cc
  libtcetools_la_LDFLAGS = ${SQLITE_LD_FLAGS} ${BOOST_LDFLAGS}
endif

PROJECT_ROOT = $(top_srcdir)
DOXYGEN_CONFIG_FILE = ${PROJECT_ROOT}/tools/Doxygen/doxygen.config
AM_CPPFLAGS = -I${PROJECT_ROOT} -I${PROJECT_ROOT}/src/base/memory

include_HEADERS = Application.hh ObjectState.hh Exception.hh Exception.icc BaseType.hh \
	SimValue.hh SimValue.icc Serializable.hh ObjectState.icc Conversion.hh \
	Conversion.icc MathTools.hh MathTools.icc TCEString.hh

REVNO=$(shell which bzr > /dev/null && bzr revno > /dev/null 2>&1 && echo "-r" | tr -d "\n" && bzr revno 2> /dev/null || echo -n "")
TCE_VERSION_STRING=@PACKAGE_VERSION@$(REVNO)

tce_version_string.h: ../../../.bzr/branch/last-revision
	@echo "Setting TCE_VERSION_STRING to $(TCE_VERSION_STRING)"
	@echo '#define TCE_VERSION_STRING "$(TCE_VERSION_STRING)"' > $@

# A hack, we'll see if something breaks.
../../../.bzr/branch/last-revision:
	@echo "Not compiling in a bazaar branch"


## headers start
libtcetools_la_SOURCES += \
	Application.hh AssocTools.hh \
	BaseType.hh BitMatrix.hh \
	CmdLineOptionParser.hh CmdLineOptions.hh \
	CmdLineParser.hh ConfigurationFile.hh \
	ContainerTools.hh Conversion.hh \
	DBTypes.hh DOMBuilderErrorHandler.hh \
	DataObject.hh Environment.hh \
	Exception.hh FileSystem.hh \
	Informer.hh Listener.hh \
	MapTools.hh MathTools.hh \
	ObjectState.hh OptionValue.hh \
	Options.hh PagedArray.hh \
	PluginTools.hh ProcessorConfigurationFile.hh \
	RelationalDB.hh RelationalDBConnection.hh \
	RelationalDBQueryResult.hh SQLite.hh \
	SQLiteConnection.hh SQLiteQueryResult.hh \
	SequenceTools.hh Serializable.hh \
	Serializer.hh SetTools.hh \
	SimValue.hh StringTools.hh \
	TCEString.hh TextGenerator.hh \
	VectorTools.hh XMLSerializer.hh \
	hash_map.hh hash_set.hh \
	AssocTools.icc BitMatrix.icc \
	CmdLineOptionParser.icc CmdLineParser.icc \
	ContainerTools.icc Conversion.icc \
	DataObject.icc Exception.icc \
	FileSystem.icc Informer.icc \
	MapTools.icc MathTools.icc \
	ObjectState.icc PagedArray.icc \
	PluginTools.icc SequenceTools.icc \
	SetTools.icc SimValue.icc \
	VectorTools.icc 
## headers end
