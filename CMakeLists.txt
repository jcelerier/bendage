cmake_minimum_required(VERSION 3.13 FATAL_ERROR)

if(NOT TARGET score_lib_base)
  include(ScoreExternalAddon)
endif()

if(NOT TARGET score_plugin_avnd)
  return()
endif()

if(EMSCRIPTEN)
  return()
endif()

project(score_addon_bendage LANGUAGES CXX)

avnd_score_plugin_init(
  BASE_TARGET score_addon_bendage
)

set_target_properties(score_addon_bendage PROPERTIES SCORE_CUSTOM_PCH 1)
target_compile_options(score_addon_bendage PRIVATE -Ofast -march=native)

avnd_score_plugin_add(
  BASE_TARGET score_addon_bendage
  SOURCES
    Bendage/JiPeg.hpp
    Bendage/JiPeg.cpp
  TARGET JiPeg
  MAIN_CLASS JiPeg
  NAMESPACE Bendage
)

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

# setup_score_plugin(score_addon_bendage)
#
# target_link_libraries(score_addon_bendage PUBLIC score_plugin_engine score_plugin_avnd)
#
