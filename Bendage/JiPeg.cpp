#include "JiPeg.hpp"
#include "jpeg_decoder.h"
#include "toojpeg.h"
#include <iostream>
namespace Bendage
{
JiPeg::JiPeg()
{

}

void JiPeg::operator()()
{
  auto& in_tex = inputs.tex.texture;
  auto& out_tex = outputs.tex.texture;
  if (in_tex.bytes == nullptr)
    return;
  if (!in_tex.changed)
    return;

  rgb.clear();
  rgb.resize(in_tex.width * in_tex.height * 3, boost::container::default_init);

  auto rgb_source = rgb.data();
  {
    // RGBA -> RGB for the input
    int bts = in_tex.width * in_tex.height * 4;
    for(int rgb_i = 0, rgba_i = 0; rgba_i < bts; )
    {
      rgb_source[rgb_i++] = in_tex.bytes[rgba_i++];
      rgb_source[rgb_i++] = in_tex.bytes[rgba_i++];
      rgb_source[rgb_i++] = in_tex.bytes[rgba_i++];
      rgba_i++;
    }
  }

  int texture_bytesize = in_tex.width * in_tex.height * 3;
  bytes.clear();
  bytes.reserve(in_tex.width * in_tex.height * 3);

  auto wr = [] (void* self, unsigned char c) {
    auto& s = *(JiPeg*) self;

    s.bytes.push_back(c);
  };

  //for(int i = 0; i < 5; i++)
  {
    // To JPEG
    TooJpeg::writeJpeg(this, wr,
                       rgb_source,
                       in_tex.width,
                       in_tex.height,
                       true,
                       inputs.quality,
                       true
    );


    // From JPEG
    Jpeg::Decoder decoder(bytes.data(), texture_bytesize);
    if(decoder.GetResult() != Jpeg::Decoder::OK)
      return;

    bytes.clear();
    rgb_source = decoder.GetImage();
  }

  outputs.tex.create(in_tex.width, in_tex.height);

  {
    // RGB -> RGBA for the output
    auto res = rgb_source;
    for(int rgb_i = 0, rgba_i = 0; rgb_i < texture_bytesize; )
    {
      out_tex.bytes[rgba_i++] = res[rgb_i++];
      out_tex.bytes[rgba_i++] = res[rgb_i++];
      out_tex.bytes[rgba_i++] = res[rgb_i++];
      out_tex.bytes[rgba_i++] = 255;
    }
  }

  in_tex.changed = false;
  out_tex.changed = true;
}
}



#include "toojpeg.cpp"
