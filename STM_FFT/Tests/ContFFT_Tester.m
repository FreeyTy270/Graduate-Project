clear
close all
clc

global fs;
global N;
global n;

n = 256;
Rep = 34;
N = (n/2+1)*Rep;
fs = 5000;
TestPath = "C:\Users\Ty Freeman\Documents\Classes\Grad_Paper\tkf1604_grad_ppr\STM_FFT\Tests\Data_Files\";

dt = n/(2*fs);
t = dt:dt:Rep*dt;
f = linspace(0,fs/2,n/2+1);

RawPath = TestPath + "CF_Sweep-1-200_500ms-mv.csv";
CPath = TestPath + "ContFFT\CF-F5k-Sweep-1-200_500ms-mv_STOPS.csv";

[x, T] = load_data(RawPath, CPath);

strt = n/2+2;
for i=2:Rep
    nxtEnd = strt+(n/2+1);
    diff = nxtEnd-strt;
    T(i,1:n/2+1) = T(1,strt:nxtEnd-1);
    strt = nxtEnd;
end

[X, F, q] = spectrogram(x, hann(n), n/2, n, fs);

figure
plot(x);

figure
waterplot(X, F, q, 'M');


figure
waterplot(T, f, t, 'C');


function [M, C] = load_data(RawDataPath, CDataPath)
    
    global N;
    %% Import Raw Signal Data
    opts = delimitedTextImportOptions("NumVariables", 2);
    
    % Specify range and delimiter
    opts.DataLines = [17, Inf];
    opts.Delimiter = ",";
    
    % Specify column names and types
    opts.VariableNames = ["Var1", "OutputV"];
    opts.SelectedVariableNames = "OutputV";
    opts.VariableTypes = ["string", "double"];
    
    % Specify file level properties
    opts.ExtraColumnsRule = "ignore";
    opts.EmptyLineRule = "read";
    
    % Specify variable properties
    opts = setvaropts(opts, "Var1", "WhitespaceRule", "preserve");
    opts = setvaropts(opts, "Var1", "EmptyFieldRule", "auto");
    
    % Import the data
    CFSweep102000 = readtable(RawDataPath, opts);
    
    % Convert to output type
    M = table2array(CFSweep102000);
    
    % Clear temporary variables
    clear opts

    %% Import Data From C Program
    opts = delimitedTextImportOptions("NumVariables", N);
    
    % Specify range and delimiter
    opts.DataLines = [3, 3];
    opts.Delimiter = ",";
    [vartypes{1, 1:N}] = deal('double');
    opts.VariableTypes = vartypes;
    
    % Specify file level properties
    opts.ExtraColumnsRule = "ignore";
    opts.EmptyLineRule = "read";
    opts.ConsecutiveDelimitersRule = "join";
    
    % Import the data
    C = readmatrix(CDataPath, opts);

    M = repmat(M, floor(length(C)/length(M)), 1);
    
    % Clear temporary variables
    clear opts
end

function waterplot(s,f,t, Nm)
global fs;
global n;
% Waterfall plot of spectrogram
    
    if(Nm == 'C')
        s = s(:, 1:(n/2+1));
        s = (s.^2); % Convert to power spectrum (Vrms^2)
        s = s./(fs/n*1.5); % Power Spectral Density (Vrms^2/Hz)
    else
        s = abs(s)'.^2;
        s = s./(fs/n*1.5);
    end

    waterfall(f,t,s)
    set(gca,XDir="reverse",View=[30 50])
    title(Nm + " Spectrogram")
    xlim([0 500])
    ylabel("Time (sec)")
    xlabel("Frequency (Hz)")
    zlabel("PSD (Vrms^2/Hz")
end        

