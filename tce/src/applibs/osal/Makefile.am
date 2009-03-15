noinst_LTLIBRARIES = libapplibsosal.la
libapplibsosal_la_SOURCES = OperationBuilder.cc OperationSimulator.cc \
							OperationDAGSelector.cc

PROJECT_ROOT = $(top_srcdir)

AM_CPPFLAGS = -I${PROJECT_ROOT} \
	-I${PROJECT_ROOT}/src/tools \
	-I${PROJECT_ROOT}/src/base/osal \
	-I${PROJECT_ROOT}/src/base/Graph \
	-I${PROJECT_ROOT}/src/base/memory

MAINTAINERCLEANFILES = *~ *.gcov *.bbg *.bb *.da

dist-hook:
	rm -rf $(distdir)/CVS $(distdir)/.deps $(distdir)/Makefile


## headers start
libapplibsosal_la_SOURCES += \
	OperationBuilder.hh OperationDAGSelector.hh \
	OperationSimulator.hh 
## headers end
