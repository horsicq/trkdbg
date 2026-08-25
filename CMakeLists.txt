cmake_minimum_required(VERSION 3.16)
project(TrkDbg)
include(CTest)
add_subdirectory(src)

if(BUILD_TESTING)
    find_package(Python3 3.7 COMPONENTS Interpreter QUIET)

    if(Python3_Interpreter_FOUND)
        add_test(
            NAME trkdbg_scripting_source_contract
            COMMAND "${Python3_EXECUTABLE}"
                    "${CMAKE_CURRENT_SOURCE_DIR}/tests/test_scripting_release.py"
                    --source-root "${CMAKE_CURRENT_SOURCE_DIR}"
        )
        add_test(
            NAME trkdbg_gui_load_source_contract
            COMMAND "${Python3_EXECUTABLE}"
                    "${CMAKE_CURRENT_SOURCE_DIR}/tests/test_gui_load_contract.py"
                    --source-root "${CMAKE_CURRENT_SOURCE_DIR}"
        )
        add_test(
            NAME trkdbg_gui_lifecycle_source_contract
            COMMAND "${Python3_EXECUTABLE}"
                    "${CMAKE_CURRENT_SOURCE_DIR}/tests/test_gui_lifecycle_contract.py"
                    --source-root "${CMAKE_CURRENT_SOURCE_DIR}"
        )
        add_test(
            NAME trkdbg_gui_execution_state_source_contract
            COMMAND "${Python3_EXECUTABLE}"
                    "${CMAKE_CURRENT_SOURCE_DIR}/tests/test_gui_execution_state_contract.py"
                    --source-root "${CMAKE_CURRENT_SOURCE_DIR}"
        )
        add_test(
            NAME trkdbg_step_over_source_contract
            COMMAND "${Python3_EXECUTABLE}"
                    "${CMAKE_CURRENT_SOURCE_DIR}/tests/test_step_over_contract.py"
                    --source-root "${CMAKE_CURRENT_SOURCE_DIR}"
        )
        add_test(
            NAME trkdbg_unix_stop_source_contract
            COMMAND "${Python3_EXECUTABLE}"
                    "${CMAKE_CURRENT_SOURCE_DIR}/tests/test_unix_stop_contract.py"
                    --source-root "${CMAKE_CURRENT_SOURCE_DIR}"
        )
        if(WIN32 OR UNIX)
            add_executable(trkdbg_script_smoke_target "${CMAKE_CURRENT_SOURCE_DIR}/tests/script_smoke_target.cpp")
            add_executable(trkdbg_step_over_smoke_target "${CMAKE_CURRENT_SOURCE_DIR}/tests/step_over_smoke_target.cpp")
        endif()
        if(WIN32)
            get_filename_component(TRKDBG_QT_BIN_DIR "${Qt5Core_DIR}/../../../bin" ABSOLUTE)
            add_test(
                NAME trkdbg_scripting_release_integration
                COMMAND "${Python3_EXECUTABLE}"
                        "${CMAKE_CURRENT_SOURCE_DIR}/tests/test_scripting_release.py"
                        --source-root "${CMAKE_CURRENT_SOURCE_DIR}"
                        --console "$<TARGET_FILE:trkdbgc>"
                        --target "$<TARGET_FILE:trkdbg_script_smoke_target>"
                        --qt-bin "${TRKDBG_QT_BIN_DIR}"
            )
            set_tests_properties(trkdbg_scripting_release_integration PROPERTIES TIMEOUT 60)
        endif()
        if(UNIX)
            add_test(
                NAME trkdbg_unix_stop_integration
                COMMAND "${Python3_EXECUTABLE}"
                        "${CMAKE_CURRENT_SOURCE_DIR}/tests/test_unix_stop_integration.py"
                        --console "$<TARGET_FILE:trkdbgc>"
                        --target "$<TARGET_FILE:trkdbg_script_smoke_target>"
            )
            set_tests_properties(trkdbg_unix_stop_integration PROPERTIES TIMEOUT 60)
        endif()
    else()
        message(WARNING "BUILD_TESTING is enabled, but Python 3 was not found; trkdbg tests will not be registered")
    endif()
endif()
