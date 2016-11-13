#include <stdio.h>
#include <stdlib.h>
#include "matasano.h"

char *xorENcrypt(char *data, unsigned int data_len, char *key){
     unsigned int i, key_len = 8;
     char *output = (char *)malloc(data_len + 1);

     for(i = 0; i < data_len; i++)
         output[i] = data[i] ^ key[i % key_len];

     output[data_len] = 0x0;

     return output;
}

int main(int argc, char **argv){
	char *result, *string = "Burning 'em, if you ain't quick and nimble\nI go crazy when I hear a cymbal", key[] = "ICE";
	unsigned int len;

	printf("Encryped: %s\n", hexEncode(xorEncrypt(string, strlen(string), key), strlen(string)));

	return 0;
}
