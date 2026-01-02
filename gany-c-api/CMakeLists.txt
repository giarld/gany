cmake_minimum_required(VERSION 3.20)

project(GxAnyCApi)

set(TARGET_NAME gany-c-api)

file(GLOB_RECURSE GX_ANY_C_SRC ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c)
file(GLOB_RECURSE GX_ANY_C_HDR ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h)

add_library(${TARGET_NAME}
        ${GX_ANY_C_SRC}
        ${GX_ANY_C_HDR}
        )
target_include_directories(${TARGET_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(${TARGET_NAME} PUBLIC gany)

if (BUILD_SHARED_LIBS)
    target_compile_definitions(${TARGET_NAME} PRIVATE BUILD_SHARED_LIBS=1)
endif ()

target_compile_options(${TARGET_NAME}
        PRIVATE
        $<$<CXX_COMPILER_ID:MSVC>:/bigobj>
        $<$<AND:$<CXX_COMPILER_ID:GNU>,$<BOOL:${GNU_BIG_OBJ_FLAG_ENABLE}>>:-Wa,-mbig-obj>)

set_target_properties(${TARGET_NAME} PROPERTIES FOLDER GAny)

## Install
if (GX_LIBS_INSTALL_DIR)
    install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/gx DESTINATION ${GX_LIBS_INSTALL_DIR}/gany-c-api/include)
    install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/cmake/gany-c-api.cmake DESTINATION ${GX_LIBS_INSTALL_DIR}/gany-c-api)
    install(TARGETS ${TARGET_NAME}
            RUNTIME DESTINATION ${GX_LIBS_INSTALL_DIR}/gany-c-api/bin
            LIBRARY DESTINATION ${GX_LIBS_INSTALL_DIR}/gany-c-api/lib
            ARCHIVE DESTINATION ${GX_LIBS_INSTALL_DIR}/gany-c-api/lib
            )
endif ()
