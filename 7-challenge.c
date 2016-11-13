#include <stdio.h>
#include <stdlib.h>
#include "matasano.h"

int main(int argc, char **argv){
	FILE *fp = fopen("7.txt", "r");
	long int len;
	unsigned int cipherlen;
	char *buf = NULL, *returned;

	fseek(fp, 0, SEEK_END);
	len = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	buf = malloc(len + 1);
	fread(buf, len, 1, fp);
	fclose(fp);
	buf[len] = 0x0;

	returned = base64Decode(buf, &cipherlen);
	printf("decrypted: %s\n", decryptAesEcb(returned, cipherlen, "YELLOW SUBMARINE"));

	return 0;
}
