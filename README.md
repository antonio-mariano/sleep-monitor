# Sleep Monitor – Real‑Time Sleep Activity Tracking

A complete sleep‑monitoring pipeline built around a **Raspberry Pi Pico W**, using **DIY pressure sensors**. It uses a **Wi‑Fi web server** for data transfer,
a **python script** to download the data in CSV format, and **MATLAB** for visualization and analysis.
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
- Serves a Wi‑Fi web server for data transfer in CSV format 

### DIY Pressure Sensors
- Based on *velostat*, a pressure‑dependent resistive sheet material  
- Each sensor is a 3cm × 3cm velostat square between two 2.5cm × 2.5cm aluminum‑foil electrodes  
- Resistance varies with applied pressure  
- Four sensors are placed across (under) the mattress at torso height  
- Although physically in a line, they are electrically connected in a matrix to reduce wiring  

## Building the Circuit
![Schematic](images/schematic.png)
- Each sensor is modeled as a resistor (pressure dependent).
- To build a sensor, cut a 3cm × 3cm square from a velostat sheet, and place it between two 2.5cm × 2.5cm aluminum‑foil electrodes
- Two external 1.5 kΩ resistors connected between GP26/27 and GND are needed to provide a correct reference. Any value in the range 0.5kΩ to 5kΩ can be used: edit "#define R0 1.5" in Sensor.h
- Working principle: SENSOR_LINE_1/2 is forced at Vcc/GND alternating, while measuring SENSOR_COL_A/B voltages. The system of 4 equations and 4 unknowns (resistences) is solved iteratively in Sensors.cpp/h.
- **Optional:** A switch allows to turn ON/OFF the Wifi. You can also use the serial-monitor command **w** to toggle WiFi state, indicated by the builtin LED.
---

## 🚀 How to Run
### Wi-Fi setup
- To setup your Wi-Fi, edit you credentials **char ssid[]** and **char password[]** in WiFiControl.cpp. Alternatively, create a different .cpp file and place there (no need for .h).
- When you power the Pico W, it will try to connect to your Wi-Fi. Using a serial monitor, you can view its progress
- If WiFI sucessfully connects, the IP running the Web Server will be printed; For example: IP: 192.168.1.21
- The buildtin LED in the Pico W shows the Wi-Fi status

### User commands - Serial Monitor
- **W** - Toggle WiFi state ON/OFF. The builtin LED switches according to WiFi State
- **P** - Prints the sensor readins in real-time. Useful to view in graph format with the Arduino IDE Serial Plotter.
- **CS** - Start the calibration process (see below)
- **CS** - Ends the calibration process (see below)
- **E** - Prints recorded Events in raw MATLAB format: [time (seconds) , sensor id (0 to 3) , pressure]

### Calibration
- The code comes with a default calibration based on prescribed dimensions, so its not mandatory to calibrate it, altough it's usefull to compensate individual differences between sensors
- The sensor array should be in its final position under the matress
- Use **CS** to start the caliprating process. If Wifi is OFF, the builtin LED will turn ON.
- Press and release the matress in each of the 4 sensor positions in sequence. Apply some vigorous pressuer, not exagerating.
- Apply aproximately the same pressure in all four sensors.
- Use **CE** to end the calibrating process.
#### Note
- The calibration algorithm considers the pressure applied to be a "nominal value", and adjusts the sensitivity of each sensor in order to map the pressure applied during calibration to the value of 1000
- The calibration is stored in permanent memory, so you don't need to recalibrate every time you power the Pico W
### Real-Time Sensor Visualization
- Using **P** in the serial activates real-time print of sensor data
- Open the Arduino IDE serial plotter to view in graphical format
---

## 🧪 Data Transfer and Analysis Tools
### Downloading the Events
- Make sure the Pico W web server is ON by checking the LED status
- Make sure your python has *requests* installed. You can install it with "pip install requests"
- Run `analysis/get_events.py` to download the data and create `events.csv`
#### Note
- `get_events.py` searches for local web servers that respond to 192.168.1.x/events, with x from 1 to 64, as it very likely is in that range
- Whenever you turn ON the Wi-Fi, it's IP is printed on the serial monitor
- If you know the IP, you can view the data in the browser by acessing http://IP/events

### Creating a .mat dataset
- Use MATLAB script `analisys/create_dataset.m` to convert raw CSV logs in `events.csv´ to the `.mat` format.
- Select a name by editing `output_name = 'example_name.mat'`.
- Optionally,  you can add timestamps, editing `labels` and `labels_time`;

### Visualizing the data
- Running `analisys/sleep_analysis.m` will load the `.mat` file and generate:
- Plot of pressure over time  
- Annotated events (if present in the file)
- ![Sleep](images/sleep.png)
- ![Histogram](images/histogram.png)
#### Note:
- If you don't have a dataset, you can test the visualization with `dataset/synthetic_sleep.mat`

---

