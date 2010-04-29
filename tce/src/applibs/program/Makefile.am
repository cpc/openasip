noinst_LTLIBRARIES = libapplibsprogram.la
libapplibsprogram_la_SOURCES = \
    POMValidator.cc POMValidatorResults.cc StaticProgramAnalyzer.cc

PROJECT_ROOT = $(top_srcdir)

AM_CPPFLAGS = -I${PROJECT_ROOT} -I${PROJECT_ROOT}/src/tools \
	-I${PROJECT_ROOT}/src/base/mach \
	-I${PROJECT_ROOT}/src/base/program \
	-I${PROJECT_ROOT}/src/base/osal

MAINTAINERCLEANFILES = *~ *.gcov *.bbg *.bb *.da

dist-hook:
	rm -rf $(distdir)/CVS $(distdir)/.deps $(distdir)/Makefile




## headers start
libapplibsprogram_la_SOURCES += \
	StaticProgramAnalyzer.hh POMValidator.hh \
	POMValidatorResults.hh 
## headers end
