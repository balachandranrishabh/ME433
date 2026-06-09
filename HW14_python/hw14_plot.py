import serial
import numpy as np
import matplotlib.pyplot as plt



PORT = 'COM6'
N = 200   # number of samples to collect

# open the serial port
ser = serial.Serial(PORT, 115200, timeout=2)

ser.write((str(N) + '\n').encode())

# initialize lists to hold the data
times = []
raws = []
filts = []

# read lines in the same format as the pico
while len(times) < N:
    line = ser.readline().decode().strip()
    parts = line.split(',')
    if len(parts) == 3:
        times.append(float(parts[0]) / 1000.0)   # ms to seconds
        raws.append(float(parts[1]))
        filts.append(float(parts[2]))

ser.close()

# turn lists into arrays
times = np.array(times)
raws = np.array(raws)
filts = np.array(filts)

# figure out the sample rate from the timestamps
dt = times[1] - times[0]
fs = 1 / dt
print("sample rate:", fs, "Hz")

# fft raw
n = len(raws)
raw_fft = np.abs(np.fft.fft(raws - np.mean(raws)) / n)
raw_fft = raw_fft[0:n//2]
freqs = np.arange(n) / (n / fs)
freqs = freqs[0:n//2]

# FFT filtered
filt_fft = np.abs(np.fft.fft(filts - np.mean(filts)) / n)
filt_fft = filt_fft[0:n//2]

# signal over time
plt.subplot(2, 1, 1)
plt.plot(times, raws, 'b')
plt.plot(times, filts, 'r')
plt.xlabel('Time [s]')
plt.ylabel('Value')

# fft
plt.subplot(2, 1, 2)
plt.plot(freqs, raw_fft, 'b')
plt.plot(freqs, filt_fft, 'r')
plt.xlabel('Frequency [Hz]')
plt.ylabel('Strength')

plt.show()