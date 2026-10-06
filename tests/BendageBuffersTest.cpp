// The bendage nodes fed with adversarial images: every output buffer must
// match the advertised width * height * 3 and be readable to its end.
// Meant to be run under AddressSanitizer as well as in the normal test suite.

#include <Bendage/JiPeg.hpp>
#include <Bendage/SafeWord.hpp>
#include <Bendage/Xlippy.hpp>

#include <catch2/catch_test_macros.hpp>

#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <memory>
#include <random>
#include <string>
#include <vector>

namespace
{
std::vector<unsigned char> filled(int w, int h, unsigned char value)
{
  return std::vector<unsigned char>(std::size_t(w) * h * 3, value);
}

std::vector<unsigned char> noise(int w, int h, unsigned seed, int max = 255)
{
  std::mt19937 rng{seed};
  std::uniform_int_distribution<int> dist{0, max};
  std::vector<unsigned char> img(std::size_t(w) * h * 3);
  for(auto& b : img)
    b = (unsigned char)dist(rng);
  return img;
}

template <typename Node>
void feed(Node& node, std::vector<unsigned char>& img, int w, int h)
{
  auto& t = node.inputs.tex.texture;
  t.bytes = img.data();
  t.width = w;
  t.height = h;
  t.changed = true;
}

// Reads the whole output the way the host does when uploading it.
template <typename Node>
std::uint64_t consume(Node& node, int w, int h)
{
  auto& t = node.outputs.tex.texture;
  REQUIRE(t.bytes != nullptr);
  REQUIRE(t.width == w);
  REQUIRE(t.height == h);
  // volatile so that the read is not optimized away
  const volatile unsigned char* bytes = t.bytes;
  std::uint64_t sum = 0;
  for(std::size_t i = 0, n = std::size_t(w) * h * 3; i < n; i++)
    sum += bytes[i];
  return sum;
}
}

TEST_CASE("Xlippy: an image of line feeds with tips doubles the stream", "[bendage][xlippy]")
{
  for(auto [w, h] : {std::pair{64, 64}, {17, 3}, {1, 1}})
  {
    auto img = filled(w, h, 0x0A);
    Bendage::Xlippy node;
    node.inputs.tips.value = true;
    node.inputs.annoy.value = 0;
    node.inputs.assist.value = 0;
    feed(node, img, w, h);
    node();
    consume(node, w, h);

    // Each interior 0x0A becomes 0A 0D; the output keeps the first N bytes.
    const auto* out = node.outputs.tex.texture.bytes;
    if(w * h * 3 >= 5)
    {
      CHECK(out[0] == 0x0A);
      CHECK(out[1] == 0x0A);
      CHECK(out[2] == 0x0D);
      CHECK(out[3] == 0x0A);
      CHECK(out[4] == 0x0D);
    }
  }
}

TEST_CASE("Xlippy: an image of carriage returns with burn doubles the stream", "[bendage][xlippy]")
{
  auto img = filled(64, 64, 0x0D);
  Bendage::Xlippy node;
  node.inputs.burn.value = true;
  node.inputs.annoy.value = 0;
  node.inputs.assist.value = 0;
  feed(node, img, 64, 64);
  node();
  consume(node, 64, 64);
  const auto* out = node.outputs.tex.texture.bytes;
  CHECK(out[0] == 0x0D);
  CHECK(out[1] == 0x0A);
  CHECK(out[2] == 0x0D);
}

// cpu-data-bending sweeps Annoyance with an LFO: b & (annoy * b) maps
// ordinary pixel values to 0x0A, here 0x0B with an annoyance of 254.
TEST_CASE("Xlippy: an annoyance that makes line feeds doubles the stream", "[bendage][xlippy]")
{
  auto img = filled(64, 64, 0x0B);
  Bendage::Xlippy node;
  node.inputs.tips.value = true;
  node.inputs.annoy.value = 254;
  node.inputs.assist.value = 80;
  feed(node, img, 64, 64);
  node();
  consume(node, 64, 64);
  const auto* out = node.outputs.tex.texture.bytes;
  CHECK(out[1] == 0x0A);
  CHECK(out[2] == 0x0D);
}

TEST_CASE("Xlippy: dark noise with every option on", "[bendage][xlippy]")
{
  for(int annoy : {0, 1, 3, 100, 255})
  {
    auto img = noise(320, 240, 1234 + annoy, 15);
    Bendage::Xlippy node;
    node.inputs.tips.value = true;
    node.inputs.burn.value = true;
    node.inputs.space.value = true;
    node.inputs.annoy.value = annoy;
    feed(node, img, 320, 240);
    node();
    consume(node, 320, 240);
  }
}

TEST_CASE("Xlippy: an empty texture is ignored", "[bendage][xlippy]")
{
  std::vector<unsigned char> img(3);
  Bendage::Xlippy node;
  feed(node, img, 0, 0);
  node();
  CHECK(node.outputs.tex.texture.bytes == nullptr);
}

TEST_CASE("JiPeg: noise at full quality on odd sizes", "[bendage][jipeg]")
{
  for(auto [w, h] : {std::pair{640, 480}, {17, 17}, {33, 19}, {101, 77}, {16, 255}})
  {
    auto img = noise(w, h, w * 31 + h);
    auto node = std::make_unique<Bendage::JiPeg>();
    node->inputs.quality.value = 0.f;
    feed(*node, img, w, h);
    (*node)();
    consume(*node, w, h);
  }
}

TEST_CASE("JiPeg: every quality on noise", "[bendage][jipeg]")
{
  auto img = noise(64, 48, 99);
  auto node = std::make_unique<Bendage::JiPeg>();
  for(float q = 0.f; q <= 100.f; q += 12.5f)
  {
    node->inputs.quality.value = q;
    feed(*node, img, 64, 48);
    (*node)();
    consume(*node, 64, 48);
  }
}

TEST_CASE("JiPeg: frames that grow encode past the first buffer", "[bendage][jipeg]")
{
  auto node = std::make_unique<Bendage::JiPeg>();
  node->inputs.quality.value = 0.f;
  for(auto [w, h] : {std::pair{32, 32}, {640, 480}, {64, 48}, {1280, 720}})
  {
    auto img = noise(w, h, w + h);
    feed(*node, img, w, h);
    (*node)();
    consume(*node, w, h);
  }
}

TEST_CASE("JiPeg: a smooth image survives full quality", "[bendage][jipeg]")
{
  const int w = 96, h = 64;
  std::vector<unsigned char> img(w * h * 3);
  for(int y = 0; y < h; y++)
    for(int x = 0; x < w; x++)
    {
      auto* p = &img[(y * w + x) * 3];
      p[0] = (unsigned char)(x * 2);
      p[1] = (unsigned char)(y * 3);
      p[2] = 128;
    }
  auto node = std::make_unique<Bendage::JiPeg>();
  node->inputs.quality.value = 0.f;
  feed(*node, img, w, h);
  (*node)();
  consume(*node, w, h);

  const auto* out = node->outputs.tex.texture.bytes;
  long err = 0;
  for(int i = 0; i < w * h * 3; i++)
    err += std::abs(int(out[i]) - int(img[i]));
  CHECK(err / (w * h * 3) < 4);
}

TEST_CASE("JiPeg: a texture wider than a JPEG can hold", "[bendage][jipeg]")
{
  // JPEG dimensions are 16-bit: 65552 would be encoded as 16.
  const int w = 65552, h = 16;
  auto img = filled(w, h, 0x40);
  auto node = std::make_unique<Bendage::JiPeg>();
  feed(*node, img, w, h);
  (*node)();
  auto& t = node->outputs.tex.texture;
  if(t.bytes)
    consume(*node, t.width, t.height);
}

TEST_CASE("SafeWord: high bytes in the word as an index", "[bendage][safeword]")
{
  for(int fetish : {5, 6, 7})
  {
    auto img = noise(128, 96, fetish);
    Bendage::SafeWord node;
    node.inputs.word.value = "str\xc3\xa9wberry";
    node.inputs.fetish.value = decltype(node.inputs.fetish.value)(fetish);
    feed(node, img, 128, 96);
    node();
    consume(node, 128, 96);
  }
}

TEST_CASE("SafeWord: high bytes in the word with every fetish", "[bendage][safeword]")
{
  for(std::string word : {std::string("\xff\x80\xfe\x81"), std::string("\xff"),
                          std::string("str\xc3\xa9wberry"), std::string("strawberry")})
    for(int fetish = 0; fetish <= 10; fetish++)
      for(int harder : {0, 2})
      {
        auto img = noise(128, 96, fetish);
        Bendage::SafeWord node;
        node.inputs.word.value = word;
        node.inputs.fetish.value = decltype(node.inputs.fetish.value)(fetish);
        node.inputs.harder.value = harder;
        feed(node, img, 128, 96);
        node();
        consume(node, 128, 96);
      }
}

TEST_CASE("SafeWord: index products past INT_MAX on a large image", "[bendage][safeword]")
{
  // 4096 * 2048 * 3 * 'z' > INT_MAX
  const int w = 4096, h = 2048;
  auto img = noise(w, h, 7);
  for(int fetish : {5, 7, 8})
  {
    Bendage::SafeWord node;
    node.inputs.word.value = "zzzz";
    node.inputs.fetish.value = decltype(node.inputs.fetish.value)(fetish);
    node.inputs.harder.value = 0;
    feed(node, img, w, h);
    node();
    consume(node, w, h);
  }
}

TEST_CASE("SafeWord: an empty word passes the image through", "[bendage][safeword]")
{
  auto img = noise(32, 32, 3);
  Bendage::SafeWord node;
  node.inputs.word.value = "";
  feed(node, img, 32, 32);
  node();
  consume(node, 32, 32);
  CHECK(std::equal(img.begin(), img.end(), node.outputs.tex.texture.bytes));
}
