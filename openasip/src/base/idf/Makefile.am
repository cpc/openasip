
noinst_LTLIBRARIES = libidf.la
libidf_la_SOURCES = MachineImplementation.cc UnitImplementationLocation.cc \
IDFSerializer.cc NullUnitImplementationLocation.cc FUGenerated.cc RFGenerated.cc

libidf_la_LDFLAGS = $PTHREAD_LIBS

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
	UnitImplementationLocation.hh FUImplementationLocation.hh \
	NullFUImplementationLocation.hh NullRFImplementationLocation.hh \
	SocketImplementationLocation.hh RFImplementationLocation.hh \
	MachineImplementation.hh BusImplementationLocation.hh \
	NullUnitImplementationLocation.hh IDFSerializer.hh \
	FUGenerated.hh RFGenerated.hh
## headers end
