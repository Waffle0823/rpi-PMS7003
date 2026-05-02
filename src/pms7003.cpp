#include "pms7003.hpp"
#include "utils.hpp"

bool check_header(const Buffer &buffer) {
  return buffer[static_cast<int>(PMSIndex::HEADER_HIGH)] == 0x42 &&
         buffer[static_cast<int>(PMSIndex::HEADER_LOW)] == 0x4D;
}

bool check_checksum(const Buffer &buffer) {
  uint16_t calculated_checksum = 0;

  for (int i = 0; i < PMS7003_PROTOCOL_SIZE - 2; i++) {
    calculated_checksum += buffer[i];
  }

  uint16_t received_checksum = ((buffer[30]) << 8) | (buffer[31]);

  if (calculated_checksum == received_checksum) {
    return true;
  } else {
    return false;
  }
}

PMS7003Data unpack_data(const Buffer &buffer) {
  PMS7003Data d;

  d.header_high = buffer[0];
  d.header_low = buffer[1];

  d.frame_length = read_u16_be(buffer, 2);

  d.pm1_0_cf1 = read_u16_be(buffer, 4);
  d.pm2_5_cf1 = read_u16_be(buffer, 6);
  d.pm10_cf1 = read_u16_be(buffer, 8);

  d.pm1_0_atm = read_u16_be(buffer, 10);
  d.pm2_5_atm = read_u16_be(buffer, 12);
  d.pm10_atm = read_u16_be(buffer, 14);

  d.air_0_3 = read_u16_be(buffer, 16);
  d.air_0_5 = read_u16_be(buffer, 18);
  d.air_1_0 = read_u16_be(buffer, 20);
  d.air_2_5 = read_u16_be(buffer, 22);
  d.air_5_0 = read_u16_be(buffer, 24);
  d.air_10_0 = read_u16_be(buffer, 26);

  d.reserved = read_u16_be(buffer, 28);

  d.checksum = read_u16_be(buffer, 30);

  return d;
}
