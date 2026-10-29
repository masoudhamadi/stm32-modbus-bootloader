# STM32 Modbus Bootloader

A Modbus RTU bootloader for STM32 microcontrollers, designed to support firmware programming and updates over a standard serial communication interface.

This project aims to provide a practical solution for updating STM32 firmware through Modbus RTU, making it suitable for embedded systems, industrial controllers, and automation equipment.

## Overview

Firmware updates are an important part of maintaining embedded devices, especially in industrial applications where accessing the microcontroller directly may be inconvenient.

This project provides a bootloader-based approach to firmware programming over Modbus RTU. It is intended to work with a host-side programming tool that communicates with the target device and transfers firmware through the serial interface.

## Features

- Firmware programming over Modbus RTU.
- Serial communication suitable for UART/RS-485-based systems.
- Integration with a PC-based firmware programming tool.
- Designed for STM32 embedded applications.
- Suitable as a starting point for firmware update systems in industrial equipment.

> The supported STM32 models, communication settings, memory layout, and programming limitations depend on the current implementation and target hardware.

## Project Components

The firmware and host-side programming utility serve different roles:

- **Bootloader:** Runs on the STM32 target and handles firmware programming requests.
- **PC Programmer:** Provides the host-side interface for communicating with the target and transferring firmware.

The PC-side utility is maintained in a separate repository:

[STM32 Modbus Bootloader — GitHub](https://github.com/masoudhamadi/stm32-modbus-bootloader)

## Typical Application

A typical firmware update workflow consists of:

1. Connecting the PC to the target device through a supported serial interface.
2. Starting the programming utility.
3. Selecting the firmware image.
4. Establishing communication with the STM32 bootloader.
5. Transferring the firmware according to the implemented protocol.
6. Completing the programming process and restarting the target, as supported by the firmware.

The exact workflow depends on the bootloader implementation and the corresponding PC programmer.

## Hardware and Development Environment

This project targets STM32 microcontrollers.

Before building or deploying the firmware, verify the following:

- The exact STM32 MCU and board configuration.
- The UART and, where applicable, RS-485 interface configuration.
- The bootloader and application flash memory layout.
- The application start address and vector table configuration.
- The communication parameters required by the implementation.

Incorrect flash addresses or incompatible application linker settings may prevent the application from starting correctly.

## Getting Started

1. Design Your PCB

Design your PCB with the required STM32 connections:

USART3 TX on PC10

USART3 RX on PC11

Download control on PC13

Ensure that the MCU, power supply, clock configuration, reset circuitry, and serial communication interface are correctly implemented.

2. Program the Bootloader

Program the provided bootloader firmware into the STM32 using an ST-Link or another supported initial programming method.

This is the initial installation step required before using the PC programming utility.

Make sure the bootloader is programmed to the correct flash address for your target configuration.

3. Connect the PC Programmer

Connect your PC to the target board through the supported serial communication interface.

Launch the provided PC programming software and configure the communication parameters required by the bootloader.

4. Upload Your Firmware

Select your application firmware in the PC programming utility and start the programming process.

The utility communicates with the bootloader using Modbus RTU and transfers the firmware to the target device.

Follow the application's instructions and wait for the programming process to finish before disconnecting power or communication.

5. Update Firmware in the Future

After the initial bootloader installation, use the PC programming utility for subsequent firmware updates through the supported communication interface, without requiring a separate ST-Link programming operation for every update.

Refer to the source code and project configuration for implementation-specific details.

## Important Notes

- Confirm that the firmware image matches the target MCU and application memory layout.
- Do not interrupt power or communication during firmware programming unless the implementation explicitly supports recovery from interrupted updates.
- Test the bootloader and firmware update procedure on development hardware before deploying it to production equipment.
- A bootloader should not be considered secure against unauthorized firmware installation unless suitable authentication and integrity protections have been implemented and verified.

## Contributing

Suggestions, bug reports, and improvements are welcome.

If you find an issue or would like to improve the project, please open a GitHub Issue or submit a Pull Request.

## License

See the `LICENSE` file in this repository for the applicable license terms.

## Author

**Masoud Hamadi**

GitHub: [@masoudhamadi](https://github.com/masoudhamadi)
