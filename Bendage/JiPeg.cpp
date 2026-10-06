#include "JiPeg.hpp"

#include <jpeglib.h>

#include <algorithm>
#include <csetjmp>
#include <cstdio>
#include <cstdlib>

namespace Bendage
{
namespace
{
// libjpeg reports errors by calling error_exit, which must not return: it
// jumps back to the setjmp of the call that failed. Nothing with a destructor
// may live between the two, hence the C-style functions below.
struct error_handler
{
  jpeg_error_mgr mgr;
  std::jmp_buf jump;
};

void on_error(j_common_ptr cinfo)
{
  std::longjmp(reinterpret_cast<error_handler*>(cinfo->err)->jump, 1);
}

void on_message(j_common_ptr) { }
}

struct JiPeg::Codec
{
  jpeg_compress_struct encoder{};
  jpeg_decompress_struct decoder{};
  error_handler encoder_error{};
  error_handler decoder_error{};

  // malloc'd: jpeg_mem_dest moves a frame that does not fit to a larger
  // malloc'd buffer of its own, which then replaces this one.
  unsigned char* buffer{};
  unsigned char* previous{};
  unsigned long buffer_size{};

  Codec()
  {
    encoder.err = jpeg_std_error(&encoder_error.mgr);
    encoder_error.mgr.error_exit = on_error;
    encoder_error.mgr.output_message = on_message;
    jpeg_create_compress(&encoder);

    decoder.err = jpeg_std_error(&decoder_error.mgr);
    decoder_error.mgr.error_exit = on_error;
    decoder_error.mgr.output_message = on_message;
    jpeg_create_decompress(&decoder);
  }

  ~Codec()
  {
    jpeg_destroy_compress(&encoder);
    jpeg_destroy_decompress(&decoder);
    std::free(buffer);
  }

  // Returns the size of the encoded frame, 0 on failure.
  unsigned long encode(const unsigned char* rgb, int w, int h, int quality)
  {
    // Allocated here, libjpeg would free it itself when growing it.
    if(!buffer)
    {
      buffer_size = std::size_t(w) * h * 3 + 1024;
      buffer = static_cast<unsigned char*>(std::malloc(buffer_size));
      if(!buffer)
        return 0;
    }
    previous = buffer;
    if(setjmp(encoder_error.jump))
    {
      jpeg_abort_compress(&encoder);
      return 0;
    }

    jpeg_mem_dest(&encoder, &buffer, &buffer_size);
    encoder.image_width = w;
    encoder.image_height = h;
    encoder.input_components = 3;
    encoder.in_color_space = JCS_RGB;
    jpeg_set_defaults(&encoder);
    jpeg_set_quality(&encoder, quality, TRUE);
    jpeg_start_compress(&encoder, TRUE);
    while(encoder.next_scanline < encoder.image_height)
    {
      auto row = const_cast<JSAMPROW>(rgb) + std::size_t(encoder.next_scanline) * w * 3;
      jpeg_write_scanlines(&encoder, &row, 1);
    }
    // Sets buffer and buffer_size to the frame
    jpeg_finish_compress(&encoder);

    if(previous != buffer)
      std::free(previous);
    return buffer_size;
  }

  bool decode(unsigned long size, unsigned char* rgb, int w, int h)
  {
    if(setjmp(decoder_error.jump))
    {
      jpeg_abort_decompress(&decoder);
      return false;
    }

    jpeg_mem_src(&decoder, buffer, size);
    if(jpeg_read_header(&decoder, TRUE) != JPEG_HEADER_OK)
    {
      jpeg_abort_decompress(&decoder);
      return false;
    }
    decoder.out_color_space = JCS_RGB;
    jpeg_start_decompress(&decoder);
    if(int(decoder.output_width) != w || int(decoder.output_height) != h
       || decoder.output_components != 3)
    {
      jpeg_abort_decompress(&decoder);
      return false;
    }
    while(decoder.output_scanline < decoder.output_height)
    {
      JSAMPROW row = rgb + std::size_t(decoder.output_scanline) * w * 3;
      jpeg_read_scanlines(&decoder, &row, 1);
    }
    jpeg_finish_decompress(&decoder);
    return true;
  }
};

JiPeg::JiPeg()
    : codec{std::make_unique<Codec>()}
{
}

JiPeg::~JiPeg() = default;

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
  if(in_tex.width > JPEG_MAX_DIMENSION || in_tex.height > JPEG_MAX_DIMENSION)
    return;

  const int w = in_tex.width;
  const int h = in_tex.height;

  const float peggage = std::clamp(inputs.quality.value, 0.f, 100.f);
  const int quality = std::max(1, int(100.f - peggage));
  const auto size = codec->encode(in_tex.bytes, w, h, quality);
  if(size == 0)
    return;

  outputs.tex.create(w, h);
  if(!codec->decode(size, out_tex.bytes, w, h))
    return;

  in_tex.changed = false;
  out_tex.changed = true;
}
}
