import serial
import numpy as np
import matplotlib.pyplot as plt

# ME433 HW16 - PI current controller tuning + plotting
# CHANGE PORT to the STM32's COM port, for me is com7

PORT = 'COM7'
N = 400

ser = serial.Serial(PORT, 115200, timeout=2)
ser.write(b'a')

idx = []
desired = []
actual = []

while len(idx) < N:
    line = ser.readline().decode(errors='ignore').strip()
    parts = line.split(',')
    if len(parts) == 3:
        try:
            idx.append(int(parts[0]))
            desired.append(float(parts[1]))
            actual.append(float(parts[2]))
        except ValueError:
            pass

ser.close()

plt.plot(idx, desired, 'b', label='desired current')
plt.plot(idx, actual, 'r', label='actual current')
plt.xlabel('sample (1 kHz)')
plt.ylabel('current [mA]')
plt.legend()
plt.title('HW16 PI current control')
plt.show()
