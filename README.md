# giulietta-radionav-retrofit

## Alfa Romeo Giulietta RadioNav Motorized Screen Mechanism Control Project 🇮🇹

[![License: GPL-3.0](https://img.shields.io/badge/License-GPL--3.0-blue.svg)](LICENSE)
[![GitHub Star Count](https://img.shields.io/github/stars/yasinerarslan/giulietta-radionav-retrofit?style=social)](https://github.com/yasinerarslan/giulietta-radionav-retrofit/stargazers)

This repository provides the microcontroller code and hardware architecture required for the original **RadioNav** motorized screen mechanism found in Alfa Romeo Giulietta MK1 vehicles to function correctly with a modern aftermarket multimedia system. The project aims to maintain the physical control and position memory of the mechanism.

---

## 🏗️ Architecture and Components

The system implements **closed-loop position control** using a microcontroller, a motor driver, and the vehicle's existing sensors/buttons.

### ⚙️ Technical Components

| Component | Model/Type | Function |
| :--- | :--- | :--- |
| **Microcontroller** | Arduino Nano | Executes the control logic and stores the position in EEPROM. |
| **Motor Driver** | TB6612FNG | Drives the DC motor using Arduino signals. |
| **Memory** | EEPROM | Permanently saves the screen's last adjusted angle. |
| **Sensor** | Potentiometer | Reads the instantaneous screen angle as an analog value. |

### 📌 Pin Configuration (`radionav.ino`)

The table below specifies the connections between the Arduino Nano, the TB6612FNG driver, and the RadioNav system's physical interface.

| Code Constant | Arduino Pin | TB6612FNG Pin | Type | Description |
| :--- | :--- | :--- | :--- | :--- |
| `DRV_STBY` | `6` | **STBY** | Output | Motor driver standby control. |
| `DRV_AIN1` | `7` | **AIN1** | Output | Motor Direction 1 (Forward/Reverse). |
| `DRV_AIN2` | `8` | **AIN2** | Output | Motor Direction 2 (Forward/Reverse). |
| `DRV_PWMA` | `9` | **PWMA** | PWM Output | Motor Speed/Power (PWM). |
| `POT_PIN` | `A0` | N/A | Analog Input | Screen angle potentiometer reading. |
| `VOLTAGE_TRIGGER` | `13` | N/A | Digital Input | System trigger from vehicle ignition (ACC). |
| `BTN_OPEN` | `3` | N/A | Digital Input (PULLUP) | Open/Close button. |
| `BTN_ANG_UP` | `4` | N/A | Digital Input (PULLUP) | Angle Increase button. |
| `BTN_ANG_DOWN` | `5` | N/A | Digital Input (PULLUP) | Angle Decrease button. |

---

## 💻 Software and Setup

### 1. Code Compilation and Upload

1.  Ensure the Arduino IDE is installed.
2.  Clone the repository and open the `radionav.ino` file.
3.  Select the **Arduino Nano** board and upload the code. (The included `EEPROM.h` library does not require external installation.)

### 2. Critical Position Values and Parameters

Motor control relies on analog values (0-1023) read from the potentiometer. These values are calibrated for the mechanism and should be adjusted if necessary:

| Parameter | Default Value | Description |
| :--- | :--- | :--- |
| `CLOSE_POT_POS` | `850` | The physical fully **closed** position of the screen. |
| `MIDDLE_POT_POS` | `400` | The approximate midpoint threshold of the motion range. |
| `OPEN_POT_POS` | `60` | The physical fully **open** position of the screen. |
| `SENSETIVE` | `20` | Tolerance/Error margin for reaching the target position. |
| `CHANGE_ANGLE` | `30` | The step size for position changes when angle buttons are pressed. |

### 3. Control Functions

* `moveToMotor(direction)`: Runs the motor in the desired direction (open/close) at a defined PWM speed.
    * **Open Direction (`ON_SCR`):** PWM 50.
    * **Close Direction (`CLOSE_SCR`):** PWM 60.
* `moveToAngle(direction, targetPosition)`: Runs the motor until the instantaneous potentiometer reading is within the tolerance (`SENSETIVE`) of the `targetPosition`.
* `EEPROMSave`/`EEPROMLoad`: Saves and loads the screen's last position to a 2-byte EEPROM address.

---

## 🔄 Operation Protocol

1.  **System Power-On:** When the `VOLTAGE_TRIGGER` pin transitions from `LOW` to `HIGH` (Ignition ON), the screen moves to the last stored position (`SAVED_POT_POS`).
2.  **System Power-Off:** When the `VOLTAGE_TRIGGER` pin transitions from `HIGH` to `LOW` (Ignition OFF), the screen moves to the fully closed position (`CLOSE_POT_POS`).
3.  **Manual Control:** The `BTN_ANG_UP` and `BTN_ANG_DOWN` buttons adjust the screen angle by `CHANGE_ANGLE` units and save the new position to **EEPROM**.
4.  **Quick Toggle:** The `BTN_OPEN` button toggles the screen between the stored open position (`SAVED_POT_POS`) and the fully closed position (`CLOSE_POT_POS`).

---

## 🛠 Hardware Integration

### Wiring and Power Supply

* **Motor Connection:** The RadioNav motor must be connected to the **Motor A Output** (`AOUT1` and `AOUT2`) on the TB6612FNG.
* **Voltage Levels:** The +12V signal from the vehicle's ignition for `VOLTAGE_TRIGGER` (Pin 13) must be stepped down to an Arduino-safe voltage (e.g., 5V) using a voltage divider or optocoupler.
* **TB6612FNG Power:** Ensure the logic (VCC) and motor (VM) power supplies are correctly sourced from the vehicle's battery or a regulated source.



---

## 🤝 Contribution Guidelines

Contributions are highly welcome to improve the stability and versatility of this project for the Giulietta community. We especially encourage contributions related to:

* **CAN-BUS** protocol integration.
* Optimization of PWM values based on motor load or position.
* Refining the angle adjustment logic.

Please file an **Issue** before submitting a significant Pull Request.

---

## 📄 License

This project is released under the **GPL-3.0** License. Please see the [LICENSE](LICENSE) file for details.