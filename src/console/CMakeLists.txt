set(CMAKE_AUTOMOC ON)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Same feature flags as gui: the console shares the same scan libraries
add_definitions(-DUSE_DEX)
add_definitions(-DUSE_PDF)
add_definitions(-DUSE_ARCHIVE)
add_definitions(-DUSE_YARA)
add_definitions(-DUSE_XPROCESS)

include(${CMAKE_CURRENT_LIST_DIR}/../../../_mylibs/XSpecDebugger/xspecdebugger.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/../../../_mylibs/XDebugScript/xdebugscript.cmake)

if(UNIX AND NOT APPLE)
    include(GNUInstallDirs)
endif()

add_executable(trkdbgc
    ${XPECDEBUGGER_SOURCES}
    ${XDEBUGSCRIPT_SOURCES}
    main_console.cpp
)

target_link_libraries(trkdbgc PRIVATE
    bzip2
    lzma
    zlib
    ppmd
    capstone
    cppfilt
    yara
    Qt${TRKDBG_QT_MAJOR}::Widgets
    Qt${TRKDBG_QT_MAJOR}::Concurrent
    Qt${TRKDBG_QT_MAJOR}::Network
    Qt${TRKDBG_QT_MAJOR}::PrintSupport
    Qt${TRKDBG_QT_MAJOR}::OpenGL
    Qt${TRKDBG_QT_MAJOR}::Svg
    Qt${TRKDBG_QT_MAJOR}::Sql
)

if(TRKDBG_QT_MAJOR EQUAL 5)
    target_link_libraries(trkdbgc PRIVATE
        Qt${TRKDBG_QT_MAJOR}::Script
        Qt${TRKDBG_QT_MAJOR}::ScriptTools
    )
endif()

if(TRKDBG_QT_MAJOR GREATER_EQUAL 6)
    target_link_libraries(trkdbgc PRIVATE
        Qt${TRKDBG_QT_MAJOR}::Qml
    )
endif()

if(WIN32)
    target_link_libraries(trkdbgc PRIVATE Wintrust Crypt32)
endif()

if(TARGET trkdbg_translations)
    add_dependencies(trkdbgc trkdbg_translations)
endif()

if(UNIX AND NOT APPLE)
    install(TARGETS trkdbgc RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}")
else()
    install(TARGETS trkdbgc RUNTIME DESTINATION ".")
endif()
