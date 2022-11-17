#include "gzip.h"

void usage(char* command)
{
	printf("Usage: %s <input file>\n", command);
}

int main(int argc, char** argv)
{
	FILE *input_file = NULL;
	uint8_t *input_data = NULL;
	size_t input_size = 0;
	uint8_t *curr = NULL;
	vector<size_t> *offsets;

	//Argc must be 2
	if(argc != 2)
	{
		usage(argv[0]);
		return 1;
	}

	//Try to open file
	input_file = fopen(argv[1], "rb");
	if(!input_file)
	{
		printf("Error opening %s\n", argv[1]);
		usage(argv[0]);
		return 1;
	}

	//Read entire file
	fseek(input_file, 0, SEEK_END);
	input_size = ftell(input_file);
	rewind(input_file);
	input_data = new uint8_t[input_size];
	fread(input_data, input_size, 1, input_file);	
	fclose(input_file);

	offsets = find_gzip_magic_offsets(input_data, input_size);
	int result = extract_gzip_data(input_data, input_size, offsets);
	if(result != Z_OK)
	{
		fprintf(stderr, "Error extracting gzip data\n");
		return result;
	}

	delete[] input_data;
	delete offsets;
	return 0;
}
