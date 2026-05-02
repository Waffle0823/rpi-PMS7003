#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

uint16_t read_u16_be(const std::array<uint8_t, 32> &buf, size_t i);
