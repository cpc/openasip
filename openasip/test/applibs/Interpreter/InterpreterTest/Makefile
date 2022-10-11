TOP_SRCDIR = ../../../..

include ${TOP_SRCDIR}/test/Makefile_configure_settings 

DIST_OBJECTS = ScriptInterpreter.o TclInterpreter.o CustomCommand.o \
	LineReader.o EditLineReader.o Script.o ExpressionScript.o \
	ConditionScript.o CmdHelp.o InterpreterContext.o \
	SimpleScriptInterpreter.o
TOOL_OBJECTS = Exception.o Conversion.o DataObject.o Application.o \
	StringTools.o FileSystem.o Environment.o
EXTRA_COMPILER_FLAGS = ${EDITLINE_INCLUDES} ${CPPFLAGS}
EXTRA_LINKER_FLAGS = ${TCL_LD_FLAGS} ${EDITLINE_LD_FLAGS} ${BOOST_LDFLAGS}

include ${TOP_SRCDIR}/test/Makefile_test.defs
