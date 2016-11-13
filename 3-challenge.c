#include <stdio.h>
#include <stdlib.h>
#include "matasano.h"

int main(int argc, char **argv){
	char *result, key, cipher[] = "1b37373331363f78151b7f2b783431333d78397828372d363c78373e783a393b3736";

	result = hexXorBrute(cipher, &key);
	printf("\nKey was: char: '%c' decimal: %d hex: 0x%02x\n", key, key, key);
	printf("Decrypted ciphertext: \"%s\"\n", result);

	return 0;
}
