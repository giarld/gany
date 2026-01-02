cmake_minimum_required(VERSION 3.20)

project(GxAnyInterface)

set(TARGET_NAME gany)

add_library(${TARGET_NAME} INTERFACE)
target_include_directories(${TARGET_NAME} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
set_target_properties(${TARGET_NAME} PROPERTIES FOLDER GAny)

### Install
if (GX_LIBS_INSTALL_DIR)
    install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/gx DESTINATION ${GX_LIBS_INSTALL_DIR}/gany/include)
    install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/cmake/gany.cmake DESTINATION ${GX_LIBS_INSTALL_DIR}/gany)
    install(TARGETS ${TARGET_NAME}
            RUNTIME DESTINATION ${GX_LIBS_INSTALL_DIR}/gany/bin
            LIBRARY DESTINATION ${GX_LIBS_INSTALL_DIR}/gany/lib
            ARCHIVE DESTINATION ${GX_LIBS_INSTALL_DIR}/gany/lib
    )
endif ()
