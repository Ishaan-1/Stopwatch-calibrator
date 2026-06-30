# STM32 Blue Pill Stopwatch

A precision stopwatch built on the STM32F103C8 (Blue Pill) with a 1.3" SH1106 SPI OLED display.

## Features
- H:MM:SS.mmm display format (millisecond precision)
- Start / Stop / Reset / Lap via UART serial commands from laptop
- Physical push buttons for standalone battery-powered use
- Drift correction for crystal accuracy (~46 ppm measured)
- Python control script with live laptop-side timer for drift comparison

## Hardware
- STM32F103C8 Blue Pill
- 1.3" SH1106 SPI OLED (128x64)
- CH340 UART-to-TTL programmer
- 1S LiPo + TP4056 charging module

## Serial Commands
| Command | Action |
|---------|--------|
| `s` | Start / Stop |
| `r` | Reset |
| `l` | Lap |

## Wiring
| OLED Pin | Blue Pill Pin |
|----------|---------------|
| SCK | PA5 |
| SDA | PA7 |
| RES | PA2 |
| DC  | PA3 |
| CS  | PA4 |

## Built With
- STM32CubeIDE
- afiskon/stm32-ssd1306 library
- Python + pyserial
