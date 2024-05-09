#include "color.hpp"

#include "bits-and-bytes/overflow_arithmetic/overflow_arithmetic.hpp"

#include <limits>

bool libconfigfile::color::rgb_model::operator==(const rgb &color_1,
                                                 const rgb &color_2) {
  return ((color_1.r == color_2.r) && (color_1.g == color_2.g) &&
          (color_1.b == color_2.b));
}

bool libconfigfile::color::rgb_model::operator==(const rgba &color_1,
                                                 const rgba &color_2) {
  return ((color_1.a == color_2.a) &&
          (static_cast<rgb>(color_1) == static_cast<rgb>(color_2)));
}

libconfigfile::color::rgb libconfigfile::color::operator+(const rgb &color_1,
                                                          const rgb &color_2) {
  return rgb{.r{impl::add_channels(color_1.r, color_2.r)},
             .g{impl::add_channels(color_1.g, color_2.g)},
             .b{impl::add_channels(color_1.b, color_2.b)}};
}

libconfigfile::color::rgba
libconfigfile::color::operator+(const rgba &color_1, const rgba &color_2) {
  return rgba{(convert<rgb>(color_1) + convert<rgb>(color_2)),
              (impl::add_channels(color_1.a, color_2.a))};
}

libconfigfile::color::channel_t
libconfigfile::color::impl::add_channels(const channel_t channel_1,
                                         const channel_t channel_2) {
  auto [value, overflow]{
      bits_and_bytes::overflow_arithmetic::add(channel_1, channel_2)};
  return ((overflow) ? (std::numeric_limits<channel_t>::max()) : (value));
}
