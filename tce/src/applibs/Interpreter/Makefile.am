noinst_LTLIBRARIES = libinterpreter.la
libinterpreter_la_SOURCES = ScriptInterpreter.cc CustomCommand.cc \
	LineReader.cc Script.cc ExpressionScript.cc \
	ConditionScript.cc CmdHelp.cc InterpreterContext.cc \
	BaseLineReader.cc SimpleScriptInterpreter.cc LineReaderFactory.cc \
	TclConditionScript.cc TclInterpreter.cc

if HAVE_EDITLINE
  libinterpreter_la_SOURCES += EditLineReader.cc
endif

AM_CPPFLAGS = -I${PROJECT_ROOT}/src/tools
AM_CPPFLAGS += -I${PROJECT_ROOT} # Needed for config.h
libinterpreter_la_LDFLAGS = ${TCL_LIBDIR} ${EDITLINE_LIBDIR} ${EDITLINE_LD_FLAGS}

PROJECT_ROOT = $(top_srcdir)

MAINTAINERCLEANFILES = *~ *.gcov *.bbg *.bb *.da


## headers start
libinterpreter_la_SOURCES += \
	BaseLineReader.hh CmdHelp.hh \
	ConditionScript.hh CustomCommand.hh \
	EditLineReader.hh ExpressionScript.hh \
	InterpreterContext.hh LineReader.hh \
	LineReaderFactory.hh Script.hh \
	ScriptInterpreter.hh SimpleScriptInterpreter.hh \
	TclConditionScript.hh TclInterpreter.hh \
	CustomCommand.icc LineReader.icc \
	Script.icc 
## headers end
