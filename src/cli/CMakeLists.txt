cmake_minimum_required(VERSION 3.5)

include(../../dep/build_tools/cmake/deploy.cmake)
include(GNUInstallDirs)

# NOTE: this directory is NOT part of the build - src/CMakeLists.txt adds only
# gui and console, and src/console is the maintained copy of this console app.
# It is kept correct rather than left to rot: X_RESOURCES is not defined here
# (this file does not include cpp_standart_setup.cmake), so a blind sweep that
# replaced CMAKE_INSTALL_LIBDIR with ${X_RESOURCES} would have produced the
# absolute destination "/db" and installed into the DESTDIR root.
include(../../dep/build_tools/cmake/x_data_destination.cmake)
if(NOT DEFINED X_RESOURCES)
    x_data_install_destination(X_RESOURCES)
endif()

project(trkdbgc VERSION ${X_PROJECT_VERSION} LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_definitions(-DUSE_DEX)
add_definitions(-DUSE_PDF)
add_definitions(-DUSE_ARCHIVE)
add_definitions(-DUSE_YARA)
add_definitions(-DUSE_XPROCESS)

include(${CMAKE_CURRENT_LIST_DIR}/../../dep/XSpecDebugger/xspecdebugger.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/../../dep/XDebugScript/xdebugscript.cmake)

set(PROJECT_SOURCES
        ${XPECDEBUGGER_SOURCES}
        ${XDEBUGSCRIPT_SOURCES}
        main_console.cpp
)

if(WIN32)
    add_executable(trkdbgc WIN32
        ${PROJECT_SOURCES}
    )
else()
    add_executable(trkdbgc
        ${PROJECT_SOURCES}
    )
endif()

target_link_libraries(trkdbgc PRIVATE bzip2)
target_link_libraries(trkdbgc PRIVATE lzma)
target_link_libraries(trkdbgc PRIVATE zlib)
target_link_libraries(trkdbgc PRIVATE ppmd)
target_link_libraries(trkdbgc PRIVATE capstone)
target_link_libraries(trkdbgc PRIVATE cppfilt)
target_link_libraries(trkdbgc PRIVATE yara)

target_link_libraries(trkdbgc PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
target_link_libraries(trkdbgc PRIVATE Qt${QT_VERSION_MAJOR}::Concurrent)
target_link_libraries(trkdbgc PRIVATE Qt${QT_VERSION_MAJOR}::Network)
target_link_libraries(trkdbgc PRIVATE Qt${QT_VERSION_MAJOR}::PrintSupport)
target_link_libraries(trkdbgc PRIVATE Qt${QT_VERSION_MAJOR}::OpenGL)
target_link_libraries(trkdbgc PRIVATE Qt${QT_VERSION_MAJOR}::Svg)
target_link_libraries(trkdbgc PRIVATE Qt${QT_VERSION_MAJOR}::Sql)

if(${QT_VERSION_MAJOR} EQUAL 5)
    target_link_libraries(trkdbgc PRIVATE Qt${QT_VERSION_MAJOR}::Script)
    target_link_libraries(trkdbgc PRIVATE Qt${QT_VERSION_MAJOR}::ScriptTools)
endif()

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    target_link_libraries(trkdbgc PRIVATE Qt${QT_VERSION_MAJOR}::Qml)
endif()

if(WIN32)
    target_link_libraries(trkdbgc PRIVATE Wintrust)
    target_link_libraries(trkdbgc PRIVATE Crypt32)
endif()

if(WIN32)
    install (TARGETS trkdbgc DESTINATION "./")
    install (DIRECTORY ../../dep/Detect-It-Easy/db DESTINATION "./" OPTIONAL)
    install (DIRECTORY ../../dep/Detect-It-Easy/db_custom DESTINATION "./" OPTIONAL)
    install (DIRECTORY ../../dep/XInfoDB/info DESTINATION "./" OPTIONAL)
    install (DIRECTORY ../../dep/XStyles/qss DESTINATION "./" OPTIONAL)
    install (DIRECTORY ../../dep/XYara/yara_rules DESTINATION "./" OPTIONAL)
    install (FILES ../../dep/signatures/crypto.db DESTINATION "./signatures" OPTIONAL)
    deploy_qt()
    deploy_msvc()
else()
    install (TARGETS trkdbgc DESTINATION ${CMAKE_INSTALL_BINDIR})
    install (DIRECTORY ../../dep/Detect-It-Easy/db DESTINATION ${X_RESOURCES} OPTIONAL)
    install (DIRECTORY ../../dep/Detect-It-Easy/db_custom DESTINATION ${X_RESOURCES} OPTIONAL)
    install (DIRECTORY ../../dep/XInfoDB/info DESTINATION ${X_RESOURCES} OPTIONAL)
    install (DIRECTORY ../../dep/XStyles/qss DESTINATION ${X_RESOURCES} OPTIONAL)
    install (DIRECTORY ../../dep/XYara/yara_rules DESTINATION ${X_RESOURCES} OPTIONAL)
    install (FILES ../../dep/signatures/crypto.db DESTINATION ${X_RESOURCES}/signatures OPTIONAL)
endif()

# if(${QT_VERSION_MAJOR} EQUAL 6)
#     qt_finalize_executable(trkdbgc)
# endif()
