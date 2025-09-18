#pragma once

#include "jpeg_decoder.h"

#include <boost/container/vector.hpp>
#include <halp/controls.hpp>
#include <halp/meta.hpp>
#include <halp/texture.hpp>

#include <optional>
namespace Bendage
{

class JiPeg
{
public:
  halp_meta(name, "JPeg")
  halp_meta(category, "Visuals/Bendage")
  halp_meta(c_name, "j_peg")
  halp_meta(author, "Jean-Michaël Celerier")
  halp_meta(
      description,
      "Hardcore jpegging. Uses Stephan Brumme's JPEG encoder, and Scott Graham's JPEG "
      "decoder, \nitself based on KeyJ's Tiny Baseline JPEG Decoder by Martin J. "
      "Fiedler for maximum jpegging.")
  halp_meta(uuid, "4c3b207c-4f3c-4b63-93d7-58e531dd3528")
  //struct ui;

  struct ins
  {
    halp::rgb_texture_input<"Input"> tex;
    halp::knob_f32<"Peggage", halp::range{.min = 0., .max = 100., .init = 90.}> quality;
  } inputs;

  struct outs
  {
    halp::rgb_texture_output<"Output"> tex;
  } outputs;

  JiPeg();

  void operator()();

private:
  boost::container::vector<unsigned char> rgb;
  boost::container::vector<unsigned char> bytes;

  std::optional<Jpeg::Decoder> decoder;

  int current_byte = 0;
};

}

#include <halp/layout.hpp>
/*
namespace Bendage
{
struct JiPeg::ui
{
  using enum halp::colors;
  using enum halp::layouts;

  halp_meta(name, "Main")
  halp_meta(layout, vbox)
  halp_meta(background, background_dark)

  halp::label title{"JPeg"};
  halp::item<&ins::quality> quality;
};
}
*/
