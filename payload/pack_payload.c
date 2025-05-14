#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "../loader/log.h"

uint32_t invert_byteorder(uint32_t data) {
	return (data & 0x000000FF) << 24u | (data & 0x0000FF00) << 8u | (data & 0x00FF0000) >> 8u | (data & 0xFF000000) >> 24u;
}

uint32_t checksum(uint8_t* addr, uint32_t size)
{
	uint32_t sum = 0;
	while (size-- != 0)
		sum -= *addr++;
	return sum;
}

int main(int argc, char *argv[]) {
	if (argc != 4) {
		log_error_file("[pack_payload] Usage: %s EGG input.bin output.bin", argv[0]);
		exit(1);
	}

	FILE* input = fopen(argv[2], "rb");
	if (input == NULL) {
		log_error_file("[pack_payload] Error opening input file: %s", argv[2]);
		exit(2);
	}

	FILE* output = fopen(argv[3], "wb");
	if (output == NULL) {
		log_error_file("[pack_payload] Error opening output file: %s", argv[3]);
		fclose(input);
		exit(3);
	}

	fseek(input, 0L, SEEK_END);
	uint32_t size = ftell(input);
	rewind(input);

	uint8_t* bytes = (uint8_t*)malloc(size);
	if (!bytes){
		log_error_file("[pack_payload] Failed to allocate %u bytes!", size);
		fclose(input);
		fclose(output);
		exit(4);
	}
	
	if (fread(bytes, size, 1, input) != 1) {
		log_error_file("[pack_payload] Failed to read %u bytes from input file", size);
		free(bytes);
		fclose(input);
		fclose(output);
		exit(5);
	}

	fclose(input);
	input = NULL;

	uint32_t chk = checksum(bytes, size);

	printf("EGG: %s\n", argv[1]);
	printf("Size: 0x%.8X\n", size);
	printf("Checksum: 0x%.8X\n", chk);

	// Convert to big endian
	uint32_t big_size = invert_byteorder(size);
	uint32_t big_chk = invert_byteorder(chk);

	if (fwrite(argv[1], 4, 1, output) != 1 ||
	    fwrite(argv[1], 4, 1, output) != 1 ||
	    fwrite(&big_size, 4, 1, output) != 1 ||
	    fwrite(&big_chk, 4, 1, output) != 1 ||
	    fwrite(bytes, size, 1, output) != 1) {
		log_error_file("[pack_payload] Failed to write output file");
		free(bytes);
		fclose(output);
		exit(6);
	}

	free(bytes);
	bytes = NULL;

	fclose(output);
	output = NULL;

	return 0;
}
