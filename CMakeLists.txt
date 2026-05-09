cmake_minimum_required(VERSION 3.23)

# Set a name and a version number for your project:
project(bit_chess_cpp
  VERSION 0.0.1
  LANGUAGES CXX
)

# Initialize some default paths
include(GNUInstallDirs)

# Define build options
option(bit_chess_cpp_BUILD_TESTING "Enable building of tests" OFF)

if(PROJECT_IS_TOP_LEVEL)
  option(bit_chess_cpp_BUILD_DOCS "Enable building of documentation" ON)
else()
  option(bit_chess_cpp_BUILD_DOCS "Enable building of documentation" OFF)
endif()

# Add an interface target for the header-only library
add_library(bit_chess_cpp INTERFACE)

# Add the public headers that belong to this library
target_sources(bit_chess_cpp
  INTERFACE
    FILE_SET HEADERS
    BASE_DIRS include
    FILES
      include/bit_chess_cpp/bit_chess_cpp.hpp
)

# Request a minimum C++ standard for this target
target_compile_features(bit_chess_cpp
  INTERFACE
    cxx_std_23
)

if(PROJECT_IS_TOP_LEVEL)
  # Compile the application executable
  add_subdirectory(app)
endif()

if(PROJECT_IS_TOP_LEVEL)
  # Compile the tests
  include(CTest)
  if(bit_chess_cpp_BUILD_TESTING)
    add_subdirectory(tests)
  endif()
endif()

# Add the documentation
if(PROJECT_IS_TOP_LEVEL AND bit_chess_cpp_BUILD_DOCS)
  add_subdirectory(doc)
endif()

# Add an alias target for use if this project is included as a subproject in another project
add_library(bit_chess_cpp::bit_chess_cpp ALIAS bit_chess_cpp)

# Install the target and headers
install(
  TARGETS bit_chess_cpp
  EXPORT bit_chess_cppTargets
  FILE_SET HEADERS
)

# Install the exported targets (this creates bit_chess_cppTargets.cmake)
install(
  EXPORT bit_chess_cppTargets
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/bit_chess_cpp
  NAMESPACE bit_chess_cpp::
)

# Include helpers for calling configure_package_config_file
include(CMakePackageConfigHelpers)

# Generate the bit_chess_cppConfig.cmake file
configure_package_config_file(
  ${CMAKE_CURRENT_LIST_DIR}/cmake/bit_chess_cppConfig.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/bit_chess_cppConfig.cmake
  INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/bit_chess_cpp
)

# Install the generated Config file
install(
  FILES
    ${CMAKE_CURRENT_BINARY_DIR}/bit_chess_cppConfig.cmake
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/bit_chess_cpp
)

# Generate a version file
write_basic_package_version_file(
  ${CMAKE_CURRENT_BINARY_DIR}/bit_chess_cppConfigVersion.cmake
  VERSION ${PROJECT_VERSION}
  COMPATIBILITY SameMajorVersion
)

# Install the version file
install(
  FILES
    ${CMAKE_CURRENT_BINARY_DIR}/bit_chess_cppConfigVersion.cmake
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/bit_chess_cpp
)

# This prints a summary of found dependencies
include(FeatureSummary)
feature_summary(WHAT ALL)
