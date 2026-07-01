# Advanced 36V Closed-Loop BLDC Motor Controller

This project is a high-power, closed-loop 3-phase BLDC motor controller using an ESP32, IR2110 gate drivers, and IRFB4115 MOSFETs. It features Hall sensor feedback for precise commutation, separated power/logic grounds for noise immunity, and a robust bootstrap topology.

![Circuit Overview](bldc%20circuit.jpg)

## System Features
- **Closed-Loop Control:** Uses 3 Hall sensors for accurate rotor position tracking.
- **Sensored Commutation:** Implements 6-step commutation with adjustable offset and direction mapping via the ESP32.
- **High-Power Stage:** Utilizes 6x IRFB4115 N-channel MOSFETs capable of handling high voltage and current.
- **Isolated Logic/Power Zones:** Prevents EMI noise and shoot-through by properly isolating the `POWER STAR GROUND` and `CLEAN LOGIC GROUND`.
- **Advanced Gate Driving:** Employs 3x IR2110 ICs with bootstrap capacitors to properly drive the high-side MOSFETs.

---

## Components List
- **Controller:** ESP32 DevKit V1
- **Gate Drivers:** 3x IR2110 ICs (DIP-14)
- **MOSFETs:** 6x IRFB4115 N-Channel MOSFETs
- **Buck Converter:** LM2596 Module (Steps 12V down to 5V)
- **Diodes:** 3x UF4007 Ultra-fast Recovery Diodes (for Bootstrap)
- **Electrolytic Capacitors:**
  - 3x 10µF (Bootstrap)
  - 2x 15µF (12V Bulk Tank)
  - 1x 1000µF (36V Main DC Link Filter)
- **Ceramic Capacitors:**
  - 6x 0.1µF (104) Ceramic Caps (Decoupling)
- **Resistors:**
  - 6x 10kΩ (Gate-Source Pull-Downs)
  - 3x 10kΩ (Hall Sensor Pull-Ups)
  - 6x 10Ω (Gate Resistors)
- **Power Sources:** 36V Battery & 12V DC Adapter
- **Protection:** 20A / 30A DC Fuse
- Veroboard (Perfboard), DIP-14 sockets, header pins, and various AWG wires.

---

## System Architecture (Mermaid Diagram)

Below is the schematic flowchart illustrating the power stages, control logic, and driver routing:

```mermaid
flowchart TD
    classDef topZone fill:#e1f5fe,stroke:#0288d1,stroke-width:2px;
    classDef midZone fill:#fff3e0,stroke:#f57c00,stroke-width:2px;
    classDef botZone fill:#ffebee,stroke:#d32f2f,stroke-width:2px;
    classDef motor fill:#e8f5e9,stroke:#388e3c,stroke-width:2px;

    subgraph Z1 [ZONE 1: TOP - Low Voltage & Brain]
        BUCK[LM2596 Buck Converter]
        ESP[ESP32 Controller]
        HALL[Hall Sensors]
        
        BUCK -- "Output: 5V & Logic GND" --> ESP
        ESP -. "Reads Position" .-> HALL
    end
    class Z1 topZone

    subgraph Z2 [ZONE 2: CENTER - Gate Drivers]
        IRA[IR2110 IC - Phase A]
        IRB[IR2110 IC - Phase B]
        IRC[IR2110 IC - Phase C]
    end
    class Z2 midZone

    subgraph Z3 [ZONE 3: BOTTOM - High Power]
        BAT[36V Battery + 12V Adapter]
        MA[Phase A: M1 & M4 MOSFETs]
        MB[Phase B: M2 & M5 MOSFETs]
        MC[Phase C: M3 & M6 MOSFETs]
        
        BAT -- "36V Power & Star GND" --> MA & MB & MC
    end
    class Z3 botZone

    MOTOR((3-Phase BLDC Motor))
    class MOTOR motor

    %% Connections between Zones
    ESP == "PWM Control Signals" ==> IRA & IRB & IRC
    BAT -- "12V Supply" --> IRA & IRB & IRC
    BAT -- "12V Supply" --> BUCK

    IRA == "Gate Drive" ==> MA
    IRB == "Gate Drive" ==> MB
    IRC == "Gate Drive" ==> MC

    MA == "Phase A Power" ==> MOTOR
    MB == "Phase B Power" ==> MOTOR
    MC == "Phase C Power" ==> MOTOR
```

---

## Step-by-Step Assembly Guide

This guide ensures maximum protection against EMI noise, ground loops, and shoot-through by strictly utilizing hardware zones. Do not alter the sequence.

### Zone Definition
Divide your Veroboard vertically into 3 sections:
- **Zone 1 (Top):** ESP32 and LM2596 Buck Converter. (No 36V here)
- **Zone 2 (Center):** 3x IR2110 ICs.
- **Zone 3 (Bottom):** 6x IRFB4115 MOSFETs & thick 36V power wiring.

### Step 1: The Heavy Busbars
1. **36V Positive Busbar:** Add a thick bare copper wire in Zone 3 and cover it with solder.
2. **Main Star Ground (-):** Add another thick bare copper wire below the 36V busbar in Zone 3. *This is your entire system's Star Point.*
3. **Phase Tracks:** Create three reinforced tracks in Zone 3 where the Source of the High-Side MOSFET meets the Drain of the Low-Side MOSFET.

### Step 2: The Logic Tracks
Use drag-soldering to create three horizontal tracks:
1. **12V Track:** In Zone 2, above the IC positions.
2. **5V Track:** In Zone 1, near the ESP32.
3. **Clean Logic Ground:** Right next to the 5V track. *(Do not connect this to the Star Ground yet!)*

### Step 3: Low-Profile Components
1. **10kΩ Pull-Downs:** Solder between the Gate (Left) and Source (Right) pins of all 6 MOSFETs to prevent dead-shorts.
2. **10Ω Gate Resistors:** Connect from Zone 2 (IC outputs) to Zone 3 (MOSFET Gates).
3. **UF4007 Diodes:** For each IC, connect the Anode to Pin 3 (VCC) and the Cathode (silver line) to Pin 6 (VB).
4. **0.1µF Ceramic Caps:** Place one between Pin 2 and 3 of each IC, and one between Pin 9 and 13.

### Step 4: Sockets and Headers
1. Solder 3x **DIP-14 bases** in Zone 2 for the IR2110 ICs. *(Keep the ICs out of the bases for now)*.
2. Solder female headers for the **ESP32** in Zone 1.

### Step 5: Electrolytic Capacitors
1. **10µF Bootstrap Caps:** Connect between Pin 5 (-) and Pin 6 (+) on all three ICs.
2. **15µF 12V Tank Caps:** Place these between Pin 2 (-) and Pin 3 (+) on IC 1 and IC 3 only.

### Step 6: Power Supply Setup
In Zone 1, mount the LM2596 Buck Converter:
- **IN+:** Connect to the 12V Track.
- **IN-:** Wire *directly* to the Main Star Ground Busbar in Zone 3.
- **OUT+:** Connect to the 5V Track.
- **OUT-:** Connect to the Clean Logic Ground Track.

### Step 7: MOSFET Installation (The Muscle)
Mount the IRFB4115 MOSFETs in Zone 3:
1. **M1, M2, M3 (High-Side):** Solder Middle Pin (Drain) to the 36V Positive Busbar.
2. **M4, M5, M6 (Low-Side):** Solder Right Pin (Source) to the Main Star Ground Busbar.
3. Solder M1 Source to M4 Drain (Phase A), M2 Source to M5 Drain (Phase B), and M3 Source to M6 Drain (Phase C).

### Step 8: Master Pin-to-Pin Wire Routing
For each IR2110 IC socket, use thin 22 AWG wire to connect:
- **Pin 1 (LO):** Through a 10Ω resistor to the Low-Side MOSFET Gate.
- **Pin 2 (COM):** Directly to the Low-Side MOSFET Source. *(DO NOT mix with logic ground!)*
- **Pin 3 (VCC):** To the 12V Track.
- **Pin 4 (NC):** Floating (Leave empty).
- **Pin 5 (VS):** Directly to its respective Phase Output Track. *(CRITICAL: Use thin wire to prevent EMI noise. This is your floating sensor point!)*
- **Pin 6 (VB):** Bootstrap capacitor/diode junction (done in Steps 3 & 5).
- **Pin 7 (HO):** Through a 10Ω resistor to the High-Side MOSFET Gate.
- **Pin 9 (VDD):** To the 5V Track.
- **Pin 10 (HIN):** To the respective ESP32 High PWM pin.
- **Pin 11 (SD):** Shorted to Pin 13.
- **Pin 12 (LIN):** To the respective ESP32 Low PWM pin.
- **Pin 13 (VSS):** To the Clean Logic Ground Track.

Finally, connect the **Main Star Ground** to the **Clean Logic Ground** via *one single jumper wire*.

---

## ESP32 Pin Mapping & Tuning
The ESP32 uses a 20kHz silent PWM frequency.

### Inputs:
- **Throttle:** GPIO 34 (Analog Read, mapped to 100-255 PWM).
- **Hall Sensors:** GPIO 22 (A), GPIO 19 (B), GPIO 21 (C). Use 10kΩ pull-ups to 5V.

### Tuning (in `main.cpp`):
If the motor does not spin correctly, modify the following variables in the `loop()` section of `main.cpp`:
- `int dir = 1;` (Change to `-1` if the motor direction mapping is reversed)
- `int offset = 0;` (Iterate from `0` to `5` to find the correct commutation alignment)
