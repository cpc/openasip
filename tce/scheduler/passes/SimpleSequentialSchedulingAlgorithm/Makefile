TCE_ROOT_DIR = ../../..
SRC_ROOT_DIR = ${TCE_ROOT_DIR}/src
BASE_DIR = ${SRC_ROOT_DIR}/base
APPLIBS_DIR = ${SRC_ROOT_DIR}/applibs
TOOLS_DIR = ${SRC_ROOT_DIR}/tools
SCHEDULER_DIR = ${APPLIBS_DIR}/Scheduler
POM_DIR = ${BASE_DIR}/program
TPEF_DIR = ${BASE_DIR}/tpef
OSAL_DIR = ${BASE_DIR}/osal
MACH_DIR = ${BASE_DIR}/mach
UM_DIR = ${BASE_DIR}/umach

COMPILER = $(shell if which tce-config > /dev/null 2>&1; then echo `tce-config --c++-compiler`; else echo g++; fi;)
CXXFLAGS = $(shell if which tce-config > /dev/null 2>&1; then echo `tce-config --cxxflags`; else echo -g -ansi -Wall -W -pedantic -Wno-long-long; fi;)
INCLUDES = -I${POM_DIR} -I${SCHEDULER_DIR} -I${TOOLS_DIR} -I${TCE_ROOT_DIR} \
	-I${MACH_DIR} -I${UM_DIR} -I${OSAL_DIR}
SOFLAGS = $(shell if which tce-config > /dev/null 2>&1; then echo `tce-config --so-flags`; else echo -lgcc -shared -fPIC -o; fi;)

all: SimpleRegisterAllocator.so SimpleResourceAllocator.so

SimpleRegisterAllocator.so: SimpleRegisterAllocator.hh SimpleRegisterAllocator.cc
	${COMPILER} ${CXXFLAGS} ${INCLUDES} ${SOFLAGS} SimpleRegisterAllocator.so \
	    SimpleRegisterAllocator.cc

SimpleResourceAllocator.so: SimpleResourceAllocator.hh SimpleResourceAllocator.cc
	${COMPILER}  ${CXXFLAGS} ${INCLUDES} ${SOFLAGS} SimpleResourceAllocator.so \
	    SimpleResourceAllocator.cc

clean:
	rm -f *.so *.o *~
