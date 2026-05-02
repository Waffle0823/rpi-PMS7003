rpi-fine-dust
=============

A small C++17 program for the Raspberry Pi that reads air-quality data from a
Plantower PMS7003 particulate-matter sensor over the UART serial port and
prints each decoded frame to standard output.


Features
--------

- Reads 32-byte PMS7003 active-mode frames from a serial port.
- Synchronises to the 0x42 0x4D frame header and validates the frame length.
- Verifies the frame checksum before decoding.
- Decodes and prints:
    * PM1.0 / PM2.5 / PM10 concentrations (CF=1, standard particle)
    * PM1.0 / PM2.5 / PM10 concentrations (atmospheric environment)
    * Particle counts per 0.1 L of air for 0.3, 0.5, 1.0, 2.5, 5.0, 10 um
    * Reserved field and checksum


Hardware
--------

- Raspberry Pi (any model with an accessible UART).
- Plantower PMS7003 sensor.

Default wiring (the program opens /dev/ttyAMA0 at 9600 baud, 8N1):

    PMS7003 VCC  -> Pi 5V
    PMS7003 GND  -> Pi GND
    PMS7003 TX   -> Pi RXD (GPIO15, pin 10)
    PMS7003 RX   -> Pi TXD (GPIO14, pin  8)

On Raspberry Pi OS you typically need to:

    1. Enable the serial hardware and disable the serial login shell via
       `sudo raspi-config` -> Interface Options -> Serial Port.
    2. Make sure your user is in the `dialout` group, or run the binary with
       sufficient permissions to access /dev/ttyAMA0.


Requirements
------------

- CMake >= 3.14
- A C++17-capable compiler (g++ or clang++)
- Linux with termios (tested on Raspberry Pi OS)


Build
-----

From the project root:

    cmake -S . -B build
    cmake --build build

The resulting executable is `build/rpi-finedust`.


Run
---

    ./build/rpi-finedust

The program opens /dev/ttyAMA0 at 9600 baud and prints one decoded frame per
received packet. Press Ctrl-C to stop.

To use a different serial device or baud rate, edit the call to `start(...)`
in `src/main.cpp` (e.g. "/dev/serial0" or "/dev/ttyUSB0").


Project layout
--------------

    include/pms7003.hpp   PMS7003 protocol constants, data struct, API
    include/utils.hpp     Byte-order helpers
    src/main.cpp          Serial I/O loop and pretty-printing
    src/pms7003.cpp       Header check, checksum, frame unpacking
    src/utils.cpp         read_u16_be implementation
    CMakeLists.txt        Build configuration


License
-------

See the LICENSE file.
