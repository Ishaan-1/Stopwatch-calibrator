# STM32 Blue Pill Stopwatch

A precision stopwatch built on the STM32F103C8 (Blue Pill), with two display versions: an OLED build and a TFT build.

## Versions

### `oled_version/` — SH1106 OLED Display
- 1.3" SH1106 SPI OLED (128x64)
- UART serial control from laptop (Python script)
- Physical push buttons for standalone battery-powered use
- H:MM:SS.mmm display format (millisecond precision)
- Drift correction for crystal accuracy (~31 ppm measured)

### `tft_version/` — ILI9341 TFT Display
- 2.4" ILI9341 SPI TFT (240x320, landscape mode)
- Interrupt-driven (EXTI) button handling — Start/Stop and Reset
- Larger, more readable display with colour support
- Battery powered via 1S LiPo + TP4056

## Features (both versions)
- Start / Stop / Reset / Lap functionality
- Millisecond precision timing using `HAL_GetTick()`
- Battery powered, standalone operation

## Hardware

### OLED Version
| Component | Spec |
|---|---|
| MCU | STM32F103C8 Blue Pill |
| Display | 1.3" SH1106 SPI OLED |
| Programmer | CH340 UART-to-TTL |
| Power | 1S LiPo + TP4056 |

### TFT Version
| Component | Spec |
|---|---|
| MCU | STM32F103C8 Blue Pill |
| Display | 2.4" ILI9341 SPI TFT |
| Buttons | 2x tactile push buttons (interrupt-driven) |
| Power | 1S LiPo + TP4056 |

## Serial Commands (OLED version only)
| Command | Action |
|---------|--------|
| `s` | Start / Stop |
| `r` | Reset |
| `l` | Lap |

## Built With
- STM32CubeIDE
- [afiskon/stm32-ssd1306](https://github.com/afiskon/stm32-ssd1306) (OLED version)
- [martnak/STM32-ILI9341](https://github.com/martnak/STM32-ILI9341) (TFT version)
- Python + pyserial (OLED version control script)

## Accuracy
Measured crystal drift on the OLED version: ~31 ppm over a 6 hour test run, translating to roughly 112ms drift per hour. Software drift correction applied in firmware.

## Roadmap
- [ ] Rubidium oscillator integration for sub-ppb timing accuracy
- [ ] Battery level indicator on TFT display
