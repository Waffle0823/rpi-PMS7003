#include "pms7003.hpp"
#include <algorithm>
#include <cstring>
#include <fcntl.h>
#include <iomanip>
#include <iostream>
#include <termios.h>
#include <unistd.h>

void print_pms_data(const PMS7003Data &data) {
  using std::cout;
  using std::endl;

  cout << std::hex << std::setw(2) << std::setfill('0');
  cout << "Header: 0x" << static_cast<int>(data.header_high) << " 0x"
       << static_cast<int>(data.header_low) << endl;

  cout << std::dec << std::setfill(' ');
  cout << "Frame length: " << data.frame_length << " bytes" << endl;

  cout << "PM CF=1 (ug/m3) - PM1.0: " << data.pm1_0_cf1
       << ", PM2.5: " << data.pm2_5_cf1 << ", PM10: " << data.pm10_cf1 << endl;

  cout << "PM ATM (ug/m3) - PM1.0: " << data.pm1_0_atm
       << ", PM2.5: " << data.pm2_5_atm << ", PM10: " << data.pm10_atm << endl;

  cout << "Particle counts (/0.1L) - 0.3um: " << data.air_0_3
       << ", 0.5um: " << data.air_0_5 << ", 1.0um: " << data.air_1_0
       << ", 2.5um: " << data.air_2_5 << ", 5.0um: " << data.air_5_0
       << ", 10um: " << data.air_10_0 << endl;

  cout << "Reserved: " << data.reserved << endl;

  cout << std::hex << std::setw(4) << std::setfill('0');
  cout << "Checksum: 0x" << data.checksum << endl;
  cout << std::dec << std::setfill(' ');
}

void start(const std::string &port, const int speed) {
  int fd = open(port.c_str(), O_RDWR | O_NOCTTY | O_SYNC);
  if (fd < 0) {
    std::cerr << "Failed to open serial port" << std::endl;
    return;
  }

  struct termios tty;
  memset(&tty, 0, sizeof tty);

  if (tcgetattr(fd, &tty) != 0) {
    std::cerr << "tcgetattr failed\n";
    return;
  }

  cfsetospeed(&tty, speed);
  cfsetispeed(&tty, speed);

  tty.c_cflag &= ~PARENB; // no parity
  tty.c_cflag &= ~CSTOPB; // 1 stop bit
  tty.c_cflag &= ~CSIZE;
  tty.c_cflag |= CS8; // 8 bits

  tty.c_cflag &= ~CRTSCTS; // no flow control
  tty.c_cflag |= CREAD | CLOCAL;

  tty.c_lflag &= ~ICANON; // raw mode
  tty.c_lflag &= ~ECHO;
  tty.c_lflag &= ~ECHOE;
  tty.c_lflag &= ~ISIG;

  tty.c_iflag &= ~(IXON | IXOFF | IXANY); // no software flow control
  tty.c_oflag &= ~OPOST;

  tty.c_cc[VMIN] = 0;
  tty.c_cc[VTIME] = 10;

  if (tcsetattr(fd, TCSANOW, &tty) != 0) {
    std::cerr << "tcsetattr failed\n";
    return;
  }

  unsigned char buf[256];

  while (true) {
    int n = read(fd, buf, sizeof(buf));
    if (n > 0) {
      std::cout << "Received " << n << " bytes: ";

      if (n > static_cast<int>(PMS7003_PROTOCOL_SIZE)) {
        std::cerr << "Discarding frame larger than PMS7003 protocol (" << n
                  << " bytes)" << std::endl;
        continue;
      }

      Buffer buffer{};
      std::copy_n(buf, n, buffer.begin());

      if (n < static_cast<int>(PMS7003_PROTOCOL_SIZE)) {
        std::cerr << "Incomplete frame (" << n
                  << " bytes), waiting for complete packet" << std::endl;
        continue;
      }

      if (!check_header(buffer)) {
        std::cerr << "Invalid PMS7003 frame header" << std::endl;
        continue;
      }

      if (!check_checksum(buffer)) {
        std::cerr << "Checksum mismatch for PMS7003 frame" << std::endl;
        continue;
      }

      const auto data = unpack_data(buffer);
      print_pms_data(data);
    }
  }
}

int main() {
  start("/dev/ttyS0", B9600);
  return 0;
}
