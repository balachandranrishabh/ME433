import numpy as np
import matplotlib.pyplot as plt

# HW9 - filtering the signals
# change the filename and the filter numbers for each signal

fname = 'sigA.csv'

# load the data
data = np.loadtxt(fname, delimiter=',')
t = data[:,0]
y = data[:,1]

dt = t[1] - t[0]
fs = 1/dt
n = len(y)
print("fs =", fs, "N =", n)

# ---- moving average ----
X = 50
y_avg = np.zeros(n)
for i in range(n):
    if i < X:
        y_avg[i] = np.mean(y[0:i+1])
    else:
        y_avg[i] = np.mean(y[i-X+1:i+1])

# ---- IIR ----
A = 0.95
B = 0.05
y_iir = np.zeros(n)
y_iir[0] = y[0]
for i in range(1, n):
    y_iir[i] = A*y_iir[i-1] + B*y[i]

# ---- FIR ----
# low pass, change cutoff per signal
cutoff = 200.0
numtaps = 51
fc = cutoff/fs
h = np.zeros(numtaps)
for i in range(numtaps):
    if i == (numtaps-1)/2:
        h[i] = 2*fc
    else:
        x = i - (numtaps-1)/2
        h[i] = np.sin(2*np.pi*fc*x)/(np.pi*x)
    # hamming window
    h[i] = h[i]*(0.54 - 0.46*np.cos(2*np.pi*i/(numtaps-1)))
h = h/np.sum(h)
y_fir = np.convolve(y, h, 'same')

# ---- pick which one to look at ----
y_filt = y_fir   # change to y_avg or y_iir

# fft of original
Y = np.fft.fft(y)/n
Y = abs(Y[0:n//2])
k = np.arange(n)
frq = k/(n/fs)
frq = frq[0:n//2]

# fft of filtered
Yf = np.fft.fft(y_filt)/n
Yf = abs(Yf[0:n//2])

# plot
plt.subplot(2,1,1)
plt.plot(t, y, 'b')
plt.plot(t, y_filt, 'r')
plt.xlabel('Time [s]')
plt.ylabel('Signal')

plt.subplot(2,1,2)
plt.loglog(frq, Y, 'b')
plt.loglog(frq, Yf, 'r')
plt.xlabel('Frequency [Hz]')
plt.ylabel('|Y(freq)|')

plt.show()