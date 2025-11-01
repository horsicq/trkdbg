cmake_minimum_required(VERSION 3.5)

include(../../dep/build_tools/cmake/deploy.cmake)
include(GNUInstallDirs)

project(trkdbg VERSION ${X_PROJECT_VERSION} LANGUAGES CXX)

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

include(${CMAKE_CURRENT_LIST_DIR}/../../dep/XDebuggerWidget/xdebuggerwidget.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/../../dep/XAboutWidget/xaboutwidget.cmake)

set(PROJECT_SOURCES
        ${XDEBUGGERWIDGET_SOURCES}
        ${XABOUTWIDGET_SOURCES}
        dialogabout.cpp
        dialogabout.h
        dialogabout.ui
        dialogoptions.cpp
        dialogoptions.h
        dialogoptions.ui
        main_gui.cpp
        guimainwindow.cpp
        guimainwindow.h
        guimainwindow.ui
        rsrc.qrc
)

if(WIN32)
    add_executable(trkdbg WIN32
        ${PROJECT_SOURCES}
    )
else()
    add_executable(trkdbg
        ${PROJECT_SOURCES}
        ../../res/resource.rc
    )
endif()

target_link_libraries(trkdbg PRIVATE bzip2)
target_link_libraries(trkdbg PRIVATE lzma)
target_link_libraries(trkdbg PRIVATE zlib)
target_link_libraries(trkdbg PRIVATE ppmd)
target_link_libraries(trkdbg PRIVATE capstone)
target_link_libraries(trkdbg PRIVATE cppfilt)
target_link_libraries(trkdbg PRIVATE yara)

target_link_libraries(trkdbg PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
target_link_libraries(trkdbg PRIVATE Qt${QT_VERSION_MAJOR}::Concurrent)
target_link_libraries(trkdbg PRIVATE Qt${QT_VERSION_MAJOR}::Network)
target_link_libraries(trkdbg PRIVATE Qt${QT_VERSION_MAJOR}::PrintSupport)
target_link_libraries(trkdbg PRIVATE Qt${QT_VERSION_MAJOR}::OpenGL)
target_link_libraries(trkdbg PRIVATE Qt${QT_VERSION_MAJOR}::Svg)
target_link_libraries(trkdbg PRIVATE Qt${QT_VERSION_MAJOR}::Sql)

if(${QT_VERSION_MAJOR} EQUAL 5)
    target_link_libraries(trkdbg PRIVATE Qt${QT_VERSION_MAJOR}::Script)
    target_link_libraries(trkdbg PRIVATE Qt${QT_VERSION_MAJOR}::ScriptTools)
endif()

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    target_link_libraries(trkdbg PRIVATE Qt${QT_VERSION_MAJOR}::Qml)
endif()

if(WIN32)
    target_link_libraries(trkdbg PRIVATE Wintrust)
    target_link_libraries(trkdbg PRIVATE Crypt32)
endif()

if(WIN32)
    install (TARGETS trkdbg DESTINATION "./")
    install (DIRECTORY ../../dep/Detect-It-Easy/db DESTINATION "./" OPTIONAL)
    install (DIRECTORY ../../dep/Detect-It-Easy/db_custom DESTINATION "./" OPTIONAL)
    install (DIRECTORY ../../dep/XInfoDB/info DESTINATION "./" OPTIONAL)
    install (DIRECTORY ../../dep/XStyles/qss DESTINATION "./" OPTIONAL)
    install (DIRECTORY ../../dep/XYara/yara_rules DESTINATION "./" OPTIONAL)
    install (FILES ../../dep/signatures/crypto.db DESTINATION "./signatures" OPTIONAL)
    deploy_qt()
    deploy_msvc()
else()
    install (TARGETS trkdbg DESTINATION ${CMAKE_INSTALL_BINDIR})
    install (DIRECTORY ../../dep/Detect-It-Easy/db DESTINATION ${CMAKE_INSTALL_LIBDIR}/trkdbg OPTIONAL)
    install (DIRECTORY ../../dep/Detect-It-Easy/db_custom DESTINATION ${CMAKE_INSTALL_LIBDIR}/trkdbg OPTIONAL)
    install (DIRECTORY ../../dep/XInfoDB/info DESTINATION ${CMAKE_INSTALL_LIBDIR}/trkdbg OPTIONAL)
    install (DIRECTORY ../../dep/XStyles/qss DESTINATION ${CMAKE_INSTALL_LIBDIR}/trkdbg OPTIONAL)
    install (DIRECTORY ../../dep/XYara/yara_rules DESTINATION ${CMAKE_INSTALL_LIBDIR}/trkdbg OPTIONAL)
    install (FILES ../../dep/signatures/crypto.db DESTINATION ${CMAKE_INSTALL_LIBDIR}/trkdbg/signatures OPTIONAL)
endif()

# if(${QT_VERSION_MAJOR} EQUAL 6)
#     qt_finalize_executable(trkdbg)
# endif()
