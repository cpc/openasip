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
	EditPolicyFactory.hh BridgeFactory.hh \
	BusChainFigure.hh Request.hh \
	BusFactory.hh SocketPortConnFactory.hh \
	ContentsFigure.hh BidirBridgeFigure.hh \
	ConnectionFigure.hh MachineCanvasTool.hh \
	MachineEditPartFactory.hh UnknownSocketFigure.hh \
	ConnectionFactory.hh SocketContainerFigure.hh \
	EditPart.hh ConnectionEditPart.hh \
	UnitContainerFigure.hh FUFactory.hh \
	SegmentFigure.hh SocketBusConnFactory.hh \
	SocketBusConnFigure.hh IUFactory.hh \
	EditPolicy.hh InputSocketFigure.hh \
	RFFactory.hh EditPartFactory.hh \
	GCUFactory.hh ComponentCommand.hh \
	Factory.hh TriggeringPortFigure.hh \
	UnitPortFigure.hh MachineCanvas.hh \
	BusContainerFigure.hh SegmentFactory.hh \
	BridgeFigure.hh OutputSocketFigure.hh \
	MachineCanvasOptions.hh MachineCanvasLayoutConstraints.hh \
	SocketPortConnFigure.hh BusFigure.hh \
	MoveFigure.hh UnitFigure.hh \
	UnitPortFactory.hh Figure.hh \
	SocketFactory.hh RootEditPart.hh \
	SelectionFigure.hh BridgeFigure.icc \
	Request.icc ConnectionFigure.icc \
	Figure.icc EditPolicy.icc \
	SegmentFigure.icc RootEditPart.icc \
	EditPart.icc EditPartFactory.icc \
	EditPolicyFactory.icc ConnectionEditPart.icc 
## headers end
