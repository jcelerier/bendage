#pragma once

#include <halp/controls.hpp>
#include <halp/meta.hpp>
#include <halp/texture.hpp>

#include <memory>

namespace Bendage
{

class JiPeg
{
public:
  halp_meta(name, "JPeg")
  halp_meta(category, "Visuals/Bendage")
  halp_meta(c_name, "j_peg")
  halp_meta(author, "Jean-Michaël Celerier")
  halp_meta(description, "Hardcore jpegging.")
  halp_meta(uuid, "4c3b207c-4f3c-4b63-93d7-58e531dd3528")

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
  ~JiPeg();

  void operator()();

private:
  struct Codec;
  std::unique_ptr<Codec> codec;
};

}
