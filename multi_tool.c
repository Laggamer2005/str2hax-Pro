#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <gmp.h> //lazy cop out for hex -> int conversion
#include "loader/log.h"

void writeStringToFile(char *filename, char *input){
	FILE *output = fopen(filename, "w");
	if (!output){
		log_error_file("[multi_tool] Failed to open %s for writing in writeStringToFile", filename);
		exit(1);
	}
	fwrite(input, strlen(input), 1, output);
	fclose(output);
}

void do_k(int k, int size, char *filename){
	int x = 0;
	if (size){
		x = 1 << k;
	}else{
		x = (1 << (k - 1)) + 1;
	}
	x = (x * 9) - 8;
	printf("Decimal size: %d\n", x);
	
	char* input = (char*)malloc(x + 1);
	
	if (!input){
		log_error_file("[multi_tool] Couldn't malloc(%d) bytes in do_k", x);
		exit(2);
	}
	
	memset(input, '0', x);
	input[x] = 0x00;
	
	writeStringToFile(filename, input);
	
	free(input);
}

void do_c(char *inputFilename, char *outputFilename){
	mpz_t trans;
	mpz_init(trans);
	
	FILE *input = fopen(inputFilename, "r");
	if (!input){
		log_error_file("[multi_tool] Couldn't open %s for input file in do_c", inputFilename);
		exit(3);
	}
	fseek(input, 0L, SEEK_END);
	size_t length = ftell(input);
	rewind(input);
	
	uint64_t* bytes = (uint64_t*)malloc(length + 8);
	if (!bytes){
		log_error_file("[multi_tool] Failed to allocate %ld bytes in do_c!", length);
		exit(4);
	}
	uint64_t* reversedBytes = (uint64_t*)malloc(length + 8);
	if (!reversedBytes){
		log_error_file("[multi_tool] Failed to allocate %ld bytes in do_c!", length);
		free(bytes);
		exit(5);
	}
	fread(bytes, 1, length, input);
	bytes[length/8] = 0x00000000;
	
	fclose(input);
	
	for (int i = 0; i < length/8; i++){
		reversedBytes[i] = bytes[(length/8)-i-1];
	}
	
	free(bytes);
	bytes = NULL;
	
	reversedBytes[length/8] = 0x00000000;
	
	/*input = fopen("test.bin", "wb");
	fwrite(reversedBytes, strlen(reversedBytes), 1, input);
	fclose(input);*/
	
	mpz_set_str(trans, (char*)reversedBytes, 16);
	
	free(reversedBytes);
	reversedBytes = NULL;
	
	FILE *output = fopen(outputFilename, "w");
	if (!output){
		log_error_file("[multi_tool] Couldn't open %s for output file in do_c", outputFilename);
		exit(6);
	}
	
	mpz_out_str(output, 10, trans);
	
	fclose(output);
	
	mpz_clear(trans);
}

void do_s(int target_k){
	printf("k min d / max d (inclusive)\n");
	for (int k = 1, y = 1; k <= target_k; k++, y <<= 1){
		printf("%d %d / %d\n", k, ((y + 1) * 9) - 8, ((y << 1) * 9) - 8);
	}
}

int main(int argc, char *argv[]){
	
	if (argc == 1){
		printf("Usage: %s [operation] [operation args]\n", argv[0]);
		printf("operations:\n");
		printf("\tk min/max # output.bin\t\tOutput a file with the correct number of decminals to reach a k=# allocation\n");
		printf("\t\t\t\t\tmin/max change whether the decmial is just enough or just below limit\n");
		printf("\n");
		printf("\tc input.bin output.bin\t\tConverts binary input file to decimal needed\n");
		printf("\n");
		printf("\ts\t\t\t\tDisplays decminal lengths for all possible values of k\n");
		return 0;
	}
	
	char operation = *argv[1];
	
	switch (operation){
		case 'k':
			if (argc != 5){
				log_error_file("[multi_tool] Invalid arguments for operation 'k'");
				exit(7);
			}
			do_k(atoi(argv[3]), strncmp("min", argv[2], 4) ? 1 : 0, argv[4]);
			return 0;
		case 'c':
			if (argc != 4){
				log_error_file("[multi_tool] Invalid arguments for operation 'c'");
				exit(8);
			}
			do_c(argv[2], argv[3]);
			break;
		case 's':
			if (argc != 3){
				log_error_file("[multi_tool] Invalid arguments for operation 's'");
				exit(9);
			}
			do_s(atoi(argv[2]));
			break;
		default:
			log_error_file("[multi_tool] Unknown operation");
			exit(10);
	}
	
	return 0;
}
