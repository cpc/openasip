noinst_LTLIBRARIES = libuniversalmachine.la
libuniversalmachine_la_SOURCES = UniversalMachine.cc \
UniversalFunctionUnit.cc UnboundedRegisterFile.cc UniversalFUPort.cc \
SmartHWOperation.cc

TOOLS_DIR = $(top_srcdir)/src/tools
OSAL_DIR = $(top_srcdir)/src/base/osal
MACH_DIR = $(top_srcdir)/src/base/mach

AM_CPPFLAGS = -I${TOOLS_DIR} -I${OSAL_DIR} -I${MACH_DIR}

dist-hook:
	rm -rf $(distdir)/CVS $(distdir)/.deps $(distdir)/Makefile

MAINTAINERCLEANFILES = *~ *.gcov *.bbg *.bb *.da


## headers start
libuniversalmachine_la_SOURCES += \
	SmartHWOperation.hh UnboundedRegisterFile.hh \
	UniversalFUPort.hh UniversalFunctionUnit.hh \
	UniversalMachine.hh 
## headers end
