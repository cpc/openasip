SET(tceoclextgen_SRCS
  ExtensionGen.cc
)

include_directories(
	${PROJECT_SOURCE_DIR}/src/base/osal
    ${PROJECT_SOURCE_DIR}/src/base/mach
	${PROJECT_SOURCE_DIR}/src/tools
)

add_executable(tceoclextgen ${tceoclextgen_SRCS})
target_link_libraries(tceoclextgen libtce)

install(TARGETS tceoclextgen DESTINATION bin)

install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/opencl-tce
    DESTINATION bin
    PERMISSIONS OWNER_EXECUTE GROUP_EXECUTE WORLD_EXECUTE
        OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ)

install(FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/opencl_c_tce.hh 
    ${CMAKE_CURRENT_SOURCE_DIR}/opencl_types.hh
    DESTINATION include)
