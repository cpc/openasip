TOP_SRCDIR = ../../../..

include ${TOP_SRCDIR}/test/Makefile_configure_settings

DIST_OBJECTS = CostDatabase.o CostDBEntry.o CostDBEntryStats.o \
               CostDBEntryStatsFU.o CostDBEntryStatsRF.o       \
	       EntryKeyField.o CostDBEntryKey.o                \
               EntryKeyProperty.o EntryKeyFieldProperty.o      \
	       FilterSearch.o ExactMatch.o Interpolation.o     \
	       MatchType.o Matcher.o SubSet.o SuperSet.o       \
	       CostDBTypes.o EntryKeyData.o
TOOL_OBJECTS = *.o
#Application.o Exception.o Conversion.o DataObject.o
HDB_OBJECTS = *.o
MACH_OBJECTS = *.o
APPLIBS_MACH_OBJS = *.o
PROG_OBJECTS = *.o
TPEF_OBJECTS = *.o
UMACH_LIB_OBJS = *.o
OSAL_OBJECTS = *.o

EXTRA_COMPILER_LFAGS = 
EXTRA_LINKER_FLAGS = ${TCL_LD_FLAGS} ${BOOST_LDFLAGS} ${XERCES_LDFLAGS} \


include ${TOP_SRCDIR}/test/Makefile_test.defs
