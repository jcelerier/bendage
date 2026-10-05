#include "JiPeg.hpp"

#include "toojpeg.h"

#include <algorithm>
#include <cstring>

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
  // JPEG stores its dimensions on 16 bits.
  if(in_tex.width > 65535 || in_tex.height > 65535)
    return;

  const int w = in_tex.width;
  const int h = in_tex.height;
  const std::size_t texture_bytesize = std::size_t(w) * h * 3;

  // At high quality, noise encodes larger than the raw image:
  // the write callback grows the buffer as needed.
  if(bytes.size() < texture_bytesize + 64)
    bytes.resize(texture_bytesize + 64, boost::container::default_init);

  auto wr = [](void* self, unsigned char c) {
    auto& s = *(JiPeg*)self;
    if(s.current_byte >= s.bytes.size())
      s.bytes.resize(s.bytes.size() * 2, boost::container::default_init);
    s.bytes[s.current_byte++] = c;
  };

  const float peggage = std::clamp(inputs.quality.value, 0.f, 100.f);
  current_byte = 0;
  if(!TooJpeg::writeJpeg(
         this, wr, in_tex.bytes, w, h, true, (unsigned char)(100.f - peggage), true))
    return;

  decoder.reset();
  decoder.emplace(bytes.data(), current_byte);
  const bool ok = decoder->GetResult() == Jpeg::Decoder::OK && decoder->IsColor()
                  && decoder->GetWidth() == w && decoder->GetHeight() == h;
  if(ok)
  {
    outputs.tex.create(w, h);
    std::memcpy(out_tex.bytes, decoder->GetImage(), texture_bytesize);
  }
  decoder.reset();
  if(!ok)
    return;

  in_tex.changed = false;
  out_tex.changed = true;
}
}

#include "toojpeg.cpp"
