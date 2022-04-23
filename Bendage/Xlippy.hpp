#pragma once

#include <halp/controls.hpp>
#include <halp/meta.hpp>
#include <halp/texture.hpp>
#include <boost/container/vector.hpp>

namespace Bendage
{

class Xlippy
{
public:
  halp_meta(name, "Xlippy")
  halp_meta(category, "Bendage")
  halp_meta(c_name, "xlippy")
  halp_meta(uuid, "12326964-0bd9-4d03-b685-49fe8b7f288c")
  //struct ui;

  struct ins
  {
    halp::rgb_texture_input<"Input"> tex;
    halp::toggle<"Letter"> space;
    halp::toggle<"Show tip again"> tips;
    halp::toggle<"Burn"> burn;
    halp::slider_t<int, "Assistance", halp::range{0, 255, 100}> assist;
    halp::slider_t<int, "Annoyance", halp::range{0, 255, 100}> annoy;
  } inputs;

  struct outs
  {
    halp::texture_output<"Output"> tex;
  } outputs;

  Xlippy();

  void operator()();

private:
  boost::container::vector<unsigned char> bytes;
};

}
