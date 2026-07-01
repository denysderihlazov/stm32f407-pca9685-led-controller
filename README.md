# STM32F407 PCA9685 LED Controller

This project controls the **PCA9685 16-channel PWM LED controller** from an **STM32F407VGT6** using I2C.

The PCA9685 driver is implemented as a separate library and can be tested from a PC using a UART console.

## Hardware

* **MCU:** STM32F407VGT6
* **Board:** STM32F407 Discovery
* **Shield:** GlobalLogic GL board
* **PWM controller:** PCA9685PW
* **USB-UART:** CH340 3.3V TTL
* **Logic analyzer:** Kingst LA2016

## Interfaces

### I2C1 — PCA9685

| STM32 Pin | Function    |
| --------- | ----------- |
| PB6       | I2C1_SCL    |
| PB9       | I2C1_SDA    |
| PB7       | PWM_EN / OE |

PCA9685 address:

```text
0x40
```

I2C speed:

```text
50 kHz
```

### USART3 — PC console

| STM32 Pin | Function  | USB-UART |
| --------- | --------- | -------- |
| PB10      | USART3_TX | RXD      |
| PB11      | USART3_RX | TXD      |
| GND       | GND       | GND      |

UART settings:

```text
115200 8N1
No flow control
```

## Project Structure

```text
Core/                 STM32 application code
Drivers/              STM32 HAL/CMSIS drivers
Libs/PCA9685/Inc/     PCA9685 public header
Libs/PCA9685/Src/     PCA9685 driver source
Workshop5.ioc         STM32CubeMX configuration
```

## PCA9685 API

```c
HAL_StatusTypeDef PCA9685_Init(I2C_HandleTypeDef *hi2c);

HAL_StatusTypeDef PCA9685_SetPWMFreq(uint16_t freq_hz);
HAL_StatusTypeDef PCA9685_SetRaw(uint8_t channel, uint16_t on, uint16_t off);
HAL_StatusTypeDef PCA9685_SetDuty(uint8_t channel, uint8_t duty_percent);

HAL_StatusTypeDef PCA9685_AllOff(void);
HAL_StatusTypeDef PCA9685_AllOn(void);

HAL_StatusTypeDef PCA9685_Sleep(void);
HAL_StatusTypeDef PCA9685_Wakeup(void);

HAL_StatusTypeDef PCA9685_EnableOutputs(void);
HAL_StatusTypeDef PCA9685_DisableOutputs(void);
```

## UART Commands

| Command | Action                                          |
| ------- | ----------------------------------------------- |
| `h`     | Print help                                      |
| `o`     | Enable PCA9685 outputs                          |
| `x`     | Disable PCA9685 outputs                         |
| `t`     | Test 4 channels: 25%, 50%, 75%, 100%            |
| `f`     | Set PWM frequency test: 50 Hz / 100 Hz / 200 Hz |
| `a`     | Turn all channels off                           |
| `b`     | Turn all channels on                            |
| `s`     | Enable sleep mode                               |
| `w`     | Wake up                                         |
| `d`     | Fade demo on channel 0                          |

Example terminal output:

```text
STM32F407 PCA9685 LED Controller
PCA9685 init OK

Commands:
h - help
o - enable PCA9685 outputs
x - disable PCA9685 outputs
t - test 4 channels: 25%, 50%, 75%, 100%
a - all channels off
b - all channels on
s - sleep mode
w - wakeup
d - fade demo on channel 0
f - set PWM frequency test: 50 Hz / 100 Hz / 200 Hz
```

## Logic Analyzer Verification

Signals are checked using **Kingst LA2016**.

### I2C

| LA Channel | Signal         |
| ---------- | -------------- |
| CH0        | PB6 / I2C1_SCL |
| CH1        | PB9 / I2C1_SDA |
| GND        | GND            |

Expected result:

```text
PCA9685 address: 0x40
Write frame: 0x80 + ACK
Read frame:  0x81 + ACK
SCL frequency: about 50 kHz
```

### PWM Outputs

| LA Channel | PCA9685 Output |
| ---------- | -------------- |
| CH0        | LED0           |
| CH1        | LED1           |
| CH2        | LED2           |
| CH3        | LED3           |
| GND        | GND            |

After sending the UART command:

```text
t
```

expected duty cycles are:

| Channel | Duty |
| ------- | ---- |
| LED0    | 25%  |
| LED1    | 50%  |
| LED2    | 75%  |
| LED3    | 100% |

## Logic Analyzer Files

The traces are recorded with **Kingst LA2016**.

Native KingstVIS format:

```text
*.kvdat
```

CSV exports are also provided when needed.

Recommended directory:

```text
docs/logic_analyzer/
```

Suggested files:

```text
pca9685_i2c_50khz.kvdat
pca9685_i2c_50khz.csv
pca9685_pwm_outputs.kvdat
pca9685_pwm_outputs.csv
```

## Status

Implemented:

* PCA9685 driver in separate `.c/.h` files
* I2C1 communication at 50 kHz
* PWM duty cycle control
* PWM frequency configuration
* output enable/disable
* sleep/wakeup mode
* UART command interface
* basic logic analyzer verification
* GitHub repository

Remaining:

* add final PWM logic analyzer captures
* add screenshots
* add video link

## Demo Videos

### I2C Communication Verification

Video link:

[STM32F407 PCA9685 LED Controller via I2C](https://youtu.be/0U33TPNM2v8)

This video demonstrates:

* STM32F407 communication with the PCA9685 over I2C1
* PCA9685 initialization
* UART console control through RealTerm
* Logic analyzer verification of I2C frames
* PCA9685 address `0x40`
* I2C SCL frequency around `50 kHz`

### PCA9685 Output Verification

Video link:

[STM32F407 PCA9685 PWM Output Verification with Kingst LA2016](https://youtu.be/opkb-P3YmcQ)

This video demonstrates:

* UART commands used to control the PCA9685
* `w` command for wakeup
* `o` command for output enable
* `t` command for setting test PWM values
* Attempt to verify PCA9685 output signals using Kingst LA2016
* Physical probing of PCA9685 output path according to the shield schematic

The first four PCA9685 output channels are routed according to the schematic as:

| PCA9685 Channel | Series Resistor | Connector Output |
| --------------- | --------------- | ---------------- |
| LED0            | R42             | P2 pin 1         |
| LED1            | R43             | P2 pin 2         |
| LED2            | R44             | P2 pin 3         |
| LED3            | R45             | P2 pin 4         |