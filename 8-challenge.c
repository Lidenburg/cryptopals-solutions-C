#include <stdio.h>
#include <stdlib.h>
#include "matasano.h"

/*int detectAesEcb(char *in_data, unsigned int len){
	unsigned int i, j, chunksize = 8;
	char chunk[chunksize + 1];
	chunk[chunksize] = 0x0;

	for(i = 0; i < (len / chunksize); i++){
		strncpy(chunk, &in_data[i], chunksize);
		for(j = 0; j < strlen(&in_data[j + chunksize]); j++){
			// printf("compared '%s' with '%.8s'\n", chunk, &in_data[i + j + chunksize]);
			if(!strncmp(chunk, &in_data[i + j + chunksize], chunksize)){
				return 1;
			}
		}
	}
	return 0;
}
*/

int main(int argc, char **argv){
	FILE *fp = fopen("8.txt", "r");
	unsigned int i;
	char *buf = calloc(1, 1000), *returned = NULL;

	for(i = 0; 1 == fscanf(fp, "%s\n", buf); i++){
		returned = hexDecode(buf);
		if(detectAesEcb(returned, strlen(buf) / 2))
			printf("row #%d was (probably) encrypted in ecb mode\n", i + 1); // +1 because i starts at 0 and row count starts at 1
	}
	return 0;
}
