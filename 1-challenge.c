// Compile with: gcc 1-challenge.c -o 1-challenge -lcrypto

#include <stdio.h>
#include <stdlib.h>
#include "matasano.h"

int main(int argc, char **argv){
	char hexstring[] = "49276d206b696c6c696e6720796f757220627261696e206c696b65206120706f69736f6e6f7573206d757368726f6f6d";

	printf("Base64: %s\n", base64Encode(hexDecode(hexstring), strlen(hexstring) / 2));

	return 0;
}
