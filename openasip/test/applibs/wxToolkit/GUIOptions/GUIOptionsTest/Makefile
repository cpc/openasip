DIST_OBJECTS = GUIOptionsSerializer.o \
	       GUIOptions.o \
               KeyboardShortcut.o \
               ToolbarButton.o

TOP_SRCDIR = ../../../../..

include ${TOP_SRCDIR}/test/Makefile_configure_settings

TOOL_OBJECTS = Exception.o\
               Environment.o\
               Application.o\
               ObjectState.o\
               XMLSerializer.o\
               DOMBuilderErrorHandler.o\
               FileSystem.o \
               Conversion.o

WXTOOLKIT_OBJS = CommandRegistry.o GUICommand.o

EXTRA_LINKER_FLAGS = ${XERCES_LDFLAGS} -lxerces-c -lpthread ${BOOST_LDFLAGS} \
		     ${GUI_LDFLAGS}

include ${TOP_SRCDIR}/test/Makefile_test.defs
