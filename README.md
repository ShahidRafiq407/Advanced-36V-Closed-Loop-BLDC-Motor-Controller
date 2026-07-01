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

## Wiring Diagrams

### Zone 1 — Power Supply (12V Adapter → Buck → ESP32)

```mermaid
flowchart LR
    ADAPT["12V DC<br/>Adapter"]
    BUCK["LM2596 Buck<br/>Converter<br/>OUT = 5.0V"]
    ESP["ESP32<br/>DevKit V1"]
    IRA["IR2110-A"]
    IRB["IR2110-B"]
    IRC["IR2110-C"]

    ADAPT -->|"12V+ → IN+"| BUCK
    ADAPT -->|"GND → IN-"| BUCK
    BUCK -->|"OUT+ 5V → VIN"| ESP
    BUCK -->|"OUT- → GND"| ESP
    ADAPT -->|"12V+ → Pin3 VCC"| IRA
    ADAPT -->|"12V+ → Pin3 VCC"| IRB
    ADAPT -->|"12V+ → Pin3 VCC"| IRC
    BUCK -->|"5V → Pin9 VDD"| IRA
    BUCK -->|"5V → Pin9 VDD"| IRB
    BUCK -->|"5V → Pin9 VDD"| IRC

    linkStyle 0 stroke:#ff8800,stroke-width:3px
    linkStyle 1 stroke:#000000,stroke-width:3px
    linkStyle 2 stroke:#00cc66,stroke-width:3px
    linkStyle 3 stroke:#000000,stroke-width:3px
    linkStyle 4 stroke:#ff8800,stroke-width:3px
    linkStyle 5 stroke:#ff8800,stroke-width:3px
    linkStyle 6 stroke:#ff8800,stroke-width:3px
    linkStyle 7 stroke:#00cc66,stroke-width:3px
    linkStyle 8 stroke:#00cc66,stroke-width:3px
    linkStyle 9 stroke:#00cc66,stroke-width:3px
```

### Zone 2 — Signal & Control (Hall Sensors → ESP32 → IR2110 PWM)

```mermaid
flowchart LR
    HA["Hall-A<br/>Sensor"]
    HB["Hall-B<br/>Sensor"]
    HC["Hall-C<br/>Sensor"]
    PUA((10kΩ))
    PUB((10kΩ))
    PUC((10kΩ))
    ESP["ESP32<br/>DevKit V1"]
    IRA["IR2110-A"]
    IRB["IR2110-B"]
    IRC["IR2110-C"]

    HA -->|"Signal → GPIO22"| PUA
    PUA --> ESP
    HB -->|"Signal → GPIO19"| PUB
    PUB --> ESP
    HC -->|"Signal → GPIO21"| PUC
    PUC --> ESP

    ESP -->|"GPIO25 → Pin10 HIN"| IRA
    ESP -->|"GPIO26 → Pin12 LIN"| IRA
    ESP -->|"GPIO27 → Pin10 HIN"| IRB
    ESP -->|"GPIO14 → Pin12 LIN"| IRB
    ESP -->|"GPIO32 → Pin10 HIN"| IRC
    ESP -->|"GPIO33 → Pin12 LIN"| IRC

    linkStyle 0 stroke:#aa44ff,stroke-width:3px
    linkStyle 1 stroke:#aa44ff,stroke-width:3px
    linkStyle 2 stroke:#aa44ff,stroke-width:3px
    linkStyle 3 stroke:#aa44ff,stroke-width:3px
    linkStyle 4 stroke:#aa44ff,stroke-width:3px
    linkStyle 5 stroke:#aa44ff,stroke-width:3px
    linkStyle 6 stroke:#2196F3,stroke-width:3px
    linkStyle 7 stroke:#2196F3,stroke-width:3px
    linkStyle 8 stroke:#2196F3,stroke-width:3px
    linkStyle 9 stroke:#2196F3,stroke-width:3px
    linkStyle 10 stroke:#2196F3,stroke-width:3px
    linkStyle 11 stroke:#2196F3,stroke-width:3px
```

### Zone 3 — Power Stage (IR2110 → MOSFETs → Motor)

```mermaid
flowchart LR
    BAT["36V<br/>Battery"]
    IRA["IR2110-A"]
    IRB["IR2110-B"]
    IRC["IR2110-C"]
    R1((10Ω))
    R2((10Ω))
    R3((10Ω))
    R4((10Ω))
    R5((10Ω))
    R6((10Ω))
    M1["M1<br/>High-Side"]
    M4["M4<br/>Low-Side"]
    M2["M2<br/>High-Side"]
    M5["M5<br/>Low-Side"]
    M3["M3<br/>High-Side"]
    M6["M6<br/>Low-Side"]
    MOTOR(("BLDC<br/>Motor"))

    BAT -->|"36V+ → Drain"| M1
    BAT -->|"36V+ → Drain"| M2
    BAT -->|"36V+ → Drain"| M3

    IRA -->|"Pin7 HO"| R1
    R1 -->|"→ Gate"| M1
    IRA -->|"Pin1 LO"| R2
    R2 -->|"→ Gate"| M4

    IRB -->|"Pin7 HO"| R3
    R3 -->|"→ Gate"| M2
    IRB -->|"Pin1 LO"| R4
    R4 -->|"→ Gate"| M5

    IRC -->|"Pin7 HO"| R5
    R5 -->|"→ Gate"| M3
    IRC -->|"Pin1 LO"| R6
    R6 -->|"→ Gate"| M6

    M1 -->|"Source → Phase A"| MOTOR
    M4 -->|"Drain → Phase A"| MOTOR
    M2 -->|"Source → Phase B"| MOTOR
    M5 -->|"Drain → Phase B"| MOTOR
    M3 -->|"Source → Phase C"| MOTOR
    M6 -->|"Drain → Phase C"| MOTOR

    linkStyle 0 stroke:#e60000,stroke-width:3px
    linkStyle 1 stroke:#e60000,stroke-width:3px
    linkStyle 2 stroke:#e60000,stroke-width:3px
    linkStyle 3 stroke:#ff6600,stroke-width:3px
    linkStyle 4 stroke:#ff6600,stroke-width:3px
    linkStyle 5 stroke:#ff6600,stroke-width:3px
    linkStyle 6 stroke:#ff6600,stroke-width:3px
    linkStyle 7 stroke:#ff6600,stroke-width:3px
    linkStyle 8 stroke:#ff6600,stroke-width:3px
    linkStyle 9 stroke:#ff6600,stroke-width:3px
    linkStyle 10 stroke:#ff6600,stroke-width:3px
    linkStyle 11 stroke:#ff6600,stroke-width:3px
    linkStyle 12 stroke:#ff6600,stroke-width:3px
    linkStyle 13 stroke:#ff6600,stroke-width:3px
    linkStyle 14 stroke:#ff6600,stroke-width:3px
    linkStyle 15 stroke:#00a000,stroke-width:3px
    linkStyle 16 stroke:#00a000,stroke-width:3px
    linkStyle 17 stroke:#0066ff,stroke-width:3px
    linkStyle 18 stroke:#0066ff,stroke-width:3px
    linkStyle 19 stroke:#8000ff,stroke-width:3px
    linkStyle 20 stroke:#8000ff,stroke-width:3px
```

### Zone 4 — Protection & Filtering (Capacitors & Diodes)

```mermaid
flowchart LR
    BAT["36V Battery"]
    FUSE["DC Fuse"]
    BAT -->|"36V+ →"| FUSE

    IRA["IR2110-A"]
    IRB["IR2110-B"]
    IRC["IR2110-C"]

    D_A["UF4007 Diode"]
    C_BA[("10uF Bootstrap")]
    C_15A[("15uF Tank")]
    C_M1A[("0.1uF Ceramic")]
    C_L1A[("0.1uF Ceramic")]

    IRA -->|"Pin3 → Anode"| D_A
    D_A -->|"Cathode → Pin6"| IRA
    IRA -->|"Pin6(+) → Pin5(-)"| C_BA
    IRA -->|"Pin3(+) → Pin2(-)"| C_15A
    IRA -->|"Pin3 → Pin2"| C_M1A
    IRA -->|"Pin9 → Pin13"| C_L1A

    D_B["UF4007 Diode"]
    C_BB[("10uF Bootstrap")]
    C_M1B[("0.1uF Ceramic")]
    C_L1B[("0.1uF Ceramic")]

    IRB -->|"Pin3 → Anode"| D_B
    D_B -->|"Cathode → Pin6"| IRB
    IRB -->|"Pin6(+) → Pin5(-)"| C_BB
    IRB -->|"Pin3 → Pin2"| C_M1B
    IRB -->|"Pin9 → Pin13"| C_L1B

    D_C["UF4007 Diode"]
    C_BC[("10uF Bootstrap")]
    C_15C[("15uF Tank")]
    C_M1C[("0.1uF Ceramic")]
    C_L1C[("0.1uF Ceramic")]

    IRC -->|"Pin3 → Anode"| D_C
    D_C -->|"Cathode → Pin6"| IRC
    IRC -->|"Pin6(+) → Pin5(-)"| C_BC
    IRC -->|"Pin3(+) → Pin2(-)"| C_15C
    IRC -->|"Pin3 → Pin2"| C_M1C
    IRC -->|"Pin9 → Pin13"| C_L1C

    linkStyle 0 stroke:#e60000,stroke-width:3px
    linkStyle 1 stroke:#aa44ff,stroke-width:2px
    linkStyle 2 stroke:#aa44ff,stroke-width:2px
    linkStyle 3 stroke:#aa44ff,stroke-width:2px
    linkStyle 4 stroke:#aa44ff,stroke-width:2px
    linkStyle 5 stroke:#aa44ff,stroke-width:2px
    linkStyle 6 stroke:#aa44ff,stroke-width:2px
    linkStyle 7 stroke:#2196F3,stroke-width:2px
    linkStyle 8 stroke:#2196F3,stroke-width:2px
    linkStyle 9 stroke:#2196F3,stroke-width:2px
    linkStyle 10 stroke:#2196F3,stroke-width:2px
    linkStyle 11 stroke:#2196F3,stroke-width:2px
    linkStyle 12 stroke:#00a000,stroke-width:2px
    linkStyle 13 stroke:#00a000,stroke-width:2px
    linkStyle 14 stroke:#00a000,stroke-width:2px
    linkStyle 15 stroke:#00a000,stroke-width:2px
    linkStyle 16 stroke:#00a000,stroke-width:2px
    linkStyle 17 stroke:#00a000,stroke-width:2px
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
