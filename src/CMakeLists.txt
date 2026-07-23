# --- Qt version detection (supports Qt5 and Qt6) ---
if(TARGET Qt6::Core OR TARGET Qt6::Widgets)
    set(TRKDBG_QT_MAJOR 6)
elseif(TARGET Qt5::Core OR TARGET Qt5::Widgets)
    set(TRKDBG_QT_MAJOR 5)
else()
    find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
    set(TRKDBG_QT_MAJOR ${QT_VERSION_MAJOR})
endif()

if(TRKDBG_QT_MAJOR EQUAL 5)
    if(NOT TARGET Qt5::Widgets)
        find_package(Qt5 REQUIRED COMPONENTS Widgets Concurrent Network Script ScriptTools PrintSupport OpenGL Svg Sql LinguistTools)
    endif()
elseif(TRKDBG_QT_MAJOR GREATER_EQUAL 6)
    find_package(Qt${TRKDBG_QT_MAJOR} REQUIRED COMPONENTS Widgets Concurrent Network PrintSupport OpenGL Svg Sql Qml LinguistTools)
    if(NOT TARGET Qt5::Widgets)
        add_library(Qt5::Widgets INTERFACE IMPORTED)
        set_property(TARGET Qt5::Widgets PROPERTY
            INTERFACE_LINK_LIBRARIES Qt${TRKDBG_QT_MAJOR}::Widgets)
    endif()
endif()

# --- Version from file ---
file(STRINGS ${CMAKE_CURRENT_LIST_DIR}/../release_version.txt X_PROJECT_VERSION)
project(TrkDbg VERSION ${X_PROJECT_VERSION} LANGUAGES CXX)

# --- Project identity variables (used by deploy_init.cmake and RC templates) ---
set(X_COMPANYNAME "ntinfo")
set(X_PROJECTNAME "trkdbg")
set(X_MAINTAINER "horsicq@gmail.com")
set(X_DESCRIPTION "trkdbg is a debugger")
set(X_HOMEPAGE "http://ntinfo.biz")
set(X_ORIGINAL_FILENAME "trkdbg")

# --- macOS bundle ID ---
if(NOT DEFINED MACOSX_BUNDLE_GUI_IDENTIFIER OR "${MACOSX_BUNDLE_GUI_IDENTIFIER}" STREQUAL "")
    set(MACOSX_BUNDLE_GUI_IDENTIFIER "com.example.trkdbg")
endif()
set(BUNDLE_ID_OPTION "${MACOSX_BUNDLE_GUI_IDENTIFIER}")

# --- Resource paths ---
set(X_RESOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/../res")
set(X_WINDOWS_ICON "${X_RESOURCE_DIR}/windows/main.ico")
set(X_WINDOWS_RESOURCE_RC_IN "${X_RESOURCE_DIR}/windows/resource.rc.in")
set(X_WINDOWS_RESOURCE_ICON_RC_IN "${X_RESOURCE_DIR}/windows/resource_icon.rc.in")
set(X_WINDOWS_RESOURCE_RC "${X_RESOURCE_DIR}/windows/resource.rc")
set(X_WINDOWS_RESOURCE_ICON_RC "${X_RESOURCE_DIR}/windows/resource_icon.rc")
set(X_MACOS_ICON "${X_RESOURCE_DIR}/macos/main.icns")
set(X_MACOS_INFO_PLIST_IN "${X_RESOURCE_DIR}/macos/Info.plist.in")
set(X_MACOS_INFO_PLIST "${X_RESOURCE_DIR}/macos/Info.plist")

# --- Packaging support via _mylibs ---
set(TRKDBG_DEPLOY_INIT "${CMAKE_CURRENT_LIST_DIR}/../dep/build_tools/cmake/deploy_init.cmake")

if(EXISTS "${TRKDBG_DEPLOY_INIT}"
   AND EXISTS "${X_WINDOWS_RESOURCE_RC_IN}"
   AND EXISTS "${X_WINDOWS_RESOURCE_ICON_RC_IN}"
   AND EXISTS "${X_MACOS_ICON}"
   AND EXISTS "${X_MACOS_INFO_PLIST_IN}")
    include("${TRKDBG_DEPLOY_INIT}")
else()
    message(STATUS "Skipping deploy_init.cmake because packaging resource templates are missing")
endif()

# --- Translations ---
set(TRKDBG_TRANSLATION_HELPER "${CMAKE_CURRENT_LIST_DIR}/../dep/build_tools/cmake/deploy_translation.cmake")
if(EXISTS "${TRKDBG_TRANSLATION_HELPER}")
    include("${TRKDBG_TRANSLATION_HELPER}")

    set(TRKDBG_TS_FILES
        "${CMAKE_CURRENT_LIST_DIR}/../translations/trkdbg_ar_AR.ts"
        "${CMAKE_CURRENT_LIST_DIR}/../translations/trkdbg_bn_BN.ts"
        "${CMAKE_CURRENT_LIST_DIR}/../translations/trkdbg_de_DE.ts"
        "${CMAKE_CURRENT_LIST_DIR}/../translations/trkdbg_es_ES.ts"
        "${CMAKE_CURRENT_LIST_DIR}/../translations/trkdbg_fa_FA.ts"
        "${CMAKE_CURRENT_LIST_DIR}/../translations/trkdbg_fr_FR.ts"
        "${CMAKE_CURRENT_LIST_DIR}/../translations/trkdbg_he_IL.ts"
        "${CMAKE_CURRENT_LIST_DIR}/../translations/trkdbg_id_ID.ts"
        "${CMAKE_CURRENT_LIST_DIR}/../translations/trkdbg_it_IT.ts"
        "${CMAKE_CURRENT_LIST_DIR}/../translations/trkdbg_ja_JP.ts"
        "${CMAKE_CURRENT_LIST_DIR}/../translations/trkdbg_ko_KR.ts"
        "${CMAKE_CURRENT_LIST_DIR}/../translations/trkdbg_pl_PL.ts"
        "${CMAKE_CURRENT_LIST_DIR}/../translations/trkdbg_pt_BR.ts"
        "${CMAKE_CURRENT_LIST_DIR}/../translations/trkdbg_pt_PT.ts"
        "${CMAKE_CURRENT_LIST_DIR}/../translations/trkdbg_ru_RU.ts"
        "${CMAKE_CURRENT_LIST_DIR}/../translations/trkdbg_sv_SE.ts"
        "${CMAKE_CURRENT_LIST_DIR}/../translations/trkdbg_tr_TR.ts"
        "${CMAKE_CURRENT_LIST_DIR}/../translations/trkdbg_uk_UA.ts"
        "${CMAKE_CURRENT_LIST_DIR}/../translations/trkdbg_vi_VN.ts"
        "${CMAKE_CURRENT_LIST_DIR}/../translations/trkdbg_zh_CN.ts"
        "${CMAKE_CURRENT_LIST_DIR}/../translations/trkdbg_zh_TW.ts"
    )

    deploy_add_translations(
        TARGET_NAME trkdbg_translations
        ADD_TO_ALL
        SOURCE_DIRS "${CMAKE_CURRENT_LIST_DIR}"
        TS_FILES ${TRKDBG_TS_FILES}
        INSTALL_DESTINATION lang
        WINDOWS_APPDATA_SUBDIR "${X_PROJECTNAME}"
    )
endif()

# --- Compiled library subdirectories shared by gui and console ---
add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/../dep/XCppfilt" "${CMAKE_CURRENT_BINARY_DIR}/XCppfilt")
add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/../dep/XCapstone" "${CMAKE_CURRENT_BINARY_DIR}/XCapstone")
add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/../dep/XArchive" "${CMAKE_CURRENT_BINARY_DIR}/XArchive")
add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/../dep/XYara" "${CMAKE_CURRENT_BINARY_DIR}/XYara")

add_subdirectory(gui)
add_subdirectory(console)
