#include <stdio.h>
#include <stdlib.h>
#include "matasano.h"

int main(int argc, char **argv){
	int i;
	FILE *fp;
	char *result, *top = NULL;
	char buf[100], key;
	float count = 0, highest = 0;

	if(NULL == (fp = fopen("4.txt", "r")) && printf("Failed to open file!\n"))
		exit(1);

	while(1 == fscanf(fp, "%s\n", buf)){
			if((result = hexXorBrute(buf, &key)) == NULL)
				continue;

			// printf("pre freqAnalysis result: %s\n", result);

			count = engHisto(result);

			// printf("count: %d\n", count);

			if(count > highest){
				//printf("%s is highest now with %f\n", result, count);
				highest = count;
				top = result;
			}

	}
	printf("\nresult: %s\n", top);
	return 0;
}
