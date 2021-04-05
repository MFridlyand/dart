# Copyright (c) 2011-2021, The DART development contributors

# ==============================================================================
function(dart_property_add property_name)
  get_property(is_defined GLOBAL PROPERTY ${property_name} DEFINED)

  if(NOT is_defined)
    define_property(
      GLOBAL PROPERTY ${property_name} BRIEF_DOCS "${property_name}"
      FULL_DOCS "Global properties for ${property_name}"
    )
  endif()

  foreach(item ${ARGN})
    set_property(GLOBAL APPEND PROPERTY ${property_name} "${item}")
  endforeach()
endfunction()

# ==============================================================================
# dart_get_max(var [value1 value2...])
function(dart_get_max var)
  set(first YES)
  set(choice NO)
  foreach(item ${ARGN})
    if(first)
      set(choice ${item})
      set(first NO)
    elseif(choice LESS ${item})
      set(choice ${item})
    endif()
  endforeach(item)
  set(${var} ${choice} PARENT_SCOPE)
endfunction()

# ==============================================================================
# dart_get_max_string_length(var [value1 value2...])
function(dart_get_max_string_length var)
  foreach(item ${ARGN})
    string(LENGTH ${item} length)
    list(APPEND list ${length})
  endforeach()
  dart_get_max(choice ${list})
  set(${var} ${choice} PARENT_SCOPE)
endfunction()

# ==============================================================================
# cmake-format: off
# dart_option(<variable> "<help_text>" <value>)
# cmake-format: on
function(dart_option variable help_text default_value)
  set_property(
    GLOBAL PROPERTY DART_DETAIL_PROPERTY_OPTION_VARIABLE "${variable}" APPEND
  )
  set_property(
    GLOBAL PROPERTY DART_DETAIL_property_option_help_text "${help_text}" APPEND
  )
  set_property(
    GLOBAL PROPERTY DART_DETAIL_property_option_default_value "${default_value}"
    APPEND
  )

  # Add option
  option(${variable} ${help_text} ${default_value})

  # Normalize boolean value variants (e.g. 1/0, On/Off, TRUE/FALSE) to ON/OFF
  if(${variable})
    set(${variable} ON PARENT_SCOPE)
  else()
    set(${variable} OFF PARENT_SCOPE)
  endif()

endfunction()

# ==============================================================================
# cmake-format: off
# dart_print_options()
# cmake-format: on
function(dart_print_options)
  # Print the header
  message(STATUS "[ Options ]")

  get_property(
    option_variables GLOBAL PROPERTY DART_DETAIL_PROPERTY_OPTION_VARIABLE
  )
  get_property(
    option_help_texts GLOBAL PROPERTY DART_DETAIL_property_option_help_text
  )
  get_property(
    option_default_values GLOBAL
    PROPERTY DART_DETAIL_property_option_default_value
  )

  dart_get_max_string_length(option_variable_max_len ${option_variables})
  list(LENGTH option_variables option_count)
  math(EXPR option_count "${option_count} - 1")
  foreach(val RANGE ${option_count})
    list(GET option_variables ${val} option_variable)
    list(GET option_default_values ${val} option_default_value)

    set(option_str "- ${option_variable}")
    set(spaces "")
    string(LENGTH ${option_variable} option_variable_len)
    math(EXPR space_count "${option_variable_max_len} - ${option_variable_len}")
    foreach(loop_var RANGE ${space_count})
      set(option_str "${option_str} ")
    endforeach()

    set(option_str "${option_str}: ${${option_variable}}")

    if(${option_variable} STREQUAL option_default_value)
      set(option_str "${option_str} [default]")
    endif()

    message(STATUS "${option_str}")
  endforeach()

  message(STATUS "")
endfunction()

# ==============================================================================
function(dart_clang_format_setup)
  cmake_parse_arguments(
    CF_ARG # prefix
    "" # no boolean args
    "VERSION" # clang-format version
    "" # no multi-value args
    ${ARGN}
  )
  if(NOT CF_ARG_VERSION)
    set(CF_NAME clang-format)
  else()
    set(CF_NAME clang-format-${CF_ARG_VERSION})
  endif()
  find_program(CLANG_FORMAT_EXECUTABLE NAMES ${CF_NAME})

  if(NOT CLANG_FORMAT_EXECUTABLE)
    message(STATUS "Looking for clang-format - NOT found, please install "
                   "${CF_NAME} to enable automatic code formatting."
    )
    return()
  endif()

  message(STATUS "Found ${CF_NAME}.")
endfunction()

# ==============================================================================
function(_property_add property_name)
  get_property(is_defined GLOBAL PROPERTY ${property_name} DEFINED)
  if(NOT is_defined)
    define_property(
      GLOBAL PROPERTY ${property_name} BRIEF_DOCS "${property_name}"
      FULL_DOCS "Global properties for ${property_name}"
    )
  endif()
  foreach(item ${ARGN})
    set_property(GLOBAL APPEND PROPERTY ${property_name} "${item}")
  endforeach()
endfunction()

# ==============================================================================
function(dart_clang_format_add_sources)
  foreach(source ${ARGN})
    if(IS_ABSOLUTE "${source}")
      set(source_abs "${source}")
    else()
      get_filename_component(
        source_abs "${CMAKE_CURRENT_LIST_DIR}/${source}" ABSOLUTE
      )
    endif()
    if(EXISTS "${source_abs}")
      _property_add(CLANG_FORMAT_FORMAT_FILES "${source_abs}")
    else()
      message(
        FATAL_ERROR
          "Source file '${source}' does not exist at absolute path"
          " '${source_abs}'. This should never happen. Did you recently delete"
          " this file or modify 'CMAKE_CURRENT_LIST_DIR'"
      )
    endif()
  endforeach()
endfunction()

# ==============================================================================
function(dart_clang_format_add_targets)
  get_property(formatting_files GLOBAL PROPERTY CLANG_FORMAT_FORMAT_FILES)
  list(LENGTH formatting_files formatting_files_length)

  if(formatting_files AND CLANG_FORMAT_EXECUTABLE)
    message(STATUS "Formatting on ${formatting_files_length} source files.")

    add_custom_target(
      clang-format
      COMMAND ${CMAKE_COMMAND} -E echo
              "Formatting ${formatting_files_length} files..."
      COMMAND ${CLANG_FORMAT_EXECUTABLE} -style=file -i ${formatting_files}
      COMMAND ${CMAKE_COMMAND} -E echo "Done."
      DEPENDS ${CLANG_FORMAT_EXECUTABLE}
      WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )
    add_custom_target(
      clang-format-check
      COMMAND ${CMAKE_COMMAND} -E echo
              "Checking ${formatting_files_length} files..."
      COMMAND ${CMAKE_SOURCE_DIR}/scripts/check_format.sh
              ${CLANG_FORMAT_EXECUTABLE} ${formatting_files}
      COMMAND ${CMAKE_COMMAND} -E echo "Done."
      DEPENDS ${CLANG_FORMAT_EXECUTABLE}
      WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )
  else()
    add_custom_target(
      format COMMAND ${CMAKE_COMMAND} -E echo "No file to format code style."
    )
    add_custom_target(
      check-format COMMAND ${CMAKE_COMMAND} -E echo
                           "No file to check code style."
    )
  endif()
endfunction()

# ==============================================================================
# cmake-format: off
# dart_build_tests(
#   TYPE <test_type>
#   SOURCES <sources>
#   [INCLUDE_DIRS <include_dependencies>]
#   [LINK_LIBRARIES <library_dependencies>]
#   [LINK_DART_LIBRARIES <library_dependencies>]
#   [TEST_LIST <output_var>]
# )
#
# Build multiple tests. Arguments are as follows:
#
# - TYPE           : Required. Preferably UNIT or INTEGRATION.
#
# - TARGET_PREFIX  : Optional. Prefix of the target name.
#
# - SOURCES        : Required. The list of source files for your tests. Each file
#                    will turn into a test.
#
# - INCLUDE_DIRS   : Optional. Additional include directories that should be
#                    visible to all the tests.
#
# - LINK_LIBRARIES : Optional. Additional library dependencies that every test
#                    should link to including the library built by this project.
#                    'gtest' and 'gtest_main' will be automatically linked.
#
# - LINK_DART_LIBRARIES:
#                    Optional. DART library dependencies.
#
# - COMPILE_DEFINITIONS:
#
# - TEST_LIST      : Optional. Provide a variable which will be given the list of the
#                    target names of the tests generated by this function.
# cmake-format: on
function(dart_build_tests)
  set(prefix dart_build_tests)
  set(options)
  set(oneValueArgs TYPE TARGET_PREFIX TEST_LIST)
  set(multiValueArgs SOURCES INCLUDE_DIRS LINK_LIBRARIES LINK_DART_LIBRARIES
                     COMPILE_DEFINITIONS
  )
  cmake_parse_arguments(
    "${prefix}" "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN}
  )

  if(NOT dart_build_tests_TYPE)
    message(
      FATAL_ERROR "DEVELOPER ERROR: You must specify a TYPE for your tests!"
    )
  endif()

  set(test_type ${dart_build_tests_TYPE})

  if(NOT BUILD_TESTING)
    message(STATUS "Testing is disabled -- skipping ${test_type} tests")
    return()
  endif()

  if(NOT DEFINED dart_build_tests_SOURCES)
    message(STATUS "No tests have been specified for ${test_type}")
  else()
    list(LENGTH dart_build_tests_SOURCES num_tests)
    message(STATUS "Adding ${num_tests} ${test_type} tests")
  endif()

  if(dart_build_tests_TEST_LIST)
    set(${dart_build_tests_TEST_LIST} "")
  endif()

  foreach(source ${dart_build_tests_SOURCES})

    # Set target name: <TYPE>[_<TARGET_PREFIX>]_<source>
    set(target_name ${test_type})
    if(dart_build_tests_TARGET_PREFIX)
      set(target_name "${target_name}_${dart_build_tests_TARGET_PREFIX}")
    endif()
    get_filename_component(source_name ${source} NAME_WE)
    string(REPLACE "test_" "" source_name ${source_name})
    get_filename_component(source_dir ${source} DIRECTORY)
    if(source_dir)
      string(REPLACE "/" "_" source_prefix ${source_dir})
      set(target_name "${target_name}_${source_prefix}_${source_name}")
    else()
      set(target_name "${target_name}_${source_name}")
    endif()

    add_executable(${target_name} ${source})
    add_test(NAME ${target_name} COMMAND $<TARGET_FILE:${target_name}>)
    target_include_directories(
      ${target_name} PRIVATE ${dart_build_tests_INCLUDE_DIRS}
    )

    target_link_libraries(${target_name} PRIVATE gtest gmock gtest_main)

    if(UNIX)
      # gtest requies pthread when compiled on a Unix machine
      target_link_libraries(${target_name} PRIVATE pthread)
    endif()

    target_link_libraries(
      ${target_name} PRIVATE ${dart_build_tests_LINK_LIBRARIES}
    )

    if(dart_build_tests_COMPILE_DEFINITIONS)
      target_compile_definitions(
        ${target_name} PRIVATE ${dart_build_tests_COMPILE_DEFINITIONS}
      )
    endif()

    foreach(dart_lib ${dart_build_tests_LINK_DART_LIBRARIES})
      if(NOT TARGET ${dart_lib})
        message(FATAL_ERROR "Invalid target: ${dart_lib}")
      endif()
      target_link_libraries(${target_name} PRIVATE ${dart_lib})
    endforeach()

    if(dart_build_tests_TEST_LIST)
      list(APPEND ${dart_build_tests_TEST_LIST} ${target_name})
    endif()

  endforeach()

  if(dart_build_tests_TEST_LIST)
    set(${dart_build_tests_TEST_LIST} "${${dart_build_tests_TEST_LIST}}"
        PARENT_SCOPE
    )
  endif()

  dart_clang_format_add_sources(${dart_build_tests_SOURCES})

endfunction()

# ==============================================================================
# cmake-format: off
# dart_build_benchmarks(
#   TYPE <test_type>
#   SOURCES <sources>
#   [INCLUDE_DIRS <include_dependencies>]
#   [LINK_LIBRARIES <library_dependencies>]
#   [LINK_DART_LIBRARIES <library_dependencies>]
#   [TEST_LIST <output_var>]
# )
#
# Build multiple tests. Arguments are as follows:
#
# - TYPE          : Required. Preferably UNIT or INTEGRATION.
#
# - TARGET_PREFIX : Optional. Prefix of the target name.
#
# - SOURCES       : Required. The list of source files for your tests. Each file
#                   will turn into a test.
#
# - INCLUDE_DIRS  : Optional. Additional include directories that should be
#                   visible to all the tests.
#
# - LINK_LIBRARIES: Optional. Additional library dependencies that every test
#                   should link to including the library built by this project.
#                   'gtest' and 'gtest_main' will be automatically linked.
#
# - LINK_DART_LIBRARIES:
#                   Optional. DART library dependencies.
#
# - TEST_LIST     : Optional. Provide a variable which will be given the list of the
#                   target names of the tests generated by this function.
# cmake-format: on
function(dart_build_benchmarks)
  set(prefix dart_build_benchmarks)
  set(options)
  set(oneValueArgs TYPE TARGET_PREFIX TEST_LIST)
  set(multiValueArgs SOURCES INCLUDE_DIRS LINK_LIBRARIES LINK_DART_LIBRARIES)
  cmake_parse_arguments(
    "${prefix}" "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN}
  )

  if(NOT dart_build_benchmarks_TYPE)
    message(
      FATAL_ERROR "DEVELOPER ERROR: You must specify a TYPE for your tests!"
    )
  endif()

  set(test_type ${dart_build_benchmarks_TYPE})

  if(NOT BUILD_TESTING)
    message(STATUS "Testing is disabled -- skipping ${test_type} tests")
    return()
  endif()

  if(NOT DEFINED dart_build_benchmarks_SOURCES)
    message(STATUS "No tests have been specified for ${test_type}")
  else()
    list(LENGTH dart_build_benchmarks_SOURCES num_tests)
    message(STATUS "Adding ${num_tests} ${test_type} tests")
  endif()

  if(dart_build_benchmarks_TEST_LIST)
    set(${dart_build_benchmarks_TEST_LIST} "")
  endif()

  foreach(source ${dart_build_benchmarks_SOURCES})

    # Set target name: <TYPE>[_<TARGET_PREFIX>]_<source>
    set(target_name ${test_type})
    if(dart_build_benchmarks_TARGET_PREFIX)
      set(target_name "${target_name}_${dart_build_benchmarks_TARGET_PREFIX}")
    endif()
    get_filename_component(source_name ${source} NAME_WE)
    string(REPLACE "benchmark_" "" source_name ${source_name})
    get_filename_component(source_dir ${source} DIRECTORY)
    if(source_dir)
      string(REPLACE "/" "_" source_prefix ${source_dir})
      set(target_name "${target_name}_${source_prefix}_${source_name}")
    else()
      set(target_name "${target_name}_${source_name}")
    endif()

    add_executable(${target_name} ${source})
    # add_test(NAME ${target_name} COMMAND $<TARGET_FILE:${target_name}>)
    target_include_directories(
      ${target_name} PRIVATE ${dart_build_benchmarks_INCLUDE_DIRS}
    )

    target_link_libraries(${target_name} PRIVATE benchmark benchmark_main)

    if(UNIX)
      # gtest requies pthread when compiled on a Unix machine
      target_link_libraries(${target_name} PRIVATE pthread)
    endif()

    target_link_libraries(
      ${target_name} PRIVATE ${dart_build_benchmarks_LINK_LIBRARIES}
    )

    foreach(dart_lib ${dart_build_benchmarks_LINK_DART_LIBRARIES})
      if(NOT TARGET ${dart_lib})
        message(FATAL_ERROR "Invalid target: ${dart_lib}")
      endif()
      target_link_libraries(${target_name} PRIVATE ${dart_lib})
    endforeach()

    if(dart_build_benchmarks_TEST_LIST)
      list(APPEND ${dart_build_benchmarks_TEST_LIST} ${target_name})
    endif()

  endforeach()

  if(dart_build_benchmarks_TEST_LIST)
    set(${dart_build_benchmarks_TEST_LIST}
        "${${dart_build_benchmarks_TEST_LIST}}" PARENT_SCOPE
    )
  endif()

  dart_clang_format_add_sources(${dart_build_benchmarks_SOURCES})

endfunction()

# ==============================================================================
# cmake-format: off
# Generate header file.
# Usage:
#   dart_generate_meta_header()
# cmake-format: on
# ==============================================================================
function(dart_generate_meta_header)
  set(prefix dart_generate_meta_header)
  set(options INSTALL)
  set(oneValueArgs DESTINATION PREFIX_TO_REMOVE OUTPUT_VAR)
  set(multiValueArgs HEADERS)
  cmake_parse_arguments(
    "${prefix}" "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN}
  )

  if(NOT dart_generate_meta_header_DESTINATION)
    message(FATAL_ERROR "DESTINATION must be specified")
  endif()

  set(destination ${dart_generate_meta_header_DESTINATION})
  if(NOT IS_ABSOLUTE ${destination})
    set(destination ${CMAKE_CURRENT_BINARY_DIR}/${destination})
  endif()

  file(WRITE ${destination} "// Automatically generated file by CMake\n\n")
  foreach(header ${dart_generate_meta_header_HEADERS})
    if(IS_ABSOLUTE ${header})
      string(REPLACE ${dart_generate_meta_header_PREFIX_TO_REMOVE} "" header
                     ${header}
      )
    endif()
    file(APPEND ${destination} "#include \"${header}\"\n")
  endforeach()

  if(dart_generate_meta_header_OUTPUT_VAR)
    set(${dart_generate_meta_header_OUTPUT_VAR} ${destination} PARENT_SCOPE)
  endif()
endfunction()

# ==============================================================================
# cmake-format: off
# Generate header file.
# Usage:
#   dart_add_module(...)
# cmake-format: on
# ==============================================================================
function(dart_add_module)
  set(prefix dart_add_module)
  set(options GENERATE_META_HEADER)
  set(oneValueArgs MODULE_NAME TARGET_NAME PROJECT_SOURCE_DIR
                   PROJECT_BINARY_DIR
  )
  set(multiValueArgs
      HEADERS SOURCES PUBLIC_LINK_LIBRARIES PUBLIC_LINK_OPTIONS
      PUBLIC_COMPILE_FEATURES PUBLIC_COMPILE_OPTIONS PUBLIC_COMPILE_DEFINITIONS
      PRIVATE_LINK_LIBRARIES
  )
  cmake_parse_arguments(
    "${prefix}" "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN}
  )

  # Shorter variable names for readability
  set(project_source_dir ${dart_add_module_PROJECT_SOURCE_DIR})
  set(project_binary_dir ${dart_add_module_PROJECT_BINARY_DIR})
  set(module_name ${dart_add_module_MODULE_NAME})
  set(target_name ${dart_add_module_TARGET_NAME})
  set(headers ${dart_add_module_HEADERS})
  set(sources ${dart_add_module_SOURCES})
  set(public_link_libraries ${dart_add_module_PUBLIC_LINK_LIBRARIES})
  set(public_link_options ${dart_add_module_PUBLIC_LINK_OPTIONS})
  set(public_compile_features ${dart_add_module_PUBLIC_COMPILE_FEATURES})
  set(public_compile_options ${dart_add_module_PUBLIC_COMPILE_OPTIONS})
  set(public_compile_definitions ${dart_add_module_PUBLIC_COMPILE_DEFINITIONS})
  set(private_link_libraries ${dart_add_module_PRIVATE_LINK_LIBRARIES})

  # Add library
  add_library(${target_name} ${headers} ${sources})

  # Set include directory
  target_include_directories(
    ${target_name}
    PUBLIC $<BUILD_INTERFACE:${project_source_dir}/src>
           $<BUILD_INTERFACE:${project_binary_dir}/src>
           $<INSTALL_INTERFACE:include/${PROJECT_NAME}${DART_VERSION_MAJOR}>
  )

  # Set link libraries
  target_link_libraries(${target_name} PUBLIC ${public_link_libraries})
  target_link_libraries(${target_name} PRIVATE ${private_link_libraries})

  # Set link options
  if(CMAKE_VERSION VERSION_GREATER_EQUAL 3.13)
    target_link_options(${target_name} PUBLIC ${public_link_options})
  endif()

  # Set compile features
  target_compile_features(${target_name} PUBLIC ${public_compile_features})

  # Set compile options
  target_compile_options(${target_name} PUBLIC ${public_compile_options})

  # Set compile definitions
  target_compile_definitions(
    ${target_name} PUBLIC ${public_compile_definitions}
  )

  # Format files
  dart_clang_format_add_sources(${headers} ${sources})

  # Generate meta header, all.hpp
  if(dart_add_module_GENERATE_META_HEADER)
    dart_generate_meta_header(
      DESTINATION "all.hpp" HEADERS ${headers}
      PREFIX_TO_REMOVE "${project_source_dir}/src/" OUTPUT_VAR dart_all_header
    )
  endif()

  # Install meta header
  install(
    FILES ${dart_all_header}
    DESTINATION
      include/${PROJECT_NAME}${DART_VERSION_MAJOR}/${PROJECT_NAME}/${module_name}
  )

  # Install targets
  # cmake-format: off
  install(
    TARGETS ${target_name}
    EXPORT ${target_name}-targets
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  )
  # cmake-format: on
  install(
    EXPORT ${target_name}-targets
    FILE ${target_name}-targets.cmake
    DESTINATION
      ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}${DART_VERSION_MAJOR}
  )

  # Register module
  set_property(
    GLOBAL APPEND PROPERTY DART_CPP_BUILDING_MODULES "${module_name}"
  )

endfunction()

# ==============================================================================
# cmake-format: off
# dart_build_target_in_source(target
#   [LINK_LIBRARIES library1 ...])
#   [COMPILE_FEATURES feature1 ...]
#   [COMPILE_OPTIONS option1 ...]
# )
# cmake-format: on
function(dart_build_target_in_source target)
  set(prefix example)
  set(options)
  set(oneValueArgs)
  set(multiValueArgs LINK_LIBRARIES COMPILE_FEATURES COMPILE_OPTIONS)
  cmake_parse_arguments(
    "${prefix}" "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN}
  )

  if(example_LINK_LIBRARIES)
    foreach(dep_target ${example_LINK_LIBRARIES})
      if(NOT TARGET ${dep_target})
        if(DART_VERBOSE)
          message(
            WARNING
              "Skipping ${target} because required target '${dep_target}' not found"
          )
        endif()
        return()
      endif()
    endforeach()
  endif()

  file(GLOB srcs "*.cpp" "*.hpp")

  add_executable(${target} ${srcs})

  if(example_LINK_LIBRARIES)
    foreach(dep_target ${example_LINK_LIBRARIES})
      target_link_libraries(${target} ${dep_target})
    endforeach()
  endif()

  if(example_COMPILE_FEATURES)
    foreach(comple_feature ${example_COMPILE_FEATURES})
      target_compile_features(${target} PUBLIC ${comple_feature})
    endforeach()
  endif()

  if(example_COMPILE_OPTIONS)
    foreach(comple_option ${example_COMPILE_OPTIONS})
      target_compile_options(${target} PUBLIC ${comple_option})
    endforeach()
  endif()

  set_target_properties(
    ${target} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
  )

  dart_clang_format_add_sources(${srcs})
endfunction()

# ==============================================================================
function(dart_add_example)
  dart_property_add(DART_EXAMPLES ${ARGN})
endfunction(dart_add_example)

# ==============================================================================
# cmake-format: off
# dart_build_example_in_source(target
#   [LINK_LIBRARIES library1 ...])
#   [COMPILE_FEATURES feature1 ...]
#   [COMPILE_OPTIONS option1 ...]
# )
# cmake-format: on
function(dart_build_example_in_source target)
  dart_build_target_in_source(${target} ${ARGN})
  dart_add_example(${target})
endfunction()

# ==============================================================================
function(dart_add_tutorial)
  dart_property_add(DART_TUTORIALS ${ARGN})
endfunction(dart_add_tutorial)

# ==============================================================================
# cmake-format: off
# dart_build_tutorial_in_source(target
#   [LINK_LIBRARIES library1 ...])
#   [COMPILE_FEATURES feature1 ...]
#   [COMPILE_OPTIONS option1 ...]
# )
# cmake-format: on
function(dart_build_tutorial_in_source target)
  dart_build_target_in_source(${target} ${ARGN})
  dart_add_tutorial(${target})
endfunction()
