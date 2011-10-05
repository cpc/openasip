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
	-I${PROJECT_ROOT}/src/applibs/Scheduler/ResourceModel \
	-I${PROJECT_ROOT}/src/applibs/Scheduler/ProgramRepresentations/PDG \
        -D__STDC_LIMIT_MACROS -D__STDC_CONSTANT_MACROS

MAINTAINERCLEANFILES = *~ *.gcov *.bbg *.bb *.da

dist-hook:
	rm -rf $(distdir)/CVS $(distdir)/.deps $(distdir)/Makefile




## headers start
libapplibsmach_la_SOURCES += \
	MachineCheckResults.hh MachineValidator.hh \
	FUReservationTableIndex.hh MachineResourceModifier.hh \
	ProgrammabilityValidator.hh ProgrammabilityValidatorResults.hh \
	CollisionMatrix.hh RegisterQuantityCheck.hh \
	MachineValidatorResults.hh ResourceVector.hh \
	ReservationTable.hh MachineCheckSuite.hh \
	MinimalOpSetCheck.hh FUCollisionMatrixIndex.hh \
	BasicMachineCheckSuite.hh MachineCheck.hh \
	MachineInfo.hh OperationBindingCheck.hh \
	FullyConnectedCheck.hh MachineConnectivityCheck.hh \
	AddressSpaceCheck.hh RFPortCheck.hh \
	ResourceVectorSet.hh FUValidator.hh \
	ReservationTable.icc 
## headers end
