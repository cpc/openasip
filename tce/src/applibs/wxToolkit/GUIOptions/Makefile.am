noinst_LTLIBRARIES = libguioptions.la

libguioptions_la_SOURCES = ToolbarButton.cc KeyboardShortcut.cc \
	GUIOptions.cc GUIOptionsSerializer.cc OptionsDialog.cc \
	KeyboardShortcutDialog.cc

PROJECT_ROOT = $(top_srcdir)

LIB_TCEtools_DIR = ${PROJECT_ROOT}/src/tools
LIB_WXTOOLKIT_DIR = ${PROJECT_ROOT}/src/applibs/wxToolkit

AM_WXCXXFLAGS = $(GUI_INCLUDES)

AM_CPPFLAGS = -I${LIB_TCEtools_DIR} -I${LIB_WXTOOLKIT_DIR}
AM_CXXFLAGS = $(AM_WXCXXFLAGS)

libguioptions_la_LDFLAGS = $(GUI_LIBS)

MAINTAINERCLEANFILES = *~ *.gcov *.bbg *.bb *.da




## headers start
libguioptions_la_SOURCES += \
	OptionsDialog.hh GUIOptions.hh \
	ToolbarButton.hh KeyboardShortcutDialog.hh \
	GUIOptionsSerializer.hh KeyboardShortcut.hh 
## headers end
