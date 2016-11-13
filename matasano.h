#include <string.h>
#include <ctype.h>
#include <openssl/evp.h>
#include <openssl/err.h>

char * hexDecode(char *);
char * hexEncode(char *, unsigned int);
char * base64Encode(char *, unsigned int);
char * base64Decode(char *, unsigned int *);
char * hexStringXor(char *, char *);
char * hexXorBrute(char *, char *);
unsigned int existsIn(char , const char *);
int engFreqAnalysis(char *);
char *xorEncrypt(char *, unsigned int, char *);
int posIn(const char *, char );
int hammingDistance(char *, char *, int);
float engHisto(char *);
unsigned int findKeysize(char *, unsigned int, unsigned int);
char * base64XorBrute(char *, unsigned int);
void ssl_error(char *);
unsigned char * decryptAesEcb(unsigned char *, unsigned int, unsigned char *);
int detectAesEcb(char *, unsigned int);

char * hexDecode(char *in_data){
	//NOTE to future self: dont change input pointers in functions, strange things happen, make a duplicate local variable instead
	unsigned int i, j, len;
	char *data = NULL, tmp[3], *ptr = in_data;
	const char okey[] = "0123456789abcdefABCDEF";


	tmp[2] = 0x0;
	len = strlen(in_data);
	data = (char *)malloc((len / 2) + 1);

	memset(data, 0x0, (len / 2) + 1);

	// Check for invalid characters
	for(i = 0; i < len; i++)
		if(!strchr(okey, in_data[i]) && printf("Invalid character in hexDecode!\n")){
			printf("\"%s\" contains an invalid char\n", in_data);
			exit(1);
		}

	for(i = 0; i < len; i++){
		if(strlen(ptr) < 2){
			// printf("input was: %s\n", hexEncode(data, (len / 2)));
			// printf("len: %d\n", len);
			return data;
		}

		memcpy(tmp, ptr, 2);
		sscanf(tmp, "%02x", &j);
		// printf("writing %02x\n", j);
		data[i] = j;
		ptr += 2;
	}

	return data;
}

char * hexEncode(char *in_data, unsigned int len){
	unsigned int i;
	char *output = (char *)malloc((len * 2) + 1), *ptr = in_data, *origin = output, tmp[3];

	for(i = 0; i < len; i++){
		snprintf(tmp, 3, "%02x", *ptr);	// Always prints null byte
		memcpy(output, tmp, 2);

		ptr++;
		output += 2;
	}

	*output = 0x0;

	ptr = in_data;

	return origin;
}

char * base64Encode(char *in_data, unsigned int len){
	unsigned int sum = 0, pointer = 0;
	unsigned char out1, out2, out3, out4;
	char *data = in_data, *out = NULL;
	unsigned int i, count;

	const char *charset = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

	/*
	char:							'A'									'B'									'C'
						******************* ******************* *******************
	binary:		*	0 1 0 0 0 0 0 1 * * 0 1 0 0 0 0 1 0 * * 0 1 0 0 0 0 1 1 *
						******************* ******************* *******************
	decimal:					65									66									67

						*************** *************** *************** ***************
	base64:		* 0 1 0 0 0 0 * * 0 1 0 1 0 0 * * 0 0 1 0 0 1 * * 0 0 0 0 1 1 *
						*************** *************** *************** ***************
	*/

	count = len % 3;

	out = malloc(2 * len);	// Unnecessarily big and dirty but works and is easy

	for(i = 0; i < len; i += 3){

		// This will only work on little endian
		sum = ((unsigned int)data[i]) << 16;

		if((i + 1) < len)
			sum += ((unsigned int)data[i + 1]) << 8;

		if((i + 2) < len)
			sum += ((unsigned int)data[i + 2]);

		out1 = (unsigned char)(sum >> 18) & 0x3f;
		out2 = (unsigned char)(sum >> 12) & 0x3f;
		out3 = (unsigned char)(sum >> 6) & 0x3f;
		out4 = (unsigned char)(sum) & 0x3f;


		out[pointer++] = charset[out1];
		out[pointer++] = charset[out2];

		if((i + 1) < len)
			out[pointer++] = charset[out3];

		if((i + 2) < len)
			out[pointer++] = charset[out4];

	}

	if(count > 0){
		for(; count < 3; count++)
			out[pointer++] = '=';
	}

	out[pointer] = 0x0;

	return out;
}

// length of the string returned is in length paramter
// Screw newlines...
char * base64Decode(char *in_data, unsigned int *length){
	unsigned int pointer = 0, len;
	unsigned char in1, in2, in3, in4;
	char *out = NULL;
	unsigned int i = 0;
	const char *charset = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=";

	len = strlen(in_data);
	out = (char *)malloc(len - (len / 4)); // not big enough

	if(len % 4 != 0 && printf("not divisible by 4\n"))
		return NULL;

	for(i = 0; i < len; i += 4){

		in1 = posIn(charset, in_data[i]);
		in2 = posIn(charset, in_data[i + 1]);
		in3 = posIn(charset, in_data[i + 2]);
		in4 = posIn(charset, in_data[i + 3]);

		// printf("values: in1 = 0x%02x in2 = 0x%02x in3 = 0x%02x in4 = 0x%02x\n", in1 ,in2, in3, in4);

		out[pointer++] = (char)((in1 << 2) | (in2 >> 4));

		if(in3 < 64){
			out[pointer++] = (char)((in2 << 4) | (in3 >> 2));

			if(in4 < 64)
				out[pointer++] = (char)((in3 << 6) | in4);

		}
	}

	//printf("pointer = %d\n", pointer);
	out[pointer] = 0x0;
	*length = pointer;


	return out;
}

// Takes arguments in hex encoded format and returns output in hex encoded format
char * hexStringXor(char *in_data1, char *in_data2){
	unsigned int len = strlen(in_data1) / 2, i;
	char *data1 = NULL, *data2 = NULL, *output = NULL;

	if(strlen(in_data1) != strlen(in_data2)){
		printf("Size of buffer 1 and 2 differ!\n");
		exit(1);
	}

	data1 = hexDecode(in_data1);
	data2 = hexDecode(in_data2);

	output = calloc(1, len + 1);

	for(i = 0; i < len; i++)
		output[i] = data1[i] ^ data2[i];

	return hexEncode(output, len);
}

char * hexXorBrute(char *in_data, char *key){
	unsigned char i;
	unsigned int j;
	int len = (strlen(in_data) / 2);
	char *data = hexDecode(in_data), decrypted[len + 1], *mostlikely = calloc(1, len + 1);
	float result = 0, highest = 0;

	// Main loop
	for(i = 32; i < 127; i++){
		// The actual decryption
		for(j = 0; j < len; j++)
			decrypted[j] = i ^ data[j];

		result = engHisto(decrypted);

		if(result > highest && result > 0){
			strncpy(mostlikely, decrypted, len);
			highest = result;
			// printf("'%c' new high(%f): %s\n\n", i, highest, mostlikely);
			// printf("new best score is now %f\n", highest);
			*key = i;
		}
	}

	if(highest <= 0){
		printf("[!] hexXorBrute() returning NULL\n");
		return NULL;
	}

	return mostlikely;
}

// Returns the amount of times 'isThis' is in 'inThis'
unsigned int existsIn(char isThis, const char *inThis){
	unsigned int i, count = 0;

	for(i = 0; i < strlen(inThis); i++)
		if(isThis == inThis[i])
			count++;

	return count;
}

int engFreqAnalysis(char *in_data){
	const char *high_freq = "ETAOINSHRDLU "; // https://inventwithpython.com/hacking/chapter20.html
	unsigned int i, count = 0, result;
	// printf("strlen\n");
	char local[strlen(in_data)];
	// printf("entering\n");

	for(i = 0; in_data[i] != 0x0 && in_data[i] > 31 && in_data[i] < 128; i++){
		local[i] = toupper(in_data[i]);

		if((result = existsIn(local[i], high_freq)))
			count += result;
	}

	// printf("freqAnalysis string: %s\n", in_data);
	// printf("freqAnalysis: %d\n", count);

	// printf("in_data: %s\n", in_data);
	return count;
}

char *xorEncrypt(char *data, unsigned int data_len, char *key){
	unsigned int i, key_len = strlen(key);
	char *output = (char *)malloc(data_len + 1);

	for(i = 0; i < data_len; i++)
		output[i] = data[i] ^ key[i % key_len];

	output[data_len] = 0x0;

	return output;
}

int posIn(const char *string, char desired){
		char *pos;

		if(NULL == (pos = strchr(string, desired))){
			//printf("returning -1\n%02x doesn't exist\n", desired);
			return -1;
		}

		return (int)(pos - string);
}

int hammingDistance(char *string1, char *string2, int len){
	unsigned int i;
	int distance = 0;
	unsigned char value;
	// printf("in hammingDistance\n");
	// printf("string1: %s\nstring2: %s\n", hexEncode(string1, len), hexEncode(string2, len));

	for(i = 0; i < len; i++){
		value = string1[i] ^ string2[i];

		for(; value; distance++)
			value &= (value - 1);
	}

	// printf("keysize %d: %f\n\n", len, (double)(distance / (double)len));
	return distance;
}

float engHisto(char *in_data){
	unsigned int i, len = strlen(in_data);
	// http://www.data-compression.com/english.html
	const char *letters = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	const char *badchars = "#¤´^~¨$@{}+:;\%&*!?()|<>/=-0123456789";
	float freqs[] = {0.0651738, 0.0124248, 0.0217339, 0.0349835, 0.1041442, 0.0197881, \
	0.0158610, 0.0492888, 0.0558094, 0.0009033, 0.0050529, 0.0331490, 0.0202124, 0.0564513, \
	0.0596302, 0.0137645, 0.0008606, 0.0497563, 0.0515760, 0.0729357, 0.0225134, 0.0082903, \
	0.0171272, 0.0013692, 0.0145984, 0.0007836, 0.1918182};
	float total = 0;
	char data[len + 1];

	for(i = 0; i < len; i++)
		data[i] = toupper(in_data[i]);

	data[i] = 0x0;

	// printf("data: %s\n", data);

	// Main loop
	for(i = 0; i < len; i++){
		if(data[i] == 0x0a)
			continue;

		else if(data[i] == 0x0 || data[i] < 32 || data[i] > 126){
			// printf("left early at %d char: 0x%02x\n", i, data[i]);
			return total;
		}

		if(!existsIn(data[i], data))
			printf("%c not in string\n", data[i]);

	// Can't do the 0x41 offset thing with space
		if(data[i] == ' '){
			total += freqs[26];
			continue;
		}
		else if(!existsIn(data[i], letters)){
			if(existsIn(data[i], badchars) && total > -1)
				total -= 0.0155419;	// Make it a bit easier to spot the bad strings, value is half average of all the freqs values (except for space)

			continue;
		}

		if(data[i] < 0x41)
			printf("WHOOPSIE!!\n");
		total += (freqs[data[i] - 0x41]);
	}

	//printf("total: %f\n", total);
	return total;
}

unsigned int findKeysize(char *data, unsigned int len, unsigned int max_keysize){
  unsigned int keysize, i, j, bestsize;
  float tmp, best = 10000;

  for(keysize = 2, i = 0; keysize < max_keysize; keysize++, i++){
    tmp = 0;
    for(j = 0; j < 9; j++)
      tmp += hammingDistance(&data[j * keysize], &data[(j + 1) * keysize], keysize) / (float)keysize;

    tmp /= 10;
    if(tmp < best){
      // printf("new best: %f  keysize: %d\n", tmp, keysize);
      best = tmp;
      bestsize = keysize;
    }
    // printf("distance: %f  keysize: %d\n", tmp, keysize);
  }

  return bestsize;
}

// base64XorCrack would be a better name
// Returns the key
char * base64XorBrute(char *in_data, unsigned int max_keysize){
	unsigned int keysize, len = 0;
	int j, k;
	char *data = base64Decode(in_data, &len), *ptr = data;
	char **blocks = NULL, **transposed = NULL, *final_key = NULL;

	if(data == NULL)
		return NULL;

	if(max_keysize < 2)
		return NULL;

  keysize = findKeysize(data, len, max_keysize);


  final_key = calloc(1, keysize + 1);
	blocks = malloc((len / keysize) * sizeof(char *));
	transposed = malloc(keysize * sizeof(char *));

  printf("keysize is: %d message length is: %d\n", keysize, len);

	// Break the ciphertext into blocks of keysize length
	for(j = 0; j < (len / keysize); j++){
		blocks[j] = malloc(keysize + 1);
		memcpy(blocks[j], ptr, keysize);
		blocks[j][keysize] = 0x0;
		ptr += keysize;
	}

	// printf("%d blocks of length %d created\n", (len / keysize), keysize);

	// Do the transposing
	for(j = 0; j < keysize; j++){
		transposed[j] = malloc((len / keysize) + 1);
		for(k = 0; k < (len / keysize); k++)
			transposed[j][k] = blocks[k][j];

		transposed[j][keysize] = 0x0;
	}

	//printf("transposed\n");


	// Break each transposed block as a single byte xor
	for(j = 0; j < keysize; j++){	// For repeating through each block

    //  Returns the key for that block in final_key[j]
     if(hexXorBrute(hexEncode(transposed[j], keysize), &final_key[j]) == NULL)
      printf("uuuuh\n");

     printf("key %d: '%c'(0x%02x)\n", j, final_key[j], final_key[j]);
   }

	printf("Done! Key is: \"%s\"\n\n", final_key);
	//printf("Decrypted is: %s\n", xorEncrypt(data, len, final_key));

	return final_key;
}

void ssl_error(char *mesg){
  printf("%s\n", mesg);
  ERR_print_errors_fp(stderr);

  exit(1);
}

unsigned char * decryptAesEcb(unsigned char *in_data, unsigned int cipherlen, unsigned char *key){
	EVP_CIPHER_CTX *ctx;
	int len;
	unsigned char *plaintext = calloc(1, cipherlen + 1);

	if(!(ctx = EVP_CIPHER_CTX_new()))
		ssl_error("Error create context");

	if(1 != EVP_DecryptInit_ex(ctx, EVP_aes_128_ecb(), NULL, key, NULL))
		ssl_error("Error initializing");

	if(1 != EVP_DecryptUpdate(ctx, plaintext, &len, in_data, cipherlen))
		ssl_error("Error in EVP_DecryptUpdate");

	printf("wrote %d plaintext bytes\n", len);

	if(1 != EVP_DecryptFinal_ex(ctx, plaintext + len, &len))
		ssl_error("Error finalizing decryption");

	printf("wrote %d more plaintext bytes\n", len);

	EVP_CIPHER_CTX_free(ctx);
	return plaintext;
}

int detectAesEcb(char *in_data, unsigned int len){
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
