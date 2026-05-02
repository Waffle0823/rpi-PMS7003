#pragma once

#include <array>
#include <cstdint>

constexpr std::size_t PMS7003_PROTOCOL_SIZE = 32;

using Buffer = std::array<uint8_t, PMS7003_PROTOCOL_SIZE>;

enum class PMSIndex : uint8_t {
  HEADER_HIGH = 0,
  HEADER_LOW,

  FRAME_LENGTH_HIGH,
  FRAME_LENGTH_LOW,

  // CF=1 (standard particle)
  PM1_0_CF1_HIGH,
  PM1_0_CF1_LOW,
  PM2_5_CF1_HIGH,
  PM2_5_CF1_LOW,
  PM10_CF1_HIGH,
  PM10_CF1_LOW,

  // Atmospheric environment
  PM1_0_ATM_HIGH,
  PM1_0_ATM_LOW,
  PM2_5_ATM_HIGH,
  PM2_5_ATM_LOW,
  PM10_ATM_HIGH,
  PM10_ATM_LOW,

  // Particle counts (per 0.1L air)
  DUST_0_3_HIGH,
  DUST_0_3_LOW,
  DUST_0_5_HIGH,
  DUST_0_5_LOW,
  DUST_1_0_HIGH,
  DUST_1_0_LOW,
  DUST_2_5_HIGH,
  DUST_2_5_LOW,
  DUST_5_0_HIGH,
  DUST_5_0_LOW,
  DUST_10_0_HIGH,
  DUST_10_0_LOW,

  // Reserved
  RESERVED_HIGH,
  RESERVED_LOW,

  // Checksum (sum of bytes 0~29)
  CHECKSUM_HIGH,
  CHECKSUM_LOW
};

struct PMS7003Data {
  uint8_t header_high;
  uint8_t header_low;

  uint16_t frame_length;

  uint16_t pm1_0_cf1;
  uint16_t pm2_5_cf1;
  uint16_t pm10_cf1;

  uint16_t pm1_0_atm;
  uint16_t pm2_5_atm;
  uint16_t pm10_atm;

  uint16_t air_0_3;
  uint16_t air_0_5;
  uint16_t air_1_0;
  uint16_t air_2_5;
  uint16_t air_5_0;
  uint16_t air_10_0;

  uint16_t reserved;

  uint16_t checksum;
};

bool check_header(const Buffer &buffer);

bool check_checksum(const Buffer &buffer);

PMS7003Data unpack_data(const Buffer &buffer);
