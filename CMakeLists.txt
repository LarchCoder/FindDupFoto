cmake_minimum_required (VERSION 2.8)
project (FindDupFoto)

if (DEFINED PROJECT_DIR)
    set(PROJECT_DIR "${PROJECT_SOURCE_DIR}/${PROJECT_DIR}")
else ()
    set(PROJECT_DIR "${PROJECT_SOURCE_DIR}")
endif ()
set(CMAKE_INSTALL_PREFIX "${PROJECT_DIR}")


set(DEFS
	LARCHCODER=1
    )

include_directories(
    ${PROJECT_DIR}/include
    )

file(GLOB_RECURSE SOURCE_TMP "${PROJECT_DIR}/src/*.c" )
list(APPEND SRC ${SOURCE_TMP})


#add all definitions
foreach (DEF ${DEFS})
    add_definitions(-D${DEF})
endforeach()


add_definitions(-g3 -c -fmessage-length=0)

set(EXT_LIBS m pthread apr-1 aprutil-1)


add_executable(${PROJECT_NAME} ${SRC})
target_link_libraries(${PROJECT_NAME} ${EXT_LIBS})

install(TARGETS ${PROJECT_NAME} DESTINATION "bin/")