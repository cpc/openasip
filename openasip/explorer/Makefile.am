pkglibdir = ${prefix}/share/openasip/explorer/base
pkglib_LTLIBRARIES = \
	FrequencySweepExplorer.la SimpleICOptimizer.la \
	RemoveUnconnectedComponents.la MinimizeMachine.la \
	GrowMachine.la ImmediateGenerator.la \
	ImplementationSelector.la ComponentAdder.la \
	Evaluate.la MinimalOpSet.la ConnectionSweeper.la \
	ADFCombiner.la VectorLSGenerator.la VLIWConnectIC.la \
	BusMergeMinimizer.la RFPortMergeMinimizer.la \
	AllOperationMachine.la \
	PruneUnusedUnits.la FUMergeMinimizer.la ShrinkRegisterCounts.la \
	AutoExplorer.la ShortImmediateOptimizer.la \
	BlocksConnectIC.la

# Libtool seems to realize we are compiling from .cc instead of .c
# just by defining the first source. Without this it fails.
FrequencySweepExplorer_la_SOURCES = FrequencySweepExplorer.cc
SimpleICOptimizer_la_SOURCES = SimpleICOptimizer.cc
RemoveUnconnectedComponents_la_SOURCES = RemoveUnconnectedComponents.cc
MinimizeMachine_la_SOURCES = MinimizeMachine.cc
GrowMachine_la_SOURCES = GrowMachine.cc
ImmediateGenerator_la_SOURCES = ImmediateGenerator.cc
ImplementationSelector_la_SOURCES = ImplementationSelector.cc
ComponentAdder_la_SOURCES = ComponentAdder.cc
Evaluate_la_SOURCES = Evaluate.cc
MinimalOpSet_la_SOURCES = MinimalOpSet.cc
ConnectionSweeper_la_SOURCES = ConnectionSweeper.cc
ADFCombiner_la_SOURCES = ADFCombiner.cc
VectorLSGenerator_la_SOURCES = VectorLSGenerator.cc
VLIWConnectIC_la_SOURCES = VLIWConnectIC.cc
BusMergeMinimizer_la_SOURCES = BusMergeMinimizer.cc
RFPortMergeMinimizer_la_SOURCES = RFPortMergeMinimizer.cc
AllOperationMachine_la_SOURCES = AllOperationMachine.cc
PruneUnusedUnits_la_SOURCES = PruneUnusedUnits.cc
FUMergeMinimizer_la_SOURCES = FUMergeMinimizer.cc
ShrinkRegisterCounts_la_SOURCES = ShrinkRegisterCounts.cc
AutoExplorer_la_SOURCES = AutoExplorer.cc
ShortImmediateOptimizer_la_SOURCES = ShortImmediateOptimizer.cc
BlocksConnectIC_la_SOURCES = BlocksConnectIC.cc

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
APPLIBS_PROG = ${SRC_ROOT_DIR}/applibs/program
APPLIBS_OSAL_DIR = ${SRC_ROOT_DIR}/applibs/osal
GRAPH_DIR = ${SRC_ROOT_DIR}/base/Graph

AM_CPPFLAGS = \
        -I${MACH_DIR} -I${UMACH_DIR} -I${OSAL_DIR} -I${TPEF_DIR} -I${BEM_DIR} \
		-I${BEM_APPLIBS_DIR} -I${PIG_APPLIBS_DIR} -I${ESTIMATOR_DIR} \
        -I${TOOLS_DIR} -I${PROG_DIR} -I${MACH_APPLIBS_DIR} -I${IDF_DIR} \
		-I${HDB_DIR} -I${TRACEDB_DIR} -I${SIM_DIR} -I${COSTDB_DIR} \
		-I${EXPLORER_DIR} -I${DSDB_DIR} -I${INTERP_DIR} \
		-I${LLVMBACKEND_DIR} -I${SCHEDULER_DIR} -I${APPLIBS_PROG} \
		-I${APPLIBS_OSAL_DIR} -I${GRAPH_DIR}

AM_CPPFLAGS += -I${PROJECT_ROOT} # Needed for config.h

AM_CPPFLAGS += -g

AM_LDFLAGS = ${BOOST_LIBDIR} ${SQLITE_LIBDIR} ${TCL_LIBDIR} ${TCL_LD_FLAGS} ${XERCES_LIBDIR} ${XERCES_LIBS} \
	${EDITLINE_LD_FLAGS} -module -version-info ${LIB_VERSION}

all-local: ${pkglib_LTLIBRARIES}
# Create links to the .so to the current directory just to make 
# running Explorer from the source tree to find them.
	@@(for soname in `ls -1 ./.libs/*.so`; do \
		ln -sf $$soname .; \
	done)

clean-local:
	rm -f ./*.so
