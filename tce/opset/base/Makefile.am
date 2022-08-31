basedir = ${prefix}/share/tce/opset/base
avalondir = ${prefix}/share/tce/opset/base
simple_iodir = ${prefix}/share/tce/opset/base
doubledir = ${prefix}/share/tce/opset/base
implicit_load_storedir = ${prefix}/share/tce/opset/base
nobase_base_DATA = base.cc base.opp
nobase_avalon_DATA = avalon.cc avalon.opp
nobase_simple_io_DATA = simple_io.cc simple_io.opp
nobase_double_DATA = double.cc double.opp
nobase_implicit_load_store_DATA = implicit_load_store.cc implicit_load_store.opp

pkglibdir = ${prefix}/share/tce/opset/base
pkglib_LTLIBRARIES = base.la avalon.la simple_io.la double.la implicit_load_store.la

base_la_SOURCES = base.cc
avalon_la_SOURCES = avalon.cc
simple_io_la_SOURCES = simple_io.cc
double_la_SOURCES = double.cc
implicit_load_store_la_SOURCES = implicit_load_store.cc

# In case libopenasip.so is loaded using RTLD_LOCAL (e.g. by an icd
# loader), the dlopen that opens the .opb does not get the missing 
# libopenasip symbols. Add libopenasip as a dependency so they can be loaded
# when the plugins are loaded.
base_la_LIBADD = ../../src/libopenasip.la
avalon_la_LIBADD = ../../src/libopenasip.la
simple_io_la_LIBADD = ../../src/libopenasip.la
double_la_LIBADD = ../../src/libopenasip.la
implicit_load_store_la_LIBADD = ../../src/libopenasip.la

EXTRA_DIST = ${nobase_base_DATA} ${nobase_avalon_DATA} ${nobase_simple_io_DATA} ${nobase_double_DATA} \
             ${nobase_implicit_load_store_DATA}

PROJECT_ROOT = $(top_srcdir)
SRC_ROOT_DIR = ${PROJECT_ROOT}/src

TPEF_DIR = ${SRC_ROOT_DIR}/base/tpef
MACH_DIR = ${SRC_ROOT_DIR}/base/mach
OSAL_DIR = ${SRC_ROOT_DIR}/base/osal
IDF_DIR = ${SRC_ROOT_DIR}/base/idf
UMACH_DIR = ${SRC_ROOT_DIR}/base/umach
PROG_DIR = ${SRC_ROOT_DIR}/base/program
BEM_DIR = ${SRC_ROOT_DIR}/base/bem
BEM_APPLIBS_DIR = ${SRC_ROOT_DIR}/applibs/bem
PIG_APPLIBS_DIR = ${SRC_ROOT_DIR}/applibs/PIG
MACH_APPLIBS_DIR = ${SRC_ROOT_DIR}/applibs/mach
HDB_DIR = ${SRC_ROOT_DIR}/applibs/hdb
TRACEDB_DIR = ${SRC_ROOT_DIR}/applibs/TraceDB
ESTIMATOR_DIR = ${SRC_ROOT_DIR}/applibs/Estimator
EXPLORER_DIR = ${SRC_ROOT_DIR}/applibs/Explorer
DSDB_DIR = ${SRC_ROOT_DIR}/applibs/dsdb
SIM_DIR = ${SRC_ROOT_DIR}/applibs/Simulator
TOOLS_DIR = ${SRC_ROOT_DIR}/tools
INTERP_DIR = ${SRC_ROOT_DIR}/applibs/Interpreter
COSTDB_DIR = ${SRC_ROOT_DIR}/applibs/costdb
LLVMBACKEND_DIR = ${SRC_ROOT_DIR}/applibs/LLVMBackend
SCHEDULER_DIR = ${SRC_ROOT_DIR}/applibs/Scheduler
ALGO_DIR = ${SRC_ROOT_DIR}/applibs/Scheduler/Algorithms
APPLIBS_PROG = ${SRC_ROOT_DIR}/applibs/program
PROGE_DIR = ${SRC_ROOT_DIR}/applibs/ProGe
RM_DIR = ${SRC_ROOT_DIR}/applibs/Scheduler/ResourceManager
GRAPH_DIR = ${SRC_ROOT_DIR}/base/Graph
PDG_DIR = ${SRC_ROOT_DIR}/applibs/Scheduler/ProgramRepresentations/ProgramDependenceGraph
DISASM_DIR = ${SRC_ROOT_DIR}/applibs/Disassembler
CFG_DIR = ${SCHEDULER_DIR}/ProgramRepresentations/ControlFlowGraph
DDG_DIR = ${SCHEDULER_DIR}/ProgramRepresentations/DataDependenceGraph
SELECTOR_DIR = ${SCHEDULER_DIR}/Selector
MEM_DIR = ${SRC_ROOT_DIR}/base/memory

AM_CPPFLAGS = \
        -I${MACH_DIR} -I${UMACH_DIR} -I${OSAL_DIR} -I${TPEF_DIR} -I${BEM_DIR} \
		-I${BEM_APPLIBS_DIR} -I${PIG_APPLIBS_DIR} -I${ESTIMATOR_DIR} \
        -I${TOOLS_DIR} -I${PROG_DIR} -I${MACH_APPLIBS_DIR} -I${IDF_DIR} \
		-I${HDB_DIR} -I${TRACEDB_DIR} -I${SIM_DIR} -I${COSTDB_DIR} \
		-I${EXPLORER_DIR} -I${DSDB_DIR} -I${INTERP_DIR} \
		-I${LLVMBACKEND_DIR} -I${SCHEDULER_DIR} -I${APPLIBS_PROG} \
		-I${PROGE_DIR} -I${ALGO_DIR} -I${RM_DIR} -I${PDG_DIR} \
		-I${GRAPH_DIR} -I${DISASM_DIR} -I${DDG_DIR} -I${CFG_DIR} \
		-I${SELECTOR_DIR} -I${MEM_DIR}

AM_CPPFLAGS += -I${PROJECT_ROOT} # Needed for config.h

AM_LDFLAGS = ${BOOST_LIBDIR} ${SQLITE_LIBDIR} ${TCL_LIBDIR} ${TCL_LD_FLAGS} ${XERCES_LIBDIR} ${XERCES_LIBS} \
	${EDITLINE_LD_FLAGS} -module -version-info ${LIB_VERSION}

all-local: ${pkglib_LTLIBRARIES}
	ln -sf ./.libs/base.so ./base.opb
	ln -sf ./.libs/avalon.so ./avalon.opb
	ln -sf ./.libs/simple_io.so ./simple_io.opb
	ln -sf ./.libs/double.so ./double.opb
	ln -sf ./.libs/implicit_load_store.so ./implicit_load_store.opb
# Ugly hack to ensure we have a proper 'base' OSAL module (opp,opb) in the
# build dir so a) newlib builds OK b) in case the user does not remove the
# installation dir, simulation still works.
	if test "${srcdir}" != "."; then  ln -sf ${srcdir}/base.opp base.opp; fi;

install-exec-hook:
	cd $(DESTDIR)/${pkglibdir} ; ln -sf base.so base.opb ; \
ln -sf avalon.so avalon.opb ; ln -sf simple_io.so simple_io.opb ; \
ln -sf double.so double.opb ; ln -sf implicit_load_store.so implicit_load_store.opb

clean-local:
	rm -f ./base.opb
	rm -f ./avalon.opb
	rm -f ./simple_io.opb
	rm -f ./double.opb
	rm -f ./implicit_load_store.opb


