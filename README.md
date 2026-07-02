# Telemetry Tool

This repository contains an STM32L4 firmware project and a Python-based telemetry UI for registering devices and decoding telemetry packets.

## Project Structure

- `Core/` - STM32Cube project sources
- `Drivers/` - HAL and CMSIS drivers
- `gemini-code-1782748779266.py` - Original Python telemetry UI script
- `gemini-code-1782748779266 - Copy.py` - Backup/experimental Python telemetry UI script

## STM32 Firmware Usage

### Build and flash
1. Open the project in STM32CubeIDE or use your existing build system.
2. Configure the target to the STM32L4 board.
3. Build and flash the firmware.

### Telemetry behavior
- The MCU waits for a boot sync byte `0xAA` from the host before registering devices.
- Device registrations are sent using commands with `0x55` sync bytes.
- Each telemetry packet from MCU to PC uses `0xAA` as the data sync byte and includes:
  - `information_type`
  - `information_id`
  - `information_len`
  - payload
  - 16-bit CRC

### String and byte-array payloads
- For string payloads, use `information_type = 0x05`.
- For byte-array payloads, use `information_type = 0x02`.
- For a string literal declared as an array, use `sizeof(sensor_text) - 1` for the length.

## Python UI Usage

### Running
1. Install required Python packages: `pip install pyserial`
2. Run the UI script: `python "gemini-code-1782748776.py"`
3. Ensure the selected COM port matches the STM32 target.

### Features
- Device registration table with ID, name, format, and handshake status
- Parsed telemetry output panel
- Raw serial data panel
- Controls for disconnect, restart, and resync
- Backup copy includes additional log and JSON persistence helpers

### Log and JSON support
- The UI allows selecting a log path and saving raw/parsed output to disk
- Device registration metadata can be exported to a JSON file

## Notes
- Do not change source files if you are only updating usage documentation.
- Keep command IDs and telemetry IDs aligned between MCU and Python host.
- Use the original Python script for stable behavior and the backup script for experimental logging/UI work.
