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

    ./build/rpi-finedust -p <device> -b <rate>

The program opens a serial device, reads PMS7003 frames, and prints one
decoded frame per received packet. Press Ctrl-C to stop.

Required options:

    -p, --port <device>   Serial device (e.g. /dev/ttyAMA0)
    -b, --baud <rate>     Baud rate: 1200, 2400, 4800, 9600, 19200, 38400,
                          57600, 115200

Other options:

    -o, --output <file>   Append decoded frames to a binary log file
                          (32-byte records, see tools/visualize.py)
    -h, --help            Show usage information and exit

Examples:

    ./build/rpi-finedust -p /dev/ttyAMA0 -b 9600
    ./build/rpi-finedust --port /dev/ttyUSB0 --baud 9600
    ./build/rpi-finedust -p /dev/ttyAMA0 -b 9600 -o run.pmslog
    ./build/rpi-finedust --help


Tools
-----

Helper scripts for the binary log live under `tools/`.

tools/visualize.py reads a log produced with `-o` and plots PM1.0/PM2.5/PM10
(CF=1, atm) and particle counts over time.

Install dependencies (Raspberry Pi OS / Debian Bookworm blocks system-wide
`pip install` per PEP 668, so pick one):

    # apt (simplest)
    sudo apt install python3-numpy python3-matplotlib

    # venv (isolated)
    sudo apt install python3-venv python3-full
    python3 -m venv .venv
    .venv/bin/pip install -r tools/requirements.txt

Usage:

    # show plot interactively
    python3 tools/visualize.py run.pmslog

    # save plot to a PNG instead of showing
    python3 tools/visualize.py run.pmslog -o run.png

    # print summary only
    python3 tools/visualize.py run.pmslog --no-plot


Project layout
--------------

    include/pms7003.hpp   PMS7003 protocol constants, data struct, API
    include/utils.hpp     Byte-order helpers
    src/main.cpp          Serial I/O loop and pretty-printing
    src/pms7003.cpp       Header check, checksum, frame unpacking
    src/utils.cpp         read_u16_be implementation
    tools/visualize.py    Plot/summarise binary logs produced with -o
    tools/requirements.txt Python dependencies for visualize.py
    CMakeLists.txt        Build configuration


License
-------

See the LICENSE file.
