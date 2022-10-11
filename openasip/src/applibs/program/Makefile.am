noinst_LTLIBRARIES = libapplibsprogram.la
libapplibsprogram_la_SOURCES = \
    POMValidator.cc POMValidatorResults.cc StaticProgramAnalyzer.cc \
    CodeGenerator.cc LoopAnalyzer.cc

PROJECT_ROOT = $(top_srcdir)

AM_CPPFLAGS = -I${PROJECT_ROOT} -I${PROJECT_ROOT}/src/tools \
	-I${PROJECT_ROOT}/src/base/mach \
	-I${PROJECT_ROOT}/src/base/program \
	-I${PROJECT_ROOT}/src/base/osal \
	-I${PROJECT_ROOT}/src/base/umach \
	-I${PROJECT_ROOT}/src/base/Graph \
	-I${PROJECT_ROOT}/src/applibs/Scheduler/ProgramRepresentations/DDG \
	-I${PROJECT_ROOT}/src/applibs/Scheduler/ProgramRepresentations/CFG \
	-I${PROJECT_ROOT}/src/applibs/mach

MAINTAINERCLEANFILES = *~ *.gcov *.bbg *.bb *.da

dist-hook:
	rm -rf $(distdir)/CVS $(distdir)/.deps $(distdir)/Makefile




## headers start
libapplibsprogram_la_SOURCES += \
	POMValidator.hh POMValidatorResults.hh \
	StaticProgramAnalyzer.hh CodeGenerator.hh LoopAnalyzer.hh
## headers end
