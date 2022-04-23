#include "Xlippy.hpp"

namespace Bendage
{
Xlippy::Xlippy()
{

}
// http://blog.animalswithinanimals.com/2010/09/advanced-wordpad-editing.html :
// I've found that Wordpad does at least two things to a binary file;
// it replaces byte 07 (ascii: BEEP) with a space ,
// and it replaces every lonely 0A or 0D (line feed and carriage return respectively)
// and also 0B (vertical tab) with the bytes "0A 0D". So the rate of glitching is probably dependent on how dark the picture is, since low bytes like these give dark pixels (i suppose).
void Xlippy::operator()()
{
  auto& in_tex = inputs.tex.texture;
  auto& out_tex = outputs.tex.texture;
  if (in_tex.bytes == nullptr)
    return;
  if (!in_tex.changed)
    return;

  int N = in_tex.width * in_tex.height * 4;
  bytes.clear();
  bytes.reserve(N * 1.25);
  bytes.push_back(in_tex.bytes[0]);
  for(int i = 1; i < N - 1; i++)
  {
    unsigned char b = in_tex.bytes[i] | inputs.annoy;
    switch(b)
    {
      case 0x07:
        bytes.push_back(inputs.space ? ' ' : 0x07);
        break;
      case 0x0A:
      {
        bytes.push_back(0x0A);
        if(inputs.tips && in_tex.bytes[i + 1] != 0x0D)
        {
          bytes.push_back(0x0D);
        }
        break;
      }
      case 0x0D:
      {
        if(inputs.burn && in_tex.bytes[i - 1] != 0x0A)
        {
          bytes.push_back(0x0A);
        }
        bytes.push_back(0x0D);
        break;
      }
      default:
        bytes.push_back(inputs.assist == 0 ? b : b ^ inputs.assist);
        break;
    }
  }

  bytes.push_back(in_tex.bytes[N - 1]);

  outputs.tex.create(in_tex.width, in_tex.height);
  memcpy(out_tex.bytes, bytes.data(), N);
  in_tex.changed = false;
  out_tex.changed = true;
}
}
