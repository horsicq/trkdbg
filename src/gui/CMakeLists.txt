include(../../dep/build_tools/cmake/cpp_standart_setup.cmake)

add_definitions(-DUSE_DEX)
add_definitions(-DUSE_PDF)
add_definitions(-DUSE_ARCHIVE)
add_definitions(-DUSE_YARA)
add_definitions(-DUSE_XPROCESS)

include(${CMAKE_CURRENT_LIST_DIR}/../../dep/XDebuggerWidget/xdebuggerwidget.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/../../dep/XAboutWidget/xaboutwidget.cmake)

if(UNIX AND NOT APPLE)
    include(GNUInstallDirs)
endif()

set(TRKDBG_GUI_SOURCES
    ${XDEBUGGERWIDGET_SOURCES}
    ${XABOUTWIDGET_SOURCES}
    main_gui.cpp
    dialogabout.cpp
    dialogabout.h
    dialogabout.ui
    dialogoptions.cpp
    dialogoptions.h
    dialogoptions.ui
    guimainwindow.cpp
    guimainwindow.h
    guimainwindow.ui
    "${CMAKE_CURRENT_LIST_DIR}/../../res/resources_gui.qrc"
)

if(WIN32 AND EXISTS "${X_WINDOWS_RESOURCE_ICON_RC}")
    list(APPEND TRKDBG_GUI_SOURCES "${X_WINDOWS_RESOURCE_ICON_RC}")
endif()

if(APPLE AND EXISTS "${X_MACOS_ICON}")
    set_source_files_properties("${X_MACOS_ICON}" PROPERTIES
        MACOSX_PACKAGE_LOCATION "Resources"
    )
    list(APPEND TRKDBG_GUI_SOURCES "${X_MACOS_ICON}")
endif()

add_executable(TrkDbg WIN32
    ${TRKDBG_GUI_SOURCES}
)

set_target_properties(TrkDbg PROPERTIES
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
    MACOSX_BUNDLE_GUI_IDENTIFIER "${MACOSX_BUNDLE_GUI_IDENTIFIER}"
    OUTPUT_NAME trkdbg
)

target_include_directories(TrkDbg PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(TrkDbg PRIVATE
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
    target_link_libraries(TrkDbg PRIVATE
        Qt${TRKDBG_QT_MAJOR}::Script
        Qt${TRKDBG_QT_MAJOR}::ScriptTools
    )
endif()

if(TRKDBG_QT_MAJOR GREATER_EQUAL 6)
    target_link_libraries(TrkDbg PRIVATE
        Qt${TRKDBG_QT_MAJOR}::Qml
    )
endif()

if(WIN32)
    target_link_libraries(TrkDbg PRIVATE Wintrust Crypt32)
endif()

if(TARGET trkdbg_translations)
    add_dependencies(TrkDbg trkdbg_translations)
endif()

# --- Install rules ---
if(UNIX AND NOT APPLE)
    set(TRKDBG_RUNTIME_INSTALL_DESTINATION "${CMAKE_INSTALL_BINDIR}")
    set(TRKDBG_DOC_INSTALL_DESTINATION "${CMAKE_INSTALL_DOCDIR}/trkdbg")
    set(TRKDBG_DATA_INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/trkdbg")
    set(TRKDBG_DESKTOP_FILE "${CMAKE_CURRENT_BINARY_DIR}/trkdbg.desktop")

    configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/../../res/linux/trkdbg.desktop.in"
        "${TRKDBG_DESKTOP_FILE}"
        @ONLY
    )
else()
    set(TRKDBG_RUNTIME_INSTALL_DESTINATION ".")
    set(TRKDBG_DOC_INSTALL_DESTINATION ".")
    set(TRKDBG_DATA_INSTALL_DESTINATION ".")
endif()

install(TARGETS TrkDbg
    RUNTIME DESTINATION ${TRKDBG_RUNTIME_INSTALL_DESTINATION}
    BUNDLE DESTINATION .
)

install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../../dep/Detect-It-Easy/db" DESTINATION "${TRKDBG_DATA_INSTALL_DESTINATION}" OPTIONAL)
install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../../dep/Detect-It-Easy/db_custom" DESTINATION "${TRKDBG_DATA_INSTALL_DESTINATION}" OPTIONAL)
install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../../dep/XInfoDB/info" DESTINATION "${TRKDBG_DATA_INSTALL_DESTINATION}" OPTIONAL)
install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../../dep/XStyles/qss" DESTINATION "${TRKDBG_DATA_INSTALL_DESTINATION}" OPTIONAL)
install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../../dep/XYara/yara_rules" DESTINATION "${TRKDBG_DATA_INSTALL_DESTINATION}" OPTIONAL)
install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/../../dep/signatures/crypto.db" DESTINATION "${TRKDBG_DATA_INSTALL_DESTINATION}/signatures" OPTIONAL)

if(UNIX AND NOT APPLE)
    install(FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/../../LICENSE
        ${CMAKE_CURRENT_SOURCE_DIR}/../../README.md
        DESTINATION ${TRKDBG_DOC_INSTALL_DESTINATION}
        OPTIONAL
    )

    install(FILES
        "${TRKDBG_DESKTOP_FILE}"
        DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/applications"
    )

    install(DIRECTORY
        "${CMAKE_CURRENT_SOURCE_DIR}/../../res/linux/hicolor/"
        DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor"
    )
else()
    install(FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/../../LICENSE
        DESTINATION .
        RENAME LICENSE.txt
        OPTIONAL
    )

    install(FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/../../README.md
        ${CMAKE_CURRENT_SOURCE_DIR}/../../res/license.txt
        ${CMAKE_CURRENT_SOURCE_DIR}/../../res/readme.txt
        ${X_WINDOWS_ICON}
        DESTINATION .
        OPTIONAL
    )

    install(CODE [[
file(WRITE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/portable" "")
]])
endif()

# --- Platform deploy helpers ---
if(APPLE)
    include(${CMAKE_CURRENT_LIST_DIR}/../../dep/build_tools/cmake/macdeployqt.cmake)
endif()

if(WIN32)
    include(${CMAKE_CURRENT_LIST_DIR}/../../dep/build_tools/cmake/deploy_qt_windows.cmake)
    include(${CMAKE_CURRENT_LIST_DIR}/../../dep/build_tools/cmake/deploy_msvc.cmake)
endif()
