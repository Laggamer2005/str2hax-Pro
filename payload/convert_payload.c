#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "../loader/log.h"

int main(int argc, char *argv[]) {
	if (argc != 3) {
		log_error_file("[convert_payload] Usage: %s in.bin out.bin", argv[0]);
		exit(1);
	}

	FILE *input = fopen(argv[1], "rb");
	if (!input) {
		log_error_file("[convert_payload] Error opening input file: %s", argv[1]);
		exit(2);
	}

	FILE *output = fopen(argv[2], "wb");
	if (!output) {
		log_error_file("[convert_payload] Error opening output file: %s", argv[2]);
		fclose(input);
		exit(3);
	}

	fseek(input, 0L, SEEK_END);
	size_t size = ftell(input);
	rewind(input);

	uint8_t* bytes = (uint8_t*)malloc(size);
	if (!bytes){
		log_error_file("[convert_payload] Failed to allocate %ld bytes!", size);
		fclose(input);
		fclose(output);
		exit(4);
	}
	
	if (fread(bytes, 1, size, input) != size) {
		log_error_file("[convert_payload] Failed to read %ld bytes from input file", size);
		free(bytes);
		fclose(input);
		fclose(output);
		exit(5);
	}

	for (size_t i = 0; i + 3 < size; i += 4) {
		uint8_t a = bytes[i];
		uint8_t r = bytes[i+1];
		uint8_t g = bytes[i+2];
		uint8_t b = bytes[i+3];
		bytes[i] = r;
		bytes[i+1] = g;
		bytes[i+2] = b;
		bytes[i+3] = a;
	}

	if (fwrite(bytes, size, 1, output) != 1) {
		log_error_file("[convert_payload] Failed to write %ld bytes to output file", size);
		free(bytes);
		fclose(input);
		fclose(output);
		exit(6);
	}

	free(bytes);
	bytes = NULL;

	fclose(input);
	input = NULL;
	fclose(output);
	output = NULL;

	return 0;
}
