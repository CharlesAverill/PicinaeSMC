#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "lz4.h"

int main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, "usage: %s in.bin out.lz4\n", argv[0]);
        return 2;
    }
    FILE *fin = fopen(argv[1], "rb");
    if (!fin) { perror("fopen"); return 2; }
    fseek(fin, 0, SEEK_END);
    long inSize = ftell(fin);
    fseek(fin, 0, SEEK_SET);
    char *inBuf = malloc(inSize);
    if (!inBuf) { perror("malloc"); return 2; }
    if (fread(inBuf, 1, inSize, fin) != (size_t)inSize) { perror("fread"); return 2; }
    fclose(fin);

    int maxOut = LZ4_compressBound((int)inSize);
    char *outBuf = malloc(maxOut);
    if (!outBuf) { perror("malloc"); return 2; }
    int outSize = LZ4_compress_default(inBuf, outBuf, (int)inSize, maxOut);
    if (outSize <= 0) { fprintf(stderr, "compression failed\n"); return 2; }

    FILE *fout = fopen(argv[2], "wb");
    if (!fout) { perror("fopen out"); return 2; }
    fwrite(outBuf, 1, outSize, fout);
    fclose(fout);

    free(inBuf);
    free(outBuf);
    return 0;
}
