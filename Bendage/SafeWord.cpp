#include "SafeWord.hpp"

#include <cstdint>
#include <cstring>
namespace Bendage
{
SafeWord::SafeWord()
{

}

void SafeWord::operator()()
{
  auto& in_tex = inputs.tex.texture;
  auto& out_tex = outputs.tex.texture;
  if (in_tex.bytes == nullptr)
    return;
  if (!in_tex.changed)
    return;

  const int N = in_tex.width * in_tex.height * 3;
  if(N <= 0)
    return;

  outputs.tex.create(in_tex.width, in_tex.height);
  if(inputs.word.value.empty())
  {
    std::memcpy(out_tex.bytes, in_tex.bytes, N);
  }
  else
  {
    // Letters are signed whatever the platform's char is, as on x86.
    auto next = [&w = inputs.word, k = std::size_t(0)]() mutable -> int {
      return (signed char)w.value[k++ % w.value.size()];
    };
    // Indices are computed on 64 bits then wrapped into [0, N):
    // a negative letter reads backwards from the end of the image.
    auto at = [N](std::int64_t index) {
      index %= N;
      return index < 0 ? index + N : index;
    };
    unsigned char* in = in_tex.bytes;
    unsigned char* out = out_tex.bytes;
    for(int iter = inputs.harder + 1; iter --> 0; )
    {
      switch((int)inputs.fetish)
      {
        case 0:
          for(int i = 0; i < N; i++)
            out[i] = in[i] ^ next();
          break;
        case 1:
          for(int i = 0; i < N; i++)
            out[i] = in[i] | next();
          break;
        case 2:
          for(int i = 0; i < N; i++)
            out[i] = in[i] + next();
          break;
        case 3:
          for(int i = 0; i < N; i++)
            out[i] = in[i] * next();
          break;
        case 4:
          for(int i = 0; i < N; i++)
          {
            // A letter of -1 would divide by zero: the divisor wraps to 256.
            const int d = next() + 1;
            out[i] = d == 0 ? in[i] : in[i] % d;
          }
          break;
        case 5:
          for(int i = 0; i < N; i++)
            out[i] = in[at(std::int64_t(next()) * i)];
          break;
        case 6:
          for(int i = 0; i < N; i++)
            out[i] = in[at(next() ^ i)];
          break;
        case 7:
          for(int i = 0; i < N; i++)
            out[i] = in[at(std::int64_t(next()) * i)] ^ i;
          break;
        case 8:
          for(int i = 0; i < N; i++)
            out[i] = in[i] ^ (unsigned(next()) * unsigned(i));
          break;
        case 9:
          for(int i = 0; i < N; i++)
            out[i] = in[i] ^ (next() | i);
          break;
        case 10:
          for(int i = 0; i < N; i++)
            out[i] = in[i] ^ (next() & i);
          break;
      }
      in = out;
    }
  }


  in_tex.changed = false;
  out_tex.changed = true;
}
}
