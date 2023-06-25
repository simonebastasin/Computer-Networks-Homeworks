%% Simone Bastasin 1201543

%% Nomi file
nameInputFile = strcat("inputaudio1.data");
nameOutputFile = strcat("outputVAD1.txt");

%% Parametri
% soglia dell'ampiezza nel dominio delle frequenze
soglia = 0.71;
% prec: numero pacchetti considerati precedenti al n-esimo
prec = 4;
% succ: numero pacchetti considerati successivi al n-esimo
succ = 2;
% L: lunghezza singolo pacchetto
L = 160;
% Fs: frequenza di campionamento
Fs = 8000; % Hz = 1/s
% T: periodo di campionamento
T = 1/Fs;
% M: lunghezza del segnale (numero di campioni nella traccia audio)
M = L*(prec+succ+1);

%% Lettura da file
% apertura i file input e output
inputFile = fopen(nameInputFile);
outputFile = fopen(nameOutputFile, "w");
% input: vettore dei byte del file di input
input = [];
% output: vettore dei byte di input con azzerati i byte non voce
output = [];
% endVect: contatore per inserimento byte nel vettore di input
endVect = 0;
% lettura primo byte (8 bit) dal file input
byte = fread(inputFile, 1, "int8");
% iterazione ciclo fino a fine file
while(~isempty(byte))
    % aggiunta byte nel vettore input
    endVect = endVect+1;
    input(endVect) = byte;
    % lettura successivo byte (8 bit)
    byte = fread(inputFile, 1, "int8");
end
% chiusura il file input
fclose(inputFile);

%% input -> FFT() -> controllo() -> output
% per ogni pacchetto
for i = 1:L:endVect-L
    % first: per i primi 4 pacchetto sottraggo i precedenti non presenti
    first = 0;
    % last: per (pen)ultimo pacchetto sottraggo i successivi non presenti
    last = 0;
    % se considero anche pacchetti precedenti all'i-esimo
    if prec > 0
        % se è il 1° pacchetto -> nessun pacchetto precedente
        if i < L
            first = prec;
        % se è il 2° pacchetto -> solo 1 pacchetto precedente
        elseif i < L*2
            first = prec-1;
        % se è il 3° pacchetto -> solo 2 pacchetto precedente
        elseif i < L*3
            first = prec-2;
        % se è il 4° pacchetto -> solo 3 pacchetto precedente
        elseif i < L*4
            first = prec-3;
        end
    end
    % se considero anche pacchetti successivi all'i-esimo
    if succ > 0
        % se è il penultimo pacchetto -> solo 1 pacchetto successivo
        if i > endVect-L*3 && i < endVect-L*2
            last = succ-1;
        % se è l'ultimo pacchetto -> nessun pacchetto successivo
        elseif i > endVect-L*2
            last = succ;
        end
    end
    % X(t): segnale con 7 pacchetti (4|n|2)
    X = input(i - L*prec + L*first : i + L + L*succ - L*last - 1);
    Y = fft(X); % Y: Trasformata di Fourier veloce di X(t)

    % calcolo dello spettro unilaterale
    P2 = abs(Y/(M-L*first-L*last)); % P2: spettro bilaterale
    P1 = P2(1:(M-L*first-L*last)/2+1); % P1: spettro unilaterale
    P1(2:end-1) = 2*P1(2:end-1);

    % controllo di voce (range spettro voce umana: [200; 3400] Hz)
    isVoice = false;
    for j = 200*T*L+1:3400*T*L+1 % nel range: [200; 3400] Hz
        % se il valore supera la soglia -> è voce
        if P1(j) > soglia
            isVoice = true;
        end
    end
    % se il pacchetto è voce -> pacchetto tenuto -> bit a 1
    if isVoice == true
        output(i:i+L-1) = input(i:i+L-1);
        fwrite(outputFile, '1', "int8");
    % se il pacchetto non è voce -> pacchetto azzerato -> bit a 0
    else
        output(i:i+L-1) = zeros(1, L, 'int8');
        fwrite(outputFile, '0', "int8");
    end
end
% dopo ultimo pacchetto: byte avanzati considerati come un pacchetto nullo
output(i+L:endVect) = zeros(1, endVect-(i+L)+1, 'int8');
fwrite(outputFile, '0', "int8");
% chiusura il file output
fclose(outputFile);

%% File contenente pacchetti dell'audio finale per grafico o per file .wav
% nameEndFile = strcat("outputaudio1.data");
% endFile = fopen(nameEndFile, "w");
% fwrite(endFile, output, "int8");
% fclose(endFile);

% END