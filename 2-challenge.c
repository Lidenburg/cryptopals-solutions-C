// Compile with: gcc 2-challenge.c -o 2-challenge -lcrypto

#include <stdio.h>
#include <stdlib.h>
#include "matasano.h"

int main(int argc, char **argv){

	printf("Xor'd result: %s\n", hexStringXor("1c0111001f010100061a024b53535009181c", "686974207468652062756c6c277320657965"));

	return 0;
}
