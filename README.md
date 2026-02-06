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
- Sensor calibration (stored in persistent memory)  
- Reads the pressure‑sensor matrix  
- Detects and stores movement events  
- Serves a lightweight Wi‑Fi webserver for data transfer  
- Streams data to a Python client in CSV format 

### DIY Pressure Sensors
- Based on *velostat*, a pressure‑dependent resistive sheet material  
- Each sensor is a 3cm × 3cm square cut from velostat sheet, between two 2.5cm × 2.5cm aluminum‑foil electrodes  
- Resistance varies with applied pressure  
- Four sensors are placed across (under) the mattress at torso height  
- Although physically in a line, they are electrically connected in a matrix to reduce wiring  

## Building the Circuit
![Workflow](images/schematic.png)
- All connections are shown in the diagram
- Each sensor is modeled as a resistor (pressure dependent). As described earlier, each sensor is a 3cm × 3cm velostat square between two 2.5cm × 2.5cm aluminum‑foil electrodes  
- Two external 1.5 kΩ resistors connected between GP26/27 and GND are needed to provide a correct reference. Any value in the range 0.5kΩ to 5kΩ can be used if you edit "#define R0 1.5" in Sensor.h
- Working principle: SENSOR_LINE_1/2 is forced at Vcc/GND alternating, while measuring SENSOR_COL_A/B voltages. The system of 4 equations and 4 unknowns (resistences) is solved iteratively in Sensors.cpp/h.
- **Optional:** A switch allows to turn ON/OFF the Wifi. You can also use the serial-monitor command **w** to toggle WiFi state, indicated by the builtin LED.
---

## How to Run
- To setup your WiFi, edit you credentials **char ssid[]** and **char password[]** in WiFiControl.cpp. Alternatively, create a different .cpp file and place there (no need for .h).
- When you connect the Pico W power, Wifi will try to connect. Using a serial monitor, you can view its status.
- If WiFI sucessfully connects, the IP running the Web Server will be printed; For example: IP: 192.168.1.21

### User commands - Serial Monitor
- **W** - Toggle WiFi state ON/OFF. The builtin LED switches according to WiFi State
- **P** - Prints the sensor readins in real-time. Useful to view in graph format with the Arduino IDE Serial Plotter.
- **CS** - Start the calibration process (see below)
- **CS** - Ends the calibration process (see below)
- **E** - Prints recorded Events in raw MATLAB format: [time (seconds) , sensor id (0 to 3) , pressure]

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
