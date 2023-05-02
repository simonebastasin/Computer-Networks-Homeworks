% start

nameFile = fopen("inputaudio1.data");
input = [];
endVect = 0;

dec = fread(nameFile, 1, "int8");

while(~isempty(dec))
    endVect = endVect + 1;
    input(endVect) = dec; 
    dec = fread(nameFile, 1, "int8");
end

fclose(nameFile);

%% PARAMETERS
% Sampling frequency
Fs = 8000; % Hz = 1/s
% Sampling period
T = 1/Fs;
% Length of signal (number of samples in the audiotrace)
L = 160;
% Time vector (time interval for the audio trace)
t = [0:endVect-1]*T; % segnale intero
% t = [0:L-1]*T; % singolo pacchetto
% scale to express time in ms
t = t*1e3;

%% Parameters in the frequency domain
% Sample step in the frequency domain
F = 1/(L*T);
% Signal semi-band
B = L/2*F;
% Support of the signal spectrum [Hz]
f = Fs*(0:(L/2))/L;
% scale to express frequency in kHz
f = f/1e3;

%% FFT

for i = 1:L:endVect-L
    
    X = input(i:i+L-1); % pacchetto
    Y = fft(X); % Fast Fourier transform
    
    % Go to single-sided spectrum
    P2 = abs(Y/L); % two-sided spectrum P2
    P1 = P2(1:L/2+1); % single-sided spectrum P1
    P1(2:end-1) = 2*P1(2:end-1);
    
end

%% Plot signal over time
figure(1)
clf
subplot(1,1,1)
plot(t,input,'b'); % segnale intero
% plot(t,X,'r'); % un pacchetto
xlabel('time [ms]')
ylabel('original signal amplitude')

%{
%% Plot signal spectrum
figure(2)
clf
subplot(1,1,1)
plot(f,P1,'b');
xlabel('frequency [kHz]')
ylabel('|P1(f)|')
%}