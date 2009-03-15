noinst_LTLIBRARIES = libapplibsbem.la
libapplibsbem_la_SOURCES = BEMGenerator.cc BEMValidator.cc

PROJECT_ROOT = $(top_srcdir)

AM_CPPFLAGS = -I${PROJECT_ROOT} -I${PROJECT_ROOT}/src/tools \
	-I${PROJECT_ROOT}/src/base/bem -I${PROJECT_ROOT}/src/base/mach

MAINTAINERCLEANFILES = *~ *.gcov *.bbg *.bb *.da

dist-hook:
	rm -rf $(distdir)/CVS $(distdir)/.deps $(distdir)/Makefile


## headers start
libapplibsbem_la_SOURCES += \
	BEMGenerator.hh BEMValidator.hh 
## headers end
