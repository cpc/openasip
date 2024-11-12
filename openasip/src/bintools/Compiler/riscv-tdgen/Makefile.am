bin_PROGRAMS = riscv-tdgen
riscv_tdgen_SOURCES = riscv-tdgen.cc

LIB_BASE_DIR = ../../../base
LIB_TCETOOLS_DIR = ../../../tools
LIB_APPLIBS_MACH_DIR = ../../../applibs/mach

PROJECT_ROOT = $(top_srcdir)
TOOLS_DIR = ${SRC_ROOT}/tools
SRC_ROOT = ${PROJECT_ROOT}/src
MACH_DIR = ${SRC_ROOT}/base/mach
PROG_DIR = ${SRC_ROOT}/base/program
TPEF_DIR  = ${SRC_ROOT}/base/tpef
OSAL_DIR = ${SRC_ROOT}/base/osal
GRAPH_DIR = ${SRC_ROOT}/base/Graph
LLVMBACKEND_DIR = ${SRC_ROOT}/applibs/LLVMBackend
OSAL_APPLIBS_DIR = ${SRC_ROOT}/applibs/osal
MACH_APPLIBS_DIR = ${SRC_ROOT}/applibs/mach
PROG_APPLIBS_DIR = ${SRC_ROOT}/applibs/program
UMACH_DIR = ${SRC_ROOT}/base/umach
DISASM_DIR = ${SRC_ROOT}/applibs/Disassembler
SCHED_ALGO_DIR = ${SRC_ROOT}/applibs/Scheduler/Algorithms
SCHED_DIR = ${SRC_ROOT}/applibs/Scheduler
RM_DIR = ${SCHED_DIR}/ResourceManager
SELECTOR_DIR = ${SCHED_DIR}/Selector
APPLIBS_DIR = ${SRC_ROOT}/applibs
PDG_DIR = ${APPLIBS_DIR}/Scheduler/ProgramRepresentations/PDG
CFG_DIR = ${APPLIBS_DIR}/Scheduler/ProgramRepresentations/CFG
DDG_DIR = ${APPLIBS_DIR}/Scheduler/ProgramRepresentations/DDG
ASSEMBLER_DIR = ${APPLIBS_DIR}/Assembler
BEM_SRC_DIR = ${SRC_ROOT}/base/bem
BEM_APPLIBS_DIR = ${APPLIBS_DIR}/bem


AM_CPPFLAGS = -I${TOOLS_DIR} -I${LLVMBACKEND_DIR} -I${MACH_DIR} \
    -I${PROG_DIR} -I${TPEF_DIR} -I${OSAL_DIR} -I${DISASM_DIR} \
    -I${UMACH_DIR} -I${OSAL_APPLIBS_DIR} -I${GRAPH_DIR} \
    -I${MACH_APPLIBS_DIR} -I${SCHED_ALGO_DIR} -I${SCHED_DIR} ${LLVM_CPPFLAGS} \
	-I${PDG_DIR} -I${CFG_DIR} -I${DDG_DIR} -I${SELECTOR_DIR} -I${RM_DIR} \
	-I${SCHED_ALGO_DIR} -I${PROG_APPLIBS_DIR} -I${ASSEMBLER_DIR} \
	-I${BEM_SRC_DIR} -I${BEM_APPLIBS_DIR} 


riscv_tdgen_LDADD = ../../../libopenasip.la

AM_LDFLAGS = ${TCE_LDFLAGS}

dist-hook:
	rm -rf $(distdir)/CVS $(distdir)/.deps $(distdir)/Makefile

MAINTAINERCLEANFILES = *~ *.gcov *.bbg *.bb *.da
