#include <stdio.h>

// ______________________________


// return 0: little endian, retrun 1: big endian
int EndianDetector() {	
  short int x = 1;
  // big endian, x: | 00000000 | 0000001 |
  // little end, x: | 00000001 | 0000000 |
  if(*(char *) &x == 1)
    return 0; // è little endian
  else return 1; // è big endian
}


// Conversione dei byte di x in big endian se necessario
unsigned int ToLittleEndian(unsigned int x) {
  char *pc = (char *) &x;
  // Se i byte vengono ordinati in big endian
  if(EndianDetector() == 1) {
    // Conversione
    char temp0 = pc[0];
    char temp1 = pc[1];
    pc[0] = pc[3];
    pc[1] = pc[2];
    pc[2] = temp1;
    pc[3] = temp0;
  }
  return x;
}

// ______________________________


// UTF8_encoding

void encoding(char inputFile[], char outputFile[], int inputSize) {

  // Apertura file binario di input in lettura
  FILE *fp = fopen(inputFile, "rb");

  // output[]: array contenente i dati in output (in UTF-8)
  // [inputSize*7/4] perché il file di output (in UTF-8) può avere al più 7 byte per ogni input da 4 byte (in UCS-4)
  unsigned char output[inputSize*7/4];
  // n (int, 4 byte): per lettura da file
  unsigned int n = 0;
  // *p: punta al 1° byte di n (int, 4 byte)
  unsigned char *p = (unsigned char *) &n;
  // i: scandisce l'inserimento nell'array di output
  int i = 0;
  
  // 1^ Lettura da file (4 byte alla volta, n è un int)
  fread(&n, 4, 1, fp);

  // Finché il file non termina
  for(i = 0; !feof(fp); i++) {

    /* Composizione dei successivi nuovi byte del codeword (in UTF-8) da inserire in output */

    // Essendo "input.data" file generato in little endian, nei confronti if..else..: ToLittleEndian(n) ritorna il valore di n in little endian (se necessario) per il confronto

    // Casi divisi in base al numero di bit del codepoint in UCS-4 da convertire, contandoli dal bit più significativo diverso da zero fino al bit meno significativo

    // Operatori di somma (+) e shift (<</>>) utilizzati per leggere diverse composizioni dei bit dei singoli byte in base alle modifiche necessarie per effettuare la conversione dei codepoint da UCS-4 a UTF-8

    /* * Conversioni utilizzate:
     * (bin -> int -> hex)
     * 00000000 00000000 00000000 01111111 -> 127 -> 0x7F
     * 00000000 00000000 00000111 11111111 -> 2047 -> 0x7FF
     * 00000000 00000000 11111111 11111111 -> 65535 -> 0xFFFF
     * 00000000 00011111 11111111 11111111 -> 2097151 -> 0x1FFFFF
     * 00000011 11111111 11111111 11111111 -> 67108863 -> 0x3FFFFFF
     * 01111111 11111111 11111111 11111111 -> 2147483647 -> 0x7FFFFFFF
     * 11111111 11111111 11111111 11111111 -> 4294967295 -> 0xFFFFFFFF
     */

    // - caso 1) da 0 a 7 bit -> 1 byte in UTF-8
    if(ToLittleEndian(n) <= 127) { // p[0] = p[1] = p[2] = 0
      output[i] = p[0]; // p[0] = n
    }

    // - caso 2) da 8 a 11 bit -> 2 byte in UTF-8
    else if(ToLittleEndian(n) <= 2047) { // p[0] = p[1] = 0
      output[i] = 192 + (p[1] << 2) + (p[0] >> 6);
      i++;
      output[i] = 128 + ((p[0] <<= 2) >> 2);
    }

    // - caso 3) da 12 a 16 bit -> 3 byte in UTF-8
    else if(ToLittleEndian(n) <= 65535) { // p[0] = 0
      output[i] = 224 + (p[1] >> 4);
      i++;
      output[i] = 128 + ((p[1] <<= 4) >> 2) + (p[0] >> 6);
      i++;      
      output[i] = 128 + ((p[0] <<= 2) >> 2);
    }

    // - caso 4) da 17 a 21 bit -> 4 byte in UTF-8
    else if(ToLittleEndian(n) <= 2097151) { // p[0] = 0
      output[i] = 240 + (p[2] >> 2);
      i++;
      output[i] = 128 + ((p[2] <<= 6) >> 2) + (p[1] >> 4);
      i++;
      output[i] = 128 + ((p[1] <<= 4) >> 2) + (p[0] >> 6);
      i++;
      output[i] = 128 + ((p[0] <<= 2) >> 2);
    }

    // - caso 5) da 22 a 27 bit -> 5 byte in UTF-8
    else if(ToLittleEndian(n) <= 67108863) {
      output[i] = 248 + (p[3]);
      i++;
      output[i] = 128 + (p[2] >> 2);
      i++;
      output[i] = 128 + ((p[2] <<= 6) >> 2) + (p[1] >> 4);
      i++;
      output[i] = 128 + ((p[1] <<= 4) >> 2) + (p[0] >> 6);
      i++;
      output[i] = 128 + ((p[0] <<= 2) >> 2);
    }

    // - caso 6) da 28 a 31 bit -> 6 byte in UTF-8
    else if(ToLittleEndian(n) <= 2147483647) {
      output[i] = 252 + (p[3] >> 6);
      i++;
      output[i] = 128 + ((p[3] <<= 2) >> 2);
      i++;
      output[i] = 128 + (p[2] >> 2);
      i++;
      output[i] = 128 + ((p[2] <<= 6) >> 2) + (p[1] >> 4);
      i++;
      output[i] = 128 + ((p[1] <<= 4) >> 2) + (p[0] >> 6);
      i++;
      output[i] = 128 + ((p[0] <<= 2) >> 2);
    }

    // - caso 7) 32 bit -> 7 byte in UTF-8
    else { // if(ToLittleEndian(n) <= 4294967295)
      output[i] = 254;
      i++;
      output[i] = 128 + (p[3] >> 6);
      i++;
      output[i] = 128 + ((p[3] <<= 2) >> 2);
      i++;
      output[i] = 128 + (p[2] >> 2);
      i++;
      output[i] = 128 + ((p[2] <<= 6) >> 2) + (p[1] >> 4);
      i++;
      output[i] = 128 + ((p[1] <<= 4) >> 2) + (p[0] >> 6);
      i++;
      output[i] = 128 + ((p[0] <<= 2) >> 2);
    }

    // nei casi precedenti si ha:
    // output[i]: inserimento in output di ogni byte in UTF-8

    // Lettura da file (4 byte alla volta, n è un int)
    fread(&n, 4, 1, fp);
  }

  // Chiusura file di input in lettura
  fclose(fp);

  // (In questo caso: outputFile è il utf8File del main)

  // Apertura file binario di output in scrittura
  fp = fopen(outputFile, "wb");

  // Scrittura su file dall'array di output
  fwrite(output, 1, i, fp);

  // Chiusura file di output in scrittura
  fclose(fp);
}

// ______________________________


// UTF8_decoding

void decoding(char inputFile[], char outputFile[], int outputSize) {
  
  // Apertura file binario di input in lettura
  FILE *fp = fopen(inputFile, "rb");
  
  // input[]: array contenente i dati in input (in UTF-8)
  // [outputSize*7/4] perché il file di input (in UTF-8) può avere al più 7 byte per ogni output da 4 byte (in UCS-4)
  unsigned char input[outputSize*7/4];
  // help (int, 4 byte): per scrittura su file
  // output[]: array contenente i dati in output (in UCS-4)
  // [outputSize/4] perché si inseriscono 4 byte alla volta (int)
  unsigned int help = 0, output[outputSize/4];
  // *p_help: punta al 1° byte di help (int, 4 byte)
  unsigned char *p_help = (unsigned char *) &help;
  // i: scandisce la lettura dall'array di input
  // j: scandisce l'inserimento nell'array di output
 	int i = 0, j = 0;

  // Lettura file nell'array di input
  fread(input, 1, sizeof(input) + 1, fp);
  
  // Finché il file non termina
  for(j = 0; j < outputSize / 4; j++) {

    help = 0; // per inserimento successivo, tutti i 4 byte a 0
    
    /* Composizione dei successivi nuovi 4 byte (in UCS-4) da inserire in output */

    // Nei confronti if..else..: input[i] equivale al 1° byte del codeword in UTF-8 che si sta per convertire

    // Casi divisi in base alla composizione del 1° byte del codepoint in UTF-8 da convertire, considerando la disposizione dei bit eventualmente presenti prima del primo bit a 0

    // Operatori di somma (+) e shift (<</>>) utilizzati per leggere diverse composizioni dei bit dei singoli byte in base alle modifiche necessarie per effettuare la conversione dei codepoint da UTF-8 a UCS-4

    /* * Conversioni utilizzate:
     * (bin -> int -> hex)
     * 10000000 -> 128 -> 0x80
     * 11000000 -> 192 -> 0xC0
     * 11100000 -> 224 -> 0xE0
     * 11110000 -> 240 -> 0xF0
     * 11111000 -> 248 -> 0xF8
     * 11111100 -> 252 -> 0xFC
     * 11111110 -> 254 -> 0xFE
     */

    // - caso 1) 1° byte 0xxxxxxx -> da 0 a 7 bit in UCS-4
    if(input[i] < 128) {
      // p_help[3] = p_help[2] = p_help[1] = 0
      p_help[0] = input[i];
    }

    // - caso con 1° byte 10xxxxxx non valido in UTF-8
    // else if (n < 192) // gestito automaticamente

    // - caso 2) 1° byte 110xxxxx -> da 8 a 11 bit in UCS-4
    else if(input[i] < 224) {
      // p_help[3] = p_help[2] = 0
      p_help[1] = (input[i] - 192) >> 2;
      p_help[0] = (input[i] << 6) + (input[i+1] - 128);
      i += 1;
    }

    // - caso 3) 1° byte 1110xxxx -> da 12 a 16 bit in UCS-4
    else if(input[i] < 240) {
      // p_help[3] = p_help[2] = 0
      p_help[1] = ((input[i] - 224) << 4) + ((input[i+1] - 128) >> 2);
      p_help[0] = (input[i+1] << 6) + (input[i+2] - 128);
      i += 2;
    }

    // - caso 4) 1° byte 11110xxx -> da 17 a 21 bit in UCS-4
    else if(input[i] < 248) {
      // p_help[3] = 0
      p_help[2] = ((input[i] - 240) << 2) + ((input[i+1] - 128) >> 4);
      p_help[1] = (input[i+1] << 4) + ((input[i+2] - 128) >> 2);
      p_help[0] = (input[i+2] << 6) + (input[i+3] - 128);
      i += 3;
    }

    // - caso 5) 1° byte 111110xx -> da 22 a 26 bit in UCS-4
    else if(input[i] < 252) {
      p_help[3] = input[i] - 248;
      p_help[2] = (input[i+1] << 2) + ((input[i+2] - 128) >> 4);
      p_help[1] = (input[i+2] << 4) + ((input[i+3] - 128) >> 2);
      p_help[0] = (input[i+3] << 6) + (input[i+4] - 128);
      i += 4;
    }

    // - caso 6) 1° byte 1111110x -> da 27 a 31 bit in UCS-4
    else if(input[i] < 254) {
      p_help[3] = ((input[i] - 252) << 6) + (input[i+1] - 128);
      p_help[2] = (input[i+2] << 2) + ((input[i+3] - 128) >> 4);
      p_help[1] = (input[i+3] << 4) + ((input[i+4] - 128) >> 2);
      p_help[0] = (input[i+4] << 6) + (input[i+5] - 128);
      i += 5;
    }

    // - caso 7) 1° byte 11111110 -> 32 bit in UCS-4
    else  if(input[i] == 254) {
      p_help[3] = ((input[i+1] - 128) << 6) + (input[i+2] - 128);
      p_help[2] = (input[i+3] << 2) + ((input[i+4] - 128) >> 4);
      p_help[1] = (input[i+4] << 4) + ((input[i+5] - 128) >> 2);
      p_help[0] = (input[i+5] << 6) + (input[i+6] - 128);
      i += 6;
    }
    
    // Inserimento in output di ogni gruppo di 4 byte in UCS-4
    output[j] = help;

    i++;
  }

  fclose(fp); // Chiusura file di input in lettura

  // Apertura file binario di output in scrittura
  fp = fopen(outputFile, "wb");

  // Scrittura su file dall'array di output
  fwrite(output, 1, sizeof(output), fp);

  fclose(fp); // Chiusura file di output in scrittura
}

// ______________________________


// return 1: il file esiste, return 0: il file non esiste
int fileExists(char nomeFile[]) {

  // Apertura file binario in lettura
  FILE *fp = fopen(nomeFile, "rb");
  // Se il file non è stato effettivamente aperto
  if(fp == NULL) {
    printf("Errore nell'apertura del file \"%s\"\n", nomeFile);
    return 0; // il file non esiste
  }
  fclose(fp); // Chiusura file in lettura
  return 1; // il file esiste
}


// return: la dimensione del file
int getFileSize(char nomeFile[]) {

  // Apertura file binario in lettura
  FILE *fp = fopen(nomeFile, "rb");
  // Ricerca della fine del file col file pointer
  fseek(fp, 0, SEEK_END);
  // Dimensione del file equivale alla posizione corrente del file pointer (posizionato alla fine del file)
  int fileSize = ftell(fp);
  fclose(fp); // Chiusura file in lettura
  return fileSize;
}


// return 1: file identici, return 0: file non identici
int equalFiles(char inputFile[], char outputFile[], int inputSize) {

  // Se la dimensione dei file è diversa
  if(getFileSize(inputFile) != getFileSize(outputFile))
    return 0; // file non identici

  unsigned char input[inputSize], output[inputSize];
  // Apertura dei file binari di input e output in lettura
  FILE *fp1 = fopen(inputFile, "rb");
  FILE *fp2 = fopen(outputFile, "rb");
  // Lettura dei 2 file in 2 array
  fread(input, 1, sizeof(input), fp1);
  fread(output, 1, sizeof(output), fp2);
  // Chiusura dei file di input e output in lettura
  fclose(fp1); fclose(fp2);
  // Per tutti gli elementi dei file
  for(int i = 0; i < inputSize; i++)
    // Se alla posizione i dei file ci sono caratteri diversi
    if(input[i] != output[i])
      return 0; // file non identici

  return 1; // file identici
}

// ______________________________


int main() {

  char inputFile[] = "input.data";
  char utf8File[] = "utf8.data";
  char outputFile[] = "output.data";

  // Se il file di input esiste (-> eseguo il programma)
  if(fileExists(inputFile) == 1) {

    // Calcolo la dimensione del file input
    int inputFileSize = getFileSize(inputFile);


    // UTF8_encoding
    encoding(inputFile, utf8File, inputFileSize);


    // UTF8_decoding
    decoding(utf8File, outputFile, inputFileSize);


    printf("Fine!\n");

    // Se i file di input e di output sono uguali
    if(equalFiles(inputFile, outputFile, inputFileSize) == 1)
      printf("I file \"%s\" e \"%s\" sono uguali!\n", inputFile, outputFile);
  }

  return 0;
}