TOP_SRCDIR = ../../../..
TOOL_DIR = ${TOP_SRCDIR}/src/tools
DIST_DIR = ${TOP_SRCDIR}/src/applibs/Interpreter

include ${TOP_SRCDIR}/test/Makefile_configure_settings 

DIST_OBJECTS = LineReader.o EditLineReader.o LineReaderFactory.o
TOOL_OBJECTS = Exception.o StringTools.o Environment.o FileSystem.o \
	Application.o Conversion.o
EXTRA_LINKER_FLAGS = ${EDITLINE_LD_FLAGS} ${BOOST_LDFLAGS}
#INITIALIZATION = test

include ${TOP_SRCDIR}/test/Makefile_test.defs

test:	TclInterpLineReader.cc
	@(${CXX} -I${TOOL_DIR} -I${DIST_DIR} -o test TclInterpLineReader.cc \
	${LDFLAGS} \
	${EDITLINE_LD_FLAGS} ${TCL_LD_FLAGS} ${TOOL_DIR}/Exception.o \
	${TOOL_DIR}/StringTools.o ${TOOL_DIR}/DataObject.o \
	${DIST_DIR}/LineReader.o ${DIST_DIR}/EditLineReader.o \
	${DIST_DIR}/ScriptInterpreter.o ${DIST_DIR}/TclInterpreter.o \
	${DIST_DIR}/CustomCommand.o ${TOOL_DIR}/Conversion.o \
	${TOOL_DIR}/Application.o ${DIST_DIR}/BaseLineReader.o \
	${DIST_DIR}/LineReaderFactory.o)
