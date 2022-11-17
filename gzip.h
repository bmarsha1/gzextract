#define GZ_HDR_1	0x1F
#define GZ_HDR_2 	0x8B
#define GZ_DEFLATE	0x8
#define GZ_HDR_SIZE	10

#define CHUNK 256 * 1024

#include <vector>
#include <stdint.h>
#include <stdio.h>

#include "zlib.h"

using namespace std;

vector<size_t>* find_gzip_magic_offsets(uint8_t *input_data, size_t input_size);

int extract_gzip_data(uint8_t *input_data, size_t input_size, vector<size_t> *magic_offsets);

