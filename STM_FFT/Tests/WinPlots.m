clc
clear
close all

% Define window length and sampling frequency
N = 256;
n = 253;
Fs = 1000;
Fc = 100;
t = (0:(N-1))/Fs;
f = (-n/2:n/2-1)*Fs/n;

x = sin(2*pi*Fc*t);

% Create rectangular window and plot its time and frequency domains
rect_window = rectwin(n)';
xR = rect_window.*x(1:n);
rect_fft = fftshift(abs(fft(xR))/N);
figure
subplot(2,1,1)
plot(t(1:n), xR)
title('Rectangular Window in Time Domain')
xlabel('Time (s)')
ylabel('Amplitude')
subplot(2,1,2)
plot(f, rect_fft)
title('Rectangular Window in Frequency Domain')
xlabel('Frequency (Hz)')
ylabel('Magnitude')

% Create Hanning window and plot its time and frequency domains
hann_window = hann(n)';
xH = rect_window.*x(1:n);
hann_fft = fftshift(abs(fft(xH))/N);
figure
subplot(2,1,1)
plot(t(1:n), xH)
title('Hanning Window in Time Domain')
xlabel('Time (s)')
ylabel('Amplitude')
subplot(2,1,2)
plot(f, hann_fft)
title('Hanning Window in Frequency Domain')
xlabel('Frequency (Hz)')
ylabel('Magnitude')
