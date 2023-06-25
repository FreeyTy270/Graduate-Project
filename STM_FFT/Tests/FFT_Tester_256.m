clear
close all
clc

%% Environment Variables
N = 4096;
sz = N/2+1;
fs = 5000;
fc = 261.63;
fg = 783.99;
fe = 659.25;
dt = 1/fs;
t = 0:dt:(N-1)*dt;
i = 0:N-1;
f = linspace(0,fs/2,N/2+1);
h = hann(N)';

%% Signal Generation
% x = sin(2*pi*200*t) + 1;
x = (sin(2*pi*1000*t)+1) + 0.5*(cos(2*pi*200*t)+1);
% x = (sin(2*pi*fc*t) + sin(2*pi*fg*t) + sin(2*pi*fe*t)); % Create C Chord Sinusoid

T = load_data("Data_Files\SR_Real\SR_ADC-1k-200_4096.csv", sz);

xUnwin = x;
x = h.*x; % Apply Hanning Window

X = abs(fft(x,N))/N; % Normalized Magnitude of fft output
X = X(1:N/2+1); % Single-sided conversion
X(2:end-1) = sqrt(2).*X(2:end-1); % Amplitude Vrms for non-DC components
% X = 2*X; % Window Amplitude Correction Factor

%% Results plotting
figure
plot(t,xUnwin);
title("Unwindowed Signal in Time Domain")
xlabel("Time (s)")
ylabel("Amplitude (V)")

figure
plot(t,x);
title("Windowed Signal in Time Domain")
xlabel("Time (s)")
ylabel("Amplitude (V)")

figure
subplot(2,1,1)
plot(f,X);
title("MATLAB FFT Results")
xlim([-1 fs/2])
xlabel("Frequency (Hz)")
ylabel("Amplitude (Vrms)")

subplot(2,1,2)
plot(f, T);
title("CTP FFT Results")
xlim([-1 fs/2])
xlabel("Frequency (Hz)")
ylabel("Amplitude (Vrms)")

function C = load_data(CDataPath, CN)

    %% Import Data From C Program
    opts = delimitedTextImportOptions("NumVariables", CN);
    
    % Specify range and delimiter
    opts.DataLines = [3, 3];
    opts.Delimiter = ",";
    [vartypes{1, 1:CN}] = deal('double');
    opts.VariableTypes = vartypes;
    
    % Specify file level properties
    opts.ExtraColumnsRule = "ignore";
    opts.EmptyLineRule = "read";
    opts.ConsecutiveDelimitersRule = "join";
    
    % Import the data
    C = readmatrix(CDataPath, opts);

    % Clear temporary variables
    clear opts
end

