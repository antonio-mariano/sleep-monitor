### IMPORTANTE: NECESSARIO INSTALAR REQUESTS, FAZENDO NUM CMD
# pip install requests

import requests
import sys

base_ip = "192.168.1."
path = "/events"

print("Scanning for device in the range 192.168.1.1 - 192.168.1.64...")
print("Trying IP: 192.168.1.x", end="\n\n")

found = False
device_url = None
data_text = None

for x in range(1, 65):  # limit to 1–64
    print(f"\rTrying x = {x:02d}", end="", flush=True)

    url = f"http://{base_ip}{x}{path}"

    try:
        response = requests.get(url, timeout=0.2)
        response.raise_for_status()
        data_text = response.text
        device_url = url
        found = True
        break
    except requests.exceptions.RequestException:
        continue

print()  # move to next line

if found:
    print(f"\nDevice found at: {device_url}")
    print("Data received successfully.")
    
    with open("events.csv", "w") as f:
        f.write(data_text)

    print("CSV file generated: events.csv")
else:
    print("\nNo device responded on /events in the range 192.168.1.1 - 192.168.1.64.")

input("\nPress ENTER to exit...")
