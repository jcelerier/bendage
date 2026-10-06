cmake_minimum_required(VERSION 3.13 FATAL_ERROR)

if(NOT TARGET score_lib_base)
  include(ScoreExternalAddon)
endif()

if(NOT TARGET score_plugin_avnd)
  return()
endif()

project(score_addon_bendage LANGUAGES CXX)

avnd_score_plugin_init(
  BASE_TARGET score_addon_bendage
)

# The libjpeg that Qt uses: the SDK's, or the system one. A second copy would
# put two sets of jpeg_* symbols in a static build.
find_package(JPEG)
if(TARGET JPEG::JPEG)
  avnd_score_plugin_add(
    BASE_TARGET score_addon_bendage
    SOURCES
      Bendage/JiPeg.hpp
      Bendage/JiPeg.cpp
    TARGET JiPeg
    MAIN_CLASS JiPeg
    NAMESPACE Bendage
  )
  target_link_libraries(score_addon_bendage PRIVATE JPEG::JPEG)

  if(COMMAND score_register_license)
    list(GET JPEG_INCLUDE_DIRS 0 _jpeg_include)
    set(_jpeg_doc "${_jpeg_include}/../share/doc/libjpeg-turbo")
    if(EXISTS "${_jpeg_doc}/LICENSE.md")
      score_register_license(libjpeg-turbo URL "https://libjpeg-turbo.org"
        FILES "${_jpeg_doc}/LICENSE.md" "${_jpeg_doc}/README.ijg")
    else()
      score_register_license(libjpeg-turbo URL "https://libjpeg-turbo.org"
        HEADER "IJG License, Modified BSD License, zlib License")
    endif()
  endif()
endif()

avnd_score_plugin_add(
  BASE_TARGET score_addon_bendage
  SOURCES
    Bendage/Xlippy.hpp
    Bendage/Xlippy.cpp
  TARGET Xlippy
  MAIN_CLASS Xlippy
  NAMESPACE Bendage
)

avnd_score_plugin_add(
  BASE_TARGET score_addon_bendage
  SOURCES
    Bendage/SafeWord.hpp
    Bendage/SafeWord.cpp
  TARGET SafeWord
  MAIN_CLASS SafeWord
  NAMESPACE Bendage
)

avnd_score_plugin_finalize(
  BASE_TARGET score_addon_bendage
  PLUGIN_VERSION 1
  PLUGIN_UUID "04205ca6-5fe6-4f7e-b9ce-c09b32a85baa"
)

score_optimize_in_debug_mode(${PROJECT_NAME})

if(SCORE_TESTING AND COMMAND score_add_test AND TARGET JPEG::JPEG)
  score_add_test(score_addon_bendage_buffers_test
    SOURCES
      tests/BendageBuffersTest.cpp
      Bendage/JiPeg.cpp
      Bendage/Xlippy.cpp
      Bendage/SafeWord.cpp
  )
  target_include_directories(score_addon_bendage_buffers_test PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${SCORE_ROOT_SOURCE_DIR}/3rdparty/avendish/include")
  target_link_libraries(score_addon_bendage_buffers_test PRIVATE JPEG::JPEG)
endif()
