import serial
import serial.tools.list_ports
import threading
import time

def list_ports():
    ports = serial.tools.list_ports.comports()
    for i, p in enumerate(ports):
        print(f"  [{i}] {p.device} — {p.description}")
    return ports

def format_time(seconds):
    h  = int(seconds // 3600)
    m  = int((seconds % 3600) // 60)
    s  = int(seconds % 60)
    cs = int((seconds * 100) % 100)
    return f"{h:02}:{m:02}:{s:02}.{cs:02}"

def read_loop(ser):
    while True:
        try:
            line = ser.readline().decode("utf-8", errors="ignore").strip()
            if line:
                print(f"  STM32 >> {line}")
        except:
            break

print("Available ports:")
ports = list_ports()
idx = int(input("Select port number: "))
port = ports[idx].device

ser = serial.Serial(port, 115200, timeout=1)
print(f"Connected to {port}")
print("Commands: s=start/stop   r=reset   l=lap   q=quit\n")

t = threading.Thread(target=read_loop, args=(ser,), daemon=True)
t.start()

# Laptop side timer state
laptop_start  = None
laptop_elapsed = 0.0
laptop_running = False

while True:
    cmd = input("").strip().lower()

    if cmd == 'q':
        break

    elif cmd == 's':
        ser.write(b's')
        if not laptop_running:
            # Starting
            laptop_start   = time.perf_counter()
            laptop_running = True
        else:
            # Stopping
            laptop_elapsed += time.perf_counter() - laptop_start
            laptop_running  = False
            print(f"  LAPTOP >> {format_time(laptop_elapsed)}")

    elif cmd == 'r':
        ser.write(b'r')
        laptop_start   = None
        laptop_elapsed = 0.0
        laptop_running = False
        print("  LAPTOP >> RESET")

    elif cmd == 'l':
        ser.write(b'l')
        if laptop_running:
            lap = laptop_elapsed + (time.perf_counter() - laptop_start)
            print(f"  LAPTOP LAP >> {format_time(lap)}")

    else:
        print("Unknown command. Use s, r, l or q.")

ser.close()
print("Disconnected.")