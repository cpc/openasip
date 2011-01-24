SET(SCHEDULER_PLUGINS
    PreOptimizerModule.cc
)

SET(SCHEDULER_DIR ${PROJECT_SOURCE_DIR}/src/applibs/Scheduler)
include_directories(
    ${PROJECT_BINARY_DIR}
    ${PROJECT_SOURCE_DIR}/src/tools
    ${PROJECT_SOURCE_DIR}/src/base/mach
    ${PROJECT_SOURCE_DIR}/src/base/osal
    ${PROJECT_SOURCE_DIR}/src/base/program
    ${PROJECT_SOURCE_DIR}/src/base/umach
    ${PROJECT_SOURCE_DIR}/src/base/Graph
    ${PROJECT_SOURCE_DIR}/src/applibs/mach
    ${PROJECT_SOURCE_DIR}/src/applibs/Disassembler
    ${SCHEDULER_DIR}
    ${SCHEDULER_DIR}/Algorithms
    ${SCHEDULER_DIR}/Algorithms/StackFrameManagement
    ${SCHEDULER_DIR}/ProgramRepresentations/CFG
    ${SCHEDULER_DIR}/ProgramRepresentations/DDG
    ${SCHEDULER_DIR}/ProgramRepresentations/PDG
    ${SCHEDULER_DIR}/ResourceModel
    ${SCHEDULER_DIR}/ResourceManager
    ${SCHEDULER_DIR}/Selector
    ${BOOST_INCLUDES}
)

foreach(arg ${SCHEDULER_PLUGINS})
    get_filename_component(_NAME ${arg} NAME_WE)
    add_library(${_NAME} MODULE ${_NAME}.cc)
    set_target_properties(${_NAME} PROPERTIES
        COMPILE_FLAGS ${CONFIGURE_CPPFLAGS}
        LINK_FLAGS "${CONFIGURE_LDFLAGS} ${SHARED_CXX_FLAGS}"
        PREFIX "")
    install(TARGETS ${_NAME}
        DESTINATION share/tce/scheduler/passes/PreOptimizer)
endforeach(arg ${SCHEDULER_PLUGINS})
