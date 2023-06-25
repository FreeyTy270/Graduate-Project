clear
close all
clc

%% Environment Variables
N = 256;
fs = 5000;
fc = 261.63;
fg = 783.99;
fe = 659.25;
dt = 1/fs;
t = 0:dt:(N-1)*dt;
i = 0:N-1;
% [x, xfs] = audioread("Data_Files\vocal-chords-key-c_104bpm_C_major.wav", [44100 44100+255]);
f = linspace(0,fs/2,N/2+1);
% x = (sin(2*pi*fc*t) + sin(2*pi*fg*t) + sin(2*pi*fe*t)); % Create C Chord Sinusoid
% x = sin(2*pi*200*t)+1;
x = (sin(2*pi*1000*t)+1) + 0.5*(cos(2*pi*200*t)+1);
h = hann(N)';
x = h.*x; % Apply Hanning Window

%% Set up the Import Options and import the data
opts = delimitedTextImportOptions("NumVariables", 129);

% Specify range and delimiter
opts.DataLines = [3, 3];
opts.Delimiter = ",";

% Specify column names and types
opts.VariableNames = ["TimePerTransform492299988US", "VarName2", "VarName3", "VarName4", "VarName5", "VarName6", "VarName7", "VarName8", "VarName9", "VarName10", "VarName11", "VarName12", "VarName13", "VarName14", "VarName15", "VarName16", "VarName17", "VarName18", "VarName19", "VarName20", "VarName21", "VarName22", "VarName23", "VarName24", "VarName25", "VarName26", "VarName27", "VarName28", "VarName29", "VarName30", "VarName31", "VarName32", "VarName33", "VarName34", "VarName35", "VarName36", "VarName37", "VarName38", "VarName39", "VarName40", "VarName41", "VarName42", "VarName43", "VarName44", "VarName45", "VarName46", "VarName47", "VarName48", "VarName49", "VarName50", "VarName51", "VarName52", "VarName53", "VarName54", "VarName55", "VarName56", "VarName57", "VarName58", "VarName59", "VarName60", "VarName61", "VarName62", "VarName63", "VarName64", "VarName65", "VarName66", "VarName67", "VarName68", "VarName69", "VarName70", "VarName71", "VarName72", "VarName73", "VarName74", "VarName75", "VarName76", "VarName77", "VarName78", "VarName79", "VarName80", "VarName81", "VarName82", "VarName83", "VarName84", "VarName85", "VarName86", "VarName87", "VarName88", "VarName89", "VarName90", "VarName91", "VarName92", "VarName93", "VarName94", "VarName95", "VarName96", "VarName97", "VarName98", "VarName99", "VarName100", "VarName101", "VarName102", "VarName103", "VarName104", "VarName105", "VarName106", "VarName107", "VarName108", "VarName109", "VarName110", "VarName111", "VarName112", "VarName113", "VarName114", "VarName115", "VarName116", "VarName117", "VarName118", "VarName119", "VarName120", "VarName121", "VarName122", "VarName123", "VarName124", "VarName125", "VarName126", "VarName127", "VarName128", "VarName129"];
opts.VariableTypes = ["double", "double", "double", "double", "double", "double", "double", "double", "double", "double", "double", "double", "double", "double", "double", "double", "double", "double", "double", "double", "double", "double", "double", "double", "double", "double", "double", "double", "double", "double", "double", "double", "double", "double", "double", "double", "double", "double", "double", "double", "double", "double", "double", "double", "double", "double", "double", "double", "double", "double", "double", "double", "double", "double", "double", "double", "double", "double", "double", "double", "double", "double", "double", "double", "double", "double", "double", "double", "double", "double", "double", "double", "double", "double", "double", "double", "double", "double", "double", "double", "double", "double", "double", "double", "double", "double", "double", "double", "double", "double", "double", "double", "double", "double", "double", "double", "double", "double", "double", "double", "double", "double", "double", "double", "double", "double", "double", "double", "double", "double", "double", "double", "double", "double", "double", "double", "double", "double", "double", "double", "double", "double", "double", "double", "double", "double", "double", "double", "double"];

% Specify file level properties
opts.ExtraColumnsRule = "ignore";
opts.EmptyLineRule = "read";
opts.ConsecutiveDelimitersRule = "join";

% Specify variable properties
opts = setvaropts(opts, "TimePerTransform492299988US", "TrimNonNumeric", true);
opts = setvaropts(opts, "TimePerTransform492299988US", "ThousandsSeparator", ",");

% Import the data
% inData = readtable("Data_Files\SR_Synth\SR_C-Chord_44k.csv", opts);
inData = readtable("Data_Files\SR_Real\SR_ADC-1k-200.csv", opts);
% inData = readtable("\Data_Files\InTime\IT_ADC-SMP_129.csv", opts);

% Convert to output type
T = 2*table2array(inData);

% Clear temporary variables
clear opts

% s = hann(N/2+1)'.*T;
% S = abs(fft(x,N))/N; % Normalized Magnitude of fft output
% S = S(1:N/2+1); % Single-sided conversion
% S(2:end-1) = sqrt(2).*S(2:end-1); % Amplitude Vrms for non-DC components
% S = 2*S;

X = abs(fft(x,N))/N; % Normalized Magnitude of fft output
X = X(1:N/2+1); % Single-sided conversion
X(2:end-1) = sqrt(2).*X(2:end-1); % Amplitude Vrms for non-DC components
% X = 2*X; % Window Correction factor

%% Results plotting
figure
plot(t,x);
title("Signal in Time Domain")
xlabel("Time (s)")
ylabel("Amplitude (V)")

% figure
% plot(t(1:N/2+1),s);
% title("Signal in Time Domain")
% xlabel("Time (s)")
% ylabel("Amplitude (V)")

figure
subplot(2,1,1)
plot(f,X);
title("MATLAB FFT Results")
xlim([-1 1500])
xlabel("Frequency (Hz)")
ylabel("Amplitude (Vrms)")

subplot(2,1,2)
plot(f, T);
title("CTP FFT Results")
xlim([-1 1500])
xlabel("Frequency (Hz)")
ylabel("Amplitude (Vrms)")

