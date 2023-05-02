#include <stdio.h>
#include <stdint.h>
#include <string.h>

FILE* fpi;
FILE* fpo;
uint32_t size = 0;

char* riff = "RIFF";
uint32_t fileSize = 0;
char* wave = "WAVEfmt ";
uint32_t format = 16;
uint16_t pcm = 1;
uint16_t channel = 1;
uint32_t sampleRate = 8000;
uint32_t sampleRateBC = 8000;
uint16_t bitsC = 1;
uint16_t bits = 8;
char* data = "data";

int main(int argc, char* argv[]) {
    
    fpi = fopen(argv[1], "rb");
    fseek(fpi, 0, SEEK_END);
    size = ftell(fpi);
    fileSize = size+36;
    fseek(fpi, 0, SEEK_SET);
    if(size <= 0){
        fclose(fpi);
        printf("Empty file"); 
        return 0;}
    printf("File size: %d\n", size); 

    char* filename = argv[1];
    int i = 0;
    for(; filename[i] != 0; i++) {}
    for(; (filename[i] != '.') && (i > 0); i--) {}
    filename[i]=0;
    strcat(filename, ".wav");
    printf("File name: %s\n", filename);

    fpo=fopen(filename, "wb");
    fwrite(riff, 4, 1, fpo);
    fwrite(&fileSize, 4, 1, fpo);
    fwrite(wave, 8, 1, fpo);
    fwrite(&format, 4, 1, fpo);
    fwrite(&pcm, 2, 1, fpo);
    fwrite(&channel, 2, 1, fpo);
    fwrite(&sampleRate, 4, 1, fpo);
    fwrite(&sampleRateBC, 4, 1, fpo);
    fwrite(&bitsC, 2, 1, fpo);
    fwrite(&bits, 2, 1, fpo);
    fwrite(data, 4, 1, fpo);
    fwrite(&size, 4, 1, fpo);

    int c = 0;
    while((c = fgetc(fpi)) != EOF)
        fputc((unsigned char)(c + 128), fpo);
    fclose(fpi);
    fclose(fpo);

    return 0;
}