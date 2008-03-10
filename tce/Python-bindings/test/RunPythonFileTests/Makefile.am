PROJECT_ROOT = ../../..
AM_CPPFLAGS = -I${PROJECT_ROOT} -I${PROJECT_ROOT}/src/tools \
	-I${PROJECT_ROOT}/src/base/mach \
	-I${PROJECT_ROOT}/src/base/umach \
	-I${PROJECT_ROOT}/src/base/osal \
	-I${PROJECT_ROOT}/src/base/program \
	-I${PROJECT_ROOT}/src/base/tpef \
	-I${PROJECT_ROOT}/src/base/memory \
	-I${PROJECT_ROOT}/src/base/Graph \
	-I${PROJECT_ROOT}/src/applibs/Scheduler/ProgramRepresentations/ProgramDependenceGraph \
	-I${PROJECT_ROOT}/src/applibs/Scheduler/ResourceModel \
	-I${PROJECT_ROOT}/src/applibs/Scheduler/ResourceManager \
        ${PYTHON_CPPFLAGS}
AM_LDFLAGS = ${PYTHON_LDFLAGS} ${PYTHON_EXTRA_LDFLAGS} ${BOOST_PYTHON_LDFLAGS} \
	-L${PROJECT_ROOT}/src/base -lttabase \
	-L${PROJECT_ROOT}/src/tools -ltcetools \
	-L${PROJECT_ROOT}/src/applibs -lapplibs \
        ${TCL_LD_FLAGS}
bin_PROGRAMS = run_python_file
run_python_file_SOURCES = run_python_file.cc
