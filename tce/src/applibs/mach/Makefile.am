noinst_LTLIBRARIES = libapplibsmach.la
libapplibsmach_la_SOURCES = MachineValidator.cc MachineValidatorResults.cc \
ProgrammabilityValidator.cc  ProgrammabilityValidatorResults.cc FUValidator.cc \
ResourceVector.cc ResourceVectorSet.cc MachineCheck.cc \
MachineCheckResults.cc MachineCheckSuite.cc MachineConnectivityCheck.cc \
FullyConnectedCheck.cc MachineResourceModifier.cc AddressSpaceCheck.cc \
ReservationTable.cc FUCollisionMatrixIndex.cc FUReservationTableIndex.cc \
CollisionMatrix.cc RFPortCheck.cc BasicMachineCheckSuite.cc MachineInfo.cc \
OperationBindingCheck.cc RegisterQuantityCheck.cc MinimalOpSetCheck.cc

PROJECT_ROOT = $(top_srcdir)

AM_CPPFLAGS = -I${PROJECT_ROOT} -I${PROJECT_ROOT}/src/tools \
	-I${PROJECT_ROOT}/src/base/mach \
	-I${PROJECT_ROOT}/src/base/Graph \
	-I${PROJECT_ROOT}/src/base/program \
	-I${PROJECT_ROOT}/src/base/osal \
	-I${PROJECT_ROOT}/src/base/tpef \
	-I${PROJECT_ROOT}/src/base/bem \
	-I${PROJECT_ROOT}/src/applibs/osal \
	-I${PROJECT_ROOT}/src/applibs/bem \
	-I${PROJECT_ROOT}/src/applibs/FSA \
	-I${PROJECT_ROOT}/src/applibs/Scheduler/ResourceManager \
	-I${PROJECT_ROOT}/src/applibs/Scheduler/ResourceModel

MAINTAINERCLEANFILES = *~ *.gcov *.bbg *.bb *.da

dist-hook:
	rm -rf $(distdir)/CVS $(distdir)/.deps $(distdir)/Makefile


## headers start
libapplibsmach_la_SOURCES += \
	AddressSpaceCheck.hh BasicMachineCheckSuite.hh \
	CollisionMatrix.hh FUCollisionMatrixIndex.hh \
	FUReservationTableIndex.hh FUValidator.hh \
	FullyConnectedCheck.hh MachineCheck.hh \
	MachineCheckResults.hh MachineCheckSuite.hh \
	MachineConnectivityCheck.hh MachineInfo.hh \
	MachineResourceModifier.hh MachineValidator.hh \
	MachineValidatorResults.hh MinimalOpSetCheck.hh \
	OperationBindingCheck.hh ProgrammabilityValidator.hh \
	ProgrammabilityValidatorResults.hh RFPortCheck.hh \
	RegisterQuantityCheck.hh ReservationTable.hh \
	ResourceVector.hh ResourceVectorSet.hh \
	ReservationTable.icc 
## headers end
