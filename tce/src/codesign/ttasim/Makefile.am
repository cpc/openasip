PROJECT_ROOT = $(top_srcdir)
SRC_ROOT_DIR = ${PROJECT_ROOT}/src

TOOLS_DIR = ${SRC_ROOT_DIR}/tools
OSAL_DIR = ${SRC_ROOT_DIR}/base/osal
SIM_APPLIB_DIR = ${SRC_ROOT_DIR}/applibs/Simulator
INT_APPLIB_DIR = ${SRC_ROOT_DIR}/applibs/Interpreter
BASE_DIR = ${SRC_ROOT_DIR}/base
MACH_DIR = ${SRC_ROOT_DIR}/base/mach
PROGRAM_DIR = ${SRC_ROOT_DIR}/base/program
APPLIBS_HDB_DIR = ${SRC_ROOT_DIR}/applibs/hdb
APPLIBS_MACH_DIR = ${SRC_ROOT_DIR}/applibs/mach
APPLIBS_SCHED_DIR = ${SRC_ROOT_DIR}/applibs/Scheduler
DISASM_DIR = ${SRC_ROOT_DIR}/applibs/Disassembler

LIB_TCETOOLS_DIR = ../../tools
LIB_BASE_DIR = ../../base
LIB_PROGRAM_DIR = ../../base/program
LIB_SIM_APPLIB_DIR = ../../applibs/Simulator
UM_DIR = ../../base/umach
LIB_INT_APPLIB_DIR = ../../applibs/Interpreter

TRACEDB_DIR = ../../applibs/TraceDB
PROG_APPLIB_DIR = ../../applibs/program
LIB_APPLIBS_HDB_DIR = ../../applibs/hdb
LIB_APPLIBS_MACH_DIR = ../../applibs/mach
LIB_APPLIBS_SCHED_DIR = ../../applibs/Scheduler
FSA_APPLIB_DIR = ../../applibs/FSA

bin_PROGRAMS = ttasim ttasim-tandem

ttasim_SOURCES = TTASim.cc SimulatorCmdLineOptions.cc
ttasim_LDADD = ../../libtce.la 

ttasim_tandem_SOURCES = TTASimTandem.cc 
ttasim_tandem_LDADD = ../../libtce.la 

AM_CPPFLAGS = -I${TOOLS_DIR} -I${OSAL_DIR} \
	-I${SIM_APPLIB_DIR} -I${INT_APPLIB_DIR} -I${BASE_DIR} \
	-I${MACH_DIR} -I$(PROGRAM_DIR) -I${APPLIBS_HDB_DIR} \
	-I${APPLIBS_MACH_DIR} -I${APPLIBS_SCHED_DIR} -I${DISASM_DIR}
AM_LDFLAGS = ${TCE_LDFLAGS}

dist-hook:
	rm -rf $(distdir)/CVS $(distdir)/.deps $(distdir)/Makefile

MAINTAINERCLEANFILES = *~ *.gcov *.bbg *.bb *.da


## headers start
ttasim_SOURCES += \
	SimulatorCmdLineOptions.hh 
## headers end
