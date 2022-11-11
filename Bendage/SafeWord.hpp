#pragma once

#include <boost/container/vector.hpp>
#include <halp/controls.hpp>
#include <halp/meta.hpp>
#include <halp/texture.hpp>

namespace Bendage
{

class SafeWord
{
public:
  halp_meta(name, "Safe Word")
  halp_meta(category, "Visuals/Bendage")
  halp_meta(c_name, "safeword")
  halp_meta(author, "Jean-Michaël Celerier")
  halp_meta(description, "Do not forget to use them")
  halp_meta(uuid, "cb158d1d-5d2b-4b1a-a85c-79dcf2aabb6d")
  //struct ui;

  struct ins
  {
    halp::rgb_texture_input<"Input"> tex;
    halp::lineedit<"Letter", "strawberry"> word;
    struct
    {
      halp__enum(
          "Fetish", Algolagnia, Algolagnia, Apotemno, Dacry, Emeto, Klisma, Myso,
          Odaxela, Pique, Sopho, Tricho)
    } fetish;
    halp::hslider_i32<"HARDER", halp::range{0, 5, 1}> harder;
  } inputs;

  struct outs
  {
    halp::texture_output<"Output"> tex;
  } outputs;

  SafeWord();

  void operator()();

private:
  boost::container::vector<unsigned char> bytes;
};

}
