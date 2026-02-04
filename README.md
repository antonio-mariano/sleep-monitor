# 💤 Sleep Monitor – Real‑Time Sleep Activity Tracking

A complete sleep‑monitoring pipeline built around a **Raspberry Pi Pico W**, using **DIY pressure sensors**. It uses a **Wi‑Fi webserver** for data transfer
a **Python script** to download the data in CSV format, and **MATLAB** for visualization and analysis.
Future goals include interpreting the data and developing a smart alarm that wakes the user during light‑sleep phases

---

## 📌 Project Overview

The system collects pressure deltas in 4 different positions on a matress during sleep, and stores it for later analysis.  
The workflow below summarizes the full pipeline:

![Workflow](images/workflow.png)

---

## 🧩 Components
### Raspberry Pi Pico W + Firmware (Arduino IDE C++)
- Reads the pressure‑sensor matrix  
- Detects movement events  
- Allows sensor calibration (stored in persistent memory)  
- Serves a lightweight Wi‑Fi webserver for data transfer  
- Streams data to a Python client in CSV format 

### Pressure Sensors
- Based on **velostat**, a pressure‑dependent resistive sheet material  
- Each sensor is a 3 cm × 3 cm velostat square between two 2.5 cm × 2.5 cm aluminum‑foil electrodes  
- Resistance varies with applied pressure  
- Four sensors placed across the mattress at torso height  
- Although physically in a line, they are electrically connected in a matrix to reduce wiring  

## How to Build the Circuit
- All connections are shown in the following diagram
- ![Workflow](images/schematic.png)
- Each sensor is a modeled as a (pressure dependent) resistor
- Two 1.5 kΩ resistors are needed to provide a correct reference. Any value in the range 500-5k Ω can be used if you edit "#define R0 1.5" in sensor.h
- A switch allows to turn ON/OFF the Wifi. You can avoid it and have always ON if you remove "disconnect_wifi()" in the setup() in sleep-monitor.ino.

---

## How to Run

### • Wi‑Fi Setup
- Connects to the Wi‑Fi network defined in `secrets.h`  
- Initializes a lightweight HTTP server


---

## 🧪 Analysis Tools

### MATLAB
- `sleep_analysis.m`  
  Loads a `.mat` dataset and generates plots of:
  - pressure over time  
  - position changes  
  - annotated events  

- `create_dataset.m`  
  Converts raw CSV logs into the `.mat` format used by the project.

### Python
- `get_events.py`  
  Extracts event markers from raw logs.

---


## 🚀 How to Run the Analysis

1. Open MATLAB  
2. Load the synthetic dataset:

```matlab
load("dataset/synthetic_sleep.mat");
