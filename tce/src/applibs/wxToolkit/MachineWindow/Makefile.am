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
	EditPolicy.hh SegmentFactory.hh \
	UnknownSocketFigure.hh EditPolicyFactory.hh \
	TriggeringPortFigure.hh MachineCanvasTool.hh \
	Request.hh RootEditPart.hh \
	RFFactory.hh SocketBusConnFactory.hh \
	BusChainFigure.hh BidirBridgeFigure.hh \
	GCUFactory.hh ComponentCommand.hh \
	UnitPortFactory.hh UnitPortFigure.hh \
	MachineCanvasOptions.hh Figure.hh \
	BridgeFactory.hh IUFactory.hh \
	MachineCanvas.hh SelectionFigure.hh \
	SocketFactory.hh EditPart.hh \
	OutputSocketFigure.hh UnitContainerFigure.hh \
	ConnectionFactory.hh ConnectionFigure.hh \
	BusFactory.hh SocketContainerFigure.hh \
	MoveFigure.hh Factory.hh \
	FUFactory.hh UnitFigure.hh \
	ConnectionEditPart.hh SocketPortConnFigure.hh \
	ContentsFigure.hh BridgeFigure.hh \
	SocketBusConnFigure.hh InputSocketFigure.hh \
	EditPartFactory.hh MachineEditPartFactory.hh \
	SegmentFigure.hh MachineCanvasLayoutConstraints.hh \
	BusFigure.hh BusContainerFigure.hh \
	SocketPortConnFactory.hh EditPolicy.icc \
	RootEditPart.icc EditPartFactory.icc \
	ConnectionEditPart.icc Request.icc \
	SegmentFigure.icc Figure.icc \
	ConnectionFigure.icc EditPolicyFactory.icc \
	BridgeFigure.icc EditPart.icc 
## headers end
