noinst_LTLIBRARIES = libdsdb.la
libdsdb_la_SOURCES = DSDBManager.cc TestApplication.cc

PROJECT_ROOT = $(top_srcdir)

AM_CPPFLAGS = -I${PROJECT_ROOT} -I${PROJECT_ROOT}/src/tools \
	-I${PROJECT_ROOT}/src/base/mach -I${PROJECT_ROOT}/src/base/idf \
	-I${PROJECT_ROOT}/src/applibs/Simulator \
	-I${PROJECT_ROOT}/src/applibs/Estimator \
	-I${PROJECT_ROOT}/src/applibs/mach \
	-I${PROJECT_ROOT}/src/base/program

MAINTAINERCLEANFILES = *~ *.gcov *.bbg *.bb *.da

dist-hook:
	rm -rf $(distdir)/CVS $(distdir)/.deps $(distdir)/Makefile




## headers start
libdsdb_la_SOURCES += \
	TestApplication.hh DSDBManager.hh 
## headers end
