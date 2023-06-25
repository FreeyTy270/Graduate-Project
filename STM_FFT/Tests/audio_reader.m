clear
close all
clc

%% Environment Variables
[x, fs] = audioread("Data_Files\vocal-chords-key-c_104bpm_C_major.wav");
% fs = 44.1;
N = length(x);
dt = 1/fs;
t = 0:dt:(N-1)*dt;
f = linspace(0,fs/2,N/2+1);
h = hann(N);

% x1 = h.*x(:,1); % Apply Hanning Window
x2 = h.*x; % Channel 2


% X1 = abs(fft(x1,N))/N; % Normalized Magnitude of fft output
% X1 = X1(1:N/2+1); % Single-sided conversion
% X1(2:end-1) = sqrt(2).*X1(2:end-1); % Amplitude Vrms for non-DC components
% X1 = 2*X1; % Window Correction factor

X2 = abs(fft(x2,N))/N; % Normalized Magnitude of fft output
X2 = X2(1:N/2+1); % Single-sided conversion
X2(2:end-1) = sqrt(2).*X2(2:end-1); % Amplitude Vrms for non-DC components
X2 = 2*X2; % Window Correction factor


%% Results plotting
figure
% subplot(2,1,1)
% plot(t,x1);
% title("Channel 1")
% xlabel("Time (s)")
% ylabel("Amplitude (V)")

% subplot(2,1,2)
plot(t,x2);
title("Signal in Time Domain")
xlabel("Time (s)")
ylabel("Amplitude (V)")

% sgtitle("Signal in Time Domain")

figure
% subplot(2,1,1)
% plot(f,X1);
% title("Channel 1")
% xlim([0 2.5e3])
% xlabel("Frequency (Hz)")
% ylabel("Amplitude (Vrms)")

% subplot(2,1,2)
plot(f,X2);
title("Full Sample FFT N = " + num2str(N))
xlim([0 2.5e3])
xlabel("Frequency (Hz)")
ylabel("Amplitude (Vrms)")

% sgtitle("MATLAB FFT Results")