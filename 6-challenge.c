#include <stdio.h>
#include <stdlib.h>
#include "matasano.h"

int main(int argc, char **argv){
	FILE *fp = fopen("6.txt", "r");
	long int len;
	unsigned int decodelen;
	char *buf = NULL, *key = NULL, *decoded = NULL;

	fseek(fp, 0, SEEK_END);
	len = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	buf = calloc(1, len + 1);
	fread(buf, len, 1, fp);
	fclose(fp);

	key = base64XorBrute(buf, 40);
	decoded = base64Decode(buf, &decodelen);
	printf("Decrypted: \"%s\"\n", xorEncrypt(decoded, decodelen, key));

	return 0;
}
