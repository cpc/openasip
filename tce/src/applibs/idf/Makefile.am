lib_LTLIBRARIES = libapplibsidf.la
libapplibsidf_la_SOURCES = IDFValidator.cc

PROJECT_ROOT = $(top_srcdir)

AM_CPPFLAGS = -I${PROJECT_ROOT} -I${PROJECT_ROOT}/src/tools \
	-I${PROJECT_ROOT}/src/base/idf -I${PROJECT_ROOT}/src/base/mach

MAINTAINERCLEANFILES = *~ *.gcov *.bbg *.bb *.da

dist-hook:
	rm -rf $(distdir)/CVS $(distdir)/.deps $(distdir)/Makefile
