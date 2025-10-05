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

score_optimize_in_debug_mode(${PROJECT_NAME})
