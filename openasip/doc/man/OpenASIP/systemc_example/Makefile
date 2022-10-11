tce_root=../../../../../tce/

include ${tce_root}/test/Makefile_configure_settings

tce_src_root=${tce_root}/src
tce_sim_inc=${tce_src_root}/applibs/Simulator
tce_lib_dir=${tce_src_root}/.libs
tcecc=${tce_src_root}/bintools/Compiler/tcecc

# copied from test/Makefile_test.defs
TCE_LIB_DIRS = \
    $(shell find ${tce_root}/src -name .libs)
LIB_MAGIC = -Wl,-rpath -Wl,
TCE_LIB_FLAGS = $(addprefix ${LIB_MAGIC},${TCE_LIB_DIRS})
TCE_LIBS = ${tce_root}/src/.libs/libopenasip${LIBRARY_SUFFIX}

simulator: simulator.cc mmio_send.tpef mmio_recv.tpef mem_map.h
	${CXX} -fpermissive simulator.cc -I${tce_sim_inc} -I${tce_src_root}/base/osal \
	-I${tce_src_root}/tools -I${tce_src_root}/.. ${TCE_LIB_FLAGS} ${TCE_LIBS} \
	${LDFLAGS} -g -lsystemc -O3 -o simulator

mmio_send.tpef: mmio_send.c mmio.adf mem_map.h
	${tcecc} -a mmio.adf mmio_send.c -O3 -o mmio_send.tpef

mmio_recv.tpef: mmio_recv.c mmio.adf mem_map.h
	${tcecc} -a mmio.adf mmio_recv.c -O3 -o mmio_recv.tpef

clean:
	@rm -f simulator mmio_send.tpef mmio_recv.tpef