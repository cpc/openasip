noinst_LTLIBRARIES = libmachinewindow.la

libmachinewindow_la_SOURCES = MachineEditPartFactory.cc BidirBridgeFigure.cc \
	BridgeFactory.cc BridgeFigure.cc BusChainFigure.cc \
	BusContainerFigure.cc BusFactory.cc BusFigure.cc ConnectionFactory.cc \
	ConnectionFigure.cc ContentsFigure.cc EditPart.cc EditPartFactory.cc \
	EditPolicy.cc FUFactory.cc Factory.cc Figure.cc GCUFactory.cc \
	IUFactory.cc InputSocketFigure.cc OutputSocketFigure.cc RFFactory.cc \
	RootEditPart.cc SegmentFactory.cc SegmentFigure.cc SelectionFigure.cc \
	SocketBusConnFactory.cc SocketBusConnFigure.cc \
	SocketContainerFigure.cc SocketFactory.cc \
	SocketPortConnFactory.cc SocketPortConnFigure.cc \
	UnitContainerFigure.cc UnitFigure.cc ConnectionEditPart.cc \
	UnitPortFactory.cc UnitPortFigure.cc UnknownSocketFigure.cc \
	MachineCanvas.cc EditPolicyFactory.cc MachineCanvasTool.cc \
	Request.cc ComponentCommand.cc MoveFigure.cc TriggeringPortFigure.cc \
	MachineCanvasOptions.cc


PROJECT_ROOT = $(top_srcdir)

LIB_TCEtools_DIR = ${PROJECT_ROOT}/src/tools
LIB_MACH_DIR = ${PROJECT_ROOT}/src/base/mach
LIB_UMACH_DIR = ${PROJECT_ROOT}/src/base/umach
LIB_WXTOOLKIT_DIR = ${PROJECT_ROOT}/src/applibs/wxToolkit
LIB_EPS_GENERATOR_DIR = ${PROJECT_ROOT}/src/applibs/EPSGenerator

AM_WXCXXFLAGS = $(GUI_INCLUDES)

AM_CPPFLAGS = -I${LIB_TCEtools_DIR} -I${LIB_MACH_DIR} -I${LIB_WXTOOLKIT_DIR} \
	-I${LIB_EPS_GENERATOR_DIR} -I${LIB_UMACH_DIR}

AM_CXXFLAGS = $(AM_WXCXXFLAGS)

libmachinewindow_la_LDFLAGS = $(GUI_LIBS)

MAINTAINERCLEANFILES = *~ *.gcov *.bbg *.bb *.da


## headers start
libmachinewindow_la_SOURCES += \
	BidirBridgeFigure.hh BridgeFactory.hh \
	BridgeFigure.hh BusChainFigure.hh \
	BusContainerFigure.hh BusFactory.hh \
	BusFigure.hh ComponentCommand.hh \
	ConnectionEditPart.hh ConnectionFactory.hh \
	ConnectionFigure.hh ContentsFigure.hh \
	EditPart.hh EditPartFactory.hh \
	EditPolicy.hh EditPolicyFactory.hh \
	FUFactory.hh Factory.hh \
	Figure.hh GCUFactory.hh \
	IUFactory.hh InputSocketFigure.hh \
	MachineCanvas.hh MachineCanvasLayoutConstraints.hh \
	MachineCanvasOptions.hh MachineCanvasTool.hh \
	MachineEditPartFactory.hh MoveFigure.hh \
	OutputSocketFigure.hh RFFactory.hh \
	Request.hh RootEditPart.hh \
	SegmentFactory.hh SegmentFigure.hh \
	SelectionFigure.hh SocketBusConnFactory.hh \
	SocketBusConnFigure.hh SocketContainerFigure.hh \
	SocketFactory.hh SocketPortConnFactory.hh \
	SocketPortConnFigure.hh TriggeringPortFigure.hh \
	UnitContainerFigure.hh UnitFigure.hh \
	UnitPortFactory.hh UnitPortFigure.hh \
	UnknownSocketFigure.hh BridgeFigure.icc \
	ConnectionEditPart.icc ConnectionFigure.icc \
	EditPart.icc EditPartFactory.icc \
	EditPolicy.icc EditPolicyFactory.icc \
	Figure.icc Request.icc \
	RootEditPart.icc SegmentFigure.icc 
## headers end
