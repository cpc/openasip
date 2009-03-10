SUBDIRS = MachineWindow GUIOptions

noinst_LTLIBRARIES = libwxtoolkit.la

libwxtoolkit_la_SOURCES = GUITextGenerator.cc ErrorDialog.cc \
WarningDialog.cc InformationDialog.cc WidgetTools.cc HelpBrowser.cc \
WxConversion.cc CommandRegistry.cc GUICommand.cc ConfirmDialog.cc \
FocusTrackingTextCtrl.cc NumberControl.cc MemoryControl.cc \
MemoryValueDialog.cc MessageDialog.cc MemoryGridTable.cc \
EPSDC.cc UserManualCmd.cc

PROJECT_ROOT = $(top_srcdir)

LIB_TCEtools_DIR = ${PROJECT_ROOT}/src/tools
LIB_MEMORY_DIR = ${PROJECT_ROOT}/src/base/memory
LIB_MACH_DIR = ${PROJECT_ROOT}/src/base/mach
LIB_EPS_GENERATOR_DIR = ${PROJECT_ROOT}/src/applibs/EPSGenerator

AM_WXCXXFLAGS = $(GUI_INCLUDES)

AM_CPPFLAGS = -I${LIB_TCEtools_DIR} -I${LIB_MEMORY_DIR} -I${LIB_MACH_DIR} \
	      -I${LIB_EPS_GENERATOR_DIR}
AM_CPPFLAGS += -I${PROJECT_ROOT} # Needed for config.h

AM_CXXFLAGS = $(AM_WXCXXFLAGS)

libwxtoolkit_la_LDFLAGS = $(GUI_LIBS)

MAINTAINERCLEANFILES = *~ *.gcov *.bbg *.bb *.da

