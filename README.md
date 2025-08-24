# EspressoCM Arduino Library

The EspressoCMlib library provides a versatile suite of tools for embedded hardware systems, primarily targeting ESP32 and ATtiny85 microcontrollers. It is designed to support system-level diagnostics, core selection, unit conversion, and several error detection and correction coding schemes.

## 📦 Features
- 🧠 Core Detection
- 🔍 Diagnostics
-  📐 Unit Conversion
-  🧬 Error Detection & Correction (Checksum, Hamming Code(7,4), LDPC, Convolution)
-  💡 Pin Definitions

---

### ESP32 Pin Variables

| Variable       | Description                    | Default Value |
|----------------|--------------------------------|---------------|
| `CORE`         | Detected core number           | `0`           |
| `CORESEL0`     | Core selection pin 0           | `37`          |
| `CORESEL1`     | Core selection pin 1           | `36`          |
| `LEDpin`       | Onboard LED control pin        | `46`          |
| `E2B_BUILTIN`  | Built-in E2B pin               | `40`          |
| `E2B_EXTERNAL` | External E2B pin               | `21`          |
| `CS_FLASH`     | Chip-select for external flash | `10`          |

---

- ## 📘 API Reference

### 🔧 Core and Diagnostics

#### `EspressoCM()`
- **Constructor**. Initializes pin modes and reads core selection pins.

#### `uint8_t getCoreNumber()`
- **Returns**: The current core number (`CORE1` or `CORE2`).

#### `uint8_t runDiagnostics()`
- **Returns**: `0` if no issues are detected; otherwise, an error code.

---

---

### ⚡ Power & Thermal Analysis

#### `float getBoardLDOEfficiency(float _pin, float _pout)`
- **Inputs**:
  - `_pin` — Input power (W).
  - `_pout` — Output power (W).
- **Returns**: Efficiency of the onboard 3.3V regulator.

#### `float getBoardHeatDissipation(float _pin, float _pout)`
- **Inputs**: Same as above.
- **Returns**: Power dissipated (W) as heat by the regulator.

#### `float getRegulatorTemperatureRise(float _pin, float _pout)`
- **Inputs**: Same as above.
- **Returns**: Estimated regulator junction temperature (°C) based on dissipation.

#### `float getRequiredHeatsinkThermalResistance(float _pin, float _pout, float Tmax, float Tambient, float RthJC, float RthCS)`
- **Inputs**:
  - `_pin` — Input power (W).
  - `_pout` — Output power (W).
  - `Tmax` — Maximum junction temperature (°C).
  - `Tambient` — Ambient temperature (°C).
  - `RthJC` — Thermal resistance junction-to-case (°C/W).
  - `RthCS` — Thermal resistance case-to-sink (°C/W).
- **Returns**: Required heatsink thermal resistance (°C/W).

#### `float getRequiredAirflow(float _pin, float _pout, float allowedRise)`
- **Inputs**:
  - `_pin` — Input power (W).
  - `_pout` — Output power (W).
  - `allowedRise` — Maximum allowed temperature rise (°C).
- **Returns**: Required airflow in **CFM** for adequate cooling.

---

### 📐 Unit Conversion

#### `float mils(float val_mm)`
- **Input**: `val_mm` — value in millimeters.
- **Returns**: Converted value in **mils**.

#### `float millimeters(float val_mils)`
- **Input**: `val_mils` — value in mils.
- **Returns**: Converted value in **millimeters**.

---

### ✅ Checksum

#### `static uint8_t checksum(const uint8_t* addr, uint8_t len)`
- **Inputs**:
  - `addr` — pointer to a byte array.
  - `len` — length of the array.
- **Returns**: 8-bit XOR checksum of the data array.

---

### 🧬 Hamming(7,4) Code

#### `uint8_t hammingEncode(uint8_t data)`
- **Input**: `data` — 4-bit input (only lower 4 bits are used).
- **Returns**: 7-bit Hamming-encoded byte (padded to 8 bits).

#### `uint8_t hammingDecode(uint8_t encodedData)`
- **Input**: `encodedData` — 7-bit Hamming code (padded to 8 bits).
- **Returns**: Corrected 4-bit original data in the lower 4 bits.

---

### 🧾 Low-Density Parity Check (LDPC)

#### `uint16_t encodeLDPC(byte data)`
- **Input**: `data` — 8-bit input.
- **Returns**: 12-bit LDPC-encoded data as a `uint16_t`.

#### `byte decodeLDPC(uint16_t encodedData)`
- **Input**: `encodedData` — 12-bit encoded data.
- **Returns**: Decoded 8-bit output. Uses simplified logic.

---

### 🔁 Convolutional Codes

#### `uint16_t encodeConvolution(uint8_t inputByte)`
- **Input**: `inputByte` — 8-bit input data.
- **Returns**: 16-bit encoded data using polynomials G1=111 and G2=101.

#### `uint8_t decodeConvolution(uint16_t encodedData)`
- **Input**: `encodedData` — 16-bit encoded data.
- **Returns**: 8-bit decoded result using a basic Viterbi-like algorithm.
