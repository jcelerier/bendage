#include "SafeWord.hpp"
#include <QDebug>
#include <cmath>
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

  int N = in_tex.width * in_tex.height * 4;

  outputs.tex.create(in_tex.width, in_tex.height);
  if(inputs.word.value.size() > 0)
  {
    auto next = [&w=inputs.word, k=0] () mutable { return w.value[k++ % w.value.size()]; };
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
            out[i] = in[i] % (int)(next() + 1);
          break;
        case 5:
          for(int i = 0; i < N; i++)
            out[i] = in[(next() * i) % N];
          break;
        case 6:
          for(int i = 0; i < N; i++)
            out[i] = in[(next() ^ i) % N];
          break;
        case 7:
          for(int i = 0; i < N; i++)
            out[i] = in[(next() * i) % N] ^ i;
          break;
        case 8:
          for(int i = 0; i < N; i++)
            out[i] = in[i] ^ (next() * i);
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
