#include "JiPeg.hpp"

#include "toojpeg.h"

#include <QDebug>
namespace Bendage
{
JiPeg::JiPeg() { }

void JiPeg::operator()()
{
  auto& in_tex = inputs.tex.texture;
  auto& out_tex = outputs.tex.texture;

  if(in_tex.bytes == nullptr)
    return;
  if(!in_tex.changed)
    return;
  if(in_tex.width < 16 || in_tex.height < 16)
    return;

  auto rgb_source = in_tex.bytes;

  int texture_bytesize = in_tex.width * in_tex.height * 3;
  bytes.resize(in_tex.width * in_tex.height * 3 + 64, boost::container::default_init);

  auto wr = [](void* self, unsigned char c) {
    auto& s = *(JiPeg*)self;
    s.bytes[s.current_byte] = c;
    s.current_byte++;
  };

  {
    current_byte = 0;

    // Encode rgb_source to JPEG, into the bytes array
    TooJpeg::writeJpeg(
        this, wr, rgb_source, in_tex.width, in_tex.height, true, 100. - inputs.quality,
        true);
    // Decode bytes and replace rgb_source with the new pointer.
    decoder.reset();
    decoder.emplace(bytes.data(), texture_bytesize);
    if(decoder->GetResult() != Jpeg::Decoder::OK)
      return;

    rgb_source = decoder->GetImage();
  }

  out_tex.width = in_tex.width;
  out_tex.height = in_tex.height;
  out_tex.bytes = rgb_source;

  in_tex.changed = false;
  out_tex.changed = true;
}
}

#include "toojpeg.cpp"
