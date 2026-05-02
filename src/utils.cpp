#include "utils.hpp"
#include "pms7003.hpp"
#include <cstdint>

uint16_t read_u16_be(const Buffer &buffer, size_t i) {
  return (static_cast<uint16_t>(buffer[i]) << 8) |
         static_cast<uint16_t>(buffer[i + 1]);
}
