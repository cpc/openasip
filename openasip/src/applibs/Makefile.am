SUBDIRS = Interpreter Simulator osal TraceDB \
bem PIG Assembler hdb Scheduler Estimator ProGe EPSGenerator mach idf \
costdb program FSA Explorer dsdb  LLVMBackend ImplementationTester \
PlatformIntegrator TestGenerator HWGen

noinst_LTLIBRARIES = libapplibs.la

libapplibs_la_SOURCES = 
libapplibs_la_LIBADD = \
	Assembler/libassembler.la \
	Interpreter/libinterpreter.la \
	Simulator/libsimulator.la \
	TraceDB/libtracedb.la \
	osal/libapplibsosal.la \
	bem/libapplibsbem.la \
	PIG/libpig.la \
	hdb/libhdb.la \
	Scheduler/libscheduler.la \
	ProGe/libproge.la \
	mach/libapplibsmach.la \
	EPSGenerator/libepsgenerator.la \
	idf/libapplibsidf.la \
	costdb/libcostdb.la \
	program/libapplibsprogram.la \
	FSA/libfsa.la \
	Explorer/libexplorer.la \
	Estimator/libestimator.la \
	dsdb/libdsdb.la \
	LLVMBackend/libopenasipllvmbackend.la \
	ImplementationTester/libimpltester.la \
	PlatformIntegrator/libplatformintegrator.la \
        TestGenerator/libtege.la \
	HWGen/libhwgen.la 
  

if WX

SUBDIRS += wxToolkit
libapplibs_la_LIBADD += \
	wxToolkit/libwxtoolkit.la \
	wxToolkit/MachineWindow/libmachinewindow.la \
	wxToolkit/GUIOptions/libguioptions.la

endif
