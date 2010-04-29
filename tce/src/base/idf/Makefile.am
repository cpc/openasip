
noinst_LTLIBRARIES = libidf.la
libidf_la_SOURCES = MachineImplementation.cc UnitImplementationLocation.cc \
IDFSerializer.cc NullUnitImplementationLocation.cc 

libidf_la_LDFLAGS = -lpthread

PROJECT_ROOT = $(top_srcdir)
DOXYGEN_CONFIG_FILE = ${PROJECT_ROOT}/tools/Doxygen/doxygen.config

AM_CPPFLAGS = -I${PROJECT_ROOT}/src/tools
AM_CXXFLAGS = -UNDEBUG

dist-hook:
	rm -rf $(distdir)/CVS $(distdir)/.deps $(distdir)/Makefile

.PHONY: doc

MAINTAINERCLEAN = *~ *.gcov *.bbg *.bb *.da




## headers start
libidf_la_SOURCES += \
	NullFUImplementationLocation.hh MachineImplementation.hh \
	SocketImplementationLocation.hh BusImplementationLocation.hh \
	NullRFImplementationLocation.hh NullUnitImplementationLocation.hh \
	FUImplementationLocation.hh UnitImplementationLocation.hh \
	RFImplementationLocation.hh IDFSerializer.hh 
## headers end
