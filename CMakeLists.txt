cmake_minimum_required(VERSION 3.13 FATAL_ERROR)

if(NOT TARGET score_lib_base)
  include("${SCORE_SOURCE_DIR}/cmake/ScoreExternalAddon.cmake")
else()
  if(NOT TARGET score_plugin_avnd)
    return()
  endif()
endif()

project(score_addon_bendage LANGUAGES CXX)

add_library(
  score_addon_bendage
  Bendage/JiPeg.hpp
  Bendage/JiPeg.cpp
  Bendage/Xlippy.hpp
  Bendage/Xlippy.cpp
  Bendage/SafeWord.hpp
  Bendage/SafeWord.cpp

  score_addon_bendage.hpp
  score_addon_bendage.cpp
)

set_target_properties(score_addon_bendage PROPERTIES SCORE_CUSTOM_PCH 1)
setup_score_plugin(score_addon_bendage)

target_link_libraries(score_addon_bendage PUBLIC score_plugin_engine score_plugin_avnd)

target_compile_options(score_addon_bendage PRIVATE -Ofast -march=native)
