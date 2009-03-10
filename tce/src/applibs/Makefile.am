SUBDIRS = Interpreter Simulator osal TraceDB Disassembler \
bem PIG Assembler hdb Scheduler Estimator ProGe EPSGenerator mach idf \
costdb program FSA Explorer dsdb

noinst_LTLIBRARIES = libapplibs.la

libapplibs_la_SOURCES = 
libapplibs_la_LIBADD = \
	Assembler/libassembler.la \
	Disassembler/libdisassembler.la \
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
	dsdb/libdsdb.la

if LLVM
SUBDIRS += LLVMBackend
libapplibs_la_LIBADD += LLVMBackend/libtcellvmbackend.la
endif

if WX

SUBDIRS += wxToolkit
libapplibs_la_LIBADD += \
	wxToolkit/libwxtoolkit.la \
	wxToolkit/MachineWindow/libmachinewindow.la \
	wxToolkit/GUIOptions/libguioptions.la

endif
