bin_PROGRAMS = tdgen_transport
tdgen_transport_SOURCES = tdgen_transport.cc

SRC_ROOT_DIR = $(top_srcdir)/src
LIB_MACH_DIR = ${SRC_ROOT_DIR}/base/mach
LIB_TCETOOLS_DIR = ../../../tools
LIB_APPLIBS_MACH_DIR = ../../../applibs/mach
LIB_APPLIBS_LLVMBACKEND_DIR = ../../../applibs/LLVMBackend

tdgen_transport_LDADD = ../../../libtce.la

AM_CPPFLAGS = -I${LIB_TCETOOLS_DIR} -I${LIB_APPLIBS_MACH_DIR} \
	-I${LIB_APPLIBS_LLVMBACKEND_DIR} -I${LIB_MACH_DIR}
AM_LDFLAGS = ${TCE_LDFLAGS}
