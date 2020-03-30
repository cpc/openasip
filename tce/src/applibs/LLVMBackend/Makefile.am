AUTOMAKE_OPTIONS = subdir-objects

SUBDIRS = passes

noinst_LTLIBRARIES = libtcellvmbackend.la 
pkglib_LTLIBRARIES = LLVMTCEPOMBuilder.la

libtcellvmbackend_la_SOURCES = \
    TCETargetMachine.cc TCEMCAsmInfo.cc  LLVMPOMBuilder.cc \
	LLVMTCEBuilder.cc ProgramPartitioner.cc InstructionPatternAnalyzer.cc \
	LLVMBackend.cc LowerMissingInstructions.cc TDGen.cc \
	passes/LinkBitcode.cc passes/MachineDCE.cc passes/LowerIntrinsics.cc \
	LLVMTCECmdLineOptions.cc LLVMTCEIRBuilder.cc \
	MachineInstrDDG.cc ConstantTransformer.cc \
	TCEStubTargetMachine.cc

libtcellvmbackend_la_SOURCES += TCEStubTargetTransformInfo.cc \
	TCEStubSubTarget.cc TCEStubTargetMachine.hh

LLVMTCEPOMBuilder_la_SOURCES = LLVMTCEPOMBuilder.cc LLVMTCEPOMBuilder.hh
LLVMTCEPOMBuilder_la_LDFLAGS = -module

PROJECT_ROOT = $(top_srcdir)
TOOLS_DIR = ${SRC_ROOT}/tools
SRC_ROOT = ${PROJECT_ROOT}/src
MACH_DIR = ${SRC_ROOT}/base/mach
PROG_DIR = ${SRC_ROOT}/base/program
TPEF_DIR  = ${SRC_ROOT}/base/tpef
OSAL_DIR = ${SRC_ROOT}/base/osal
GRAPH_DIR = ${SRC_ROOT}/base/Graph
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

AM_CPPFLAGS = -I${TOOLS_DIR} -I${MACH_DIR} \
    -I${PROG_DIR} -I${TPEF_DIR} -I${OSAL_DIR} -I${DISASM_DIR} \
    -I${UMACH_DIR} -I${OSAL_APPLIBS_DIR} -I${GRAPH_DIR} \
    -I${MACH_APPLIBS_DIR} -I${SCHED_ALGO_DIR} -I${SCHED_DIR} ${LLVM_CPPFLAGS} \
	-I${PDG_DIR} -I${CFG_DIR} -I${DDG_DIR} -I${SELECTOR_DIR} -I${RM_DIR} \
	-I${SCHED_ALGO_DIR} -I${PROG_APPLIBS_DIR}

AM_CPPFLAGS += -I${PROJECT_ROOT} # Needed for config.h

# AM_CXXFLAGS is put before CXXFLAGS in compile command and -Wall overrides
# -Wno-unused-parameter in that case. So to keep correct ordering
# we move CXXFLAGS to AM_CXXFLAGS with additional -Wno switch 
# and clean old CXXFLAGS variable
AM_CXXFLAGS = -g ${CXXFLAGS}

# Static sources for target machine plugins.
include_HEADERS = \
 plugin/TCEDAGToDAGISel.cc \
 plugin/TCEDAGToDAGISel.hh \
 plugin/TCEInstrFormats.td \
 plugin/TCEInstrInfo.cc \
 plugin/TCEInstrInfo.hh \
 plugin/TCEInstrInfo.td \
 plugin/TCEPlugin.hh \
 plugin/TCERegisterInfo.cc \
 plugin/TCERegisterInfo.hh \
 plugin/TCESubtarget.cc \
 plugin/TCEISelLowering.cc \
 plugin/TCEISelLowering.hh \
 plugin/TCETargetObjectFile.cc \
 plugin/TCETargetObjectFile.hh \
 plugin/TCEFrameInfo.hh \
 plugin/TCEFrameInfo.cc \
 plugin/TCETargetMachinePlugin.cc \
 plugin/PluginCompileWrapper.cc \
 TCETargetMachine.hh \
 TCETargetMachinePlugin.hh \
 plugin/TCESubtarget.hh \
 LLVMTCECmdLineOptions.hh \
 TCEStubTargetMachine.hh \
 TCEStubSubTarget.hh

MAINTAINERCLEANFILES = *~ *.gcov *.bbg *.bb *.da

## headers start
libtcellvmbackend_la_SOURCES += \
	TCETargetMachinePlugin.hh \
	LLVMTCECmdLineOptions.hh LLVMTCEPOMBuilder.hh \
	MachineInstrDDG.hh ProgramPartitioner.hh \
	LLVMTCEIRBuilder.hh LLVMBackend.hh \
	TDGen.hh LLVMTCEScheduler.hh \
	InstructionPatternAnalyzer.hh TCETargetMachine.hh \
	LLVMPOMBuilder.hh LLVMTCEBuilder.hh \
	TCEMCAsmInfo.hh ConstantTransformer.hh \
	TCEStubTargetMachine.hh \
	TCEStubSubTarget.hh \
	TCEStubTargetTransformInfo.hh \
	TCEStubSubTarget.hh

## headers end
