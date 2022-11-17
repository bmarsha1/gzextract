#include "gzip.h"

#include <string>
#include <iomanip>
#include <stdlib.h>
#include <cassert>

vector<size_t>* find_gzip_magic_offsets(uint8_t *input_data, size_t input_size)
{
	vector<size_t> *result = new vector<size_t>();
	uint8_t *curr = NULL;
	
	//Loop through file and look for GZ headers
	uint8_t *end = input_data + input_size;
	for(curr = input_data; (curr + GZ_HDR_SIZE) < end; )
	{
		size_t offset = curr - input_data;
		//Look for magic and deflate
		if(*curr++ == GZ_HDR_1 && *curr++ == GZ_HDR_2 && *curr++ == GZ_DEFLATE)
		{
			printf("Gzip magic found at %x\n", offset);
			result->push_back(offset);
		}
	}

	return result;
}

int extract_gzip_data(uint8_t *input_data, size_t input_size, vector<size_t> *magic_offsets)
{
	int ret;
	size_t have;
	z_stream strm;
	uint8_t out[CHUNK];
		
	//Set up zlib stream
	strm.zalloc = Z_NULL;
	strm.zfree = Z_NULL;
	strm.opaque = Z_NULL;
	strm.avail_in = 0;
	strm.next_in = Z_NULL;

	//32 for Gzip header detection
	ret = inflateInit2(&strm, 32);
	if(ret != Z_OK)
	{
		fprintf(stderr, "Error initializing inflate\n");
		return ret;
	}

	size_t end_offset = 0;
	
	for(int i = 0; i < magic_offsets->size(); i++)
	{
		
		size_t offset = (*magic_offsets)[i];
		
		if(offset < end_offset)
		{
			printf("Skipping offset %x\n", offset);
			continue;
		}

		printf("Extracting at offset %x\n", offset);

		stringstream fnamestream;
		fnamestream << hex << offset;
		string fname = fnamestream.str();
		FILE *dest = fopen(fname.c_str(), "wb");
		if(dest == NULL)
		{
			fprintf(stderr, "Error opening file %s\n", fname.c_str());
			continue;
		}

		strm.avail_in = input_size - offset;
		strm.next_in = input_data + offset;

		do
		{
			strm.avail_out = CHUNK;
			strm.next_out = out;
			ret = inflate(&strm, Z_NO_FLUSH);
			assert(ret != Z_STREAM_ERROR);
			switch(ret)
			{
				case Z_NEED_DICT:
					ret = Z_DATA_ERROR;
				case Z_DATA_ERROR:
				case Z_MEM_ERROR:
					fprintf(stderr, "Error extracting %x\n", offset);
					goto done;
			}
			
			have = CHUNK - strm.avail_out;
			if(fwrite(out, 1, have, dest) != have || ferror(dest))
			{
				fprintf(stderr, "Error writing to file %s\n", fname.c_str());
				goto done;	
			}

		}while(strm.avail_out == 0);

done:
		end_offset = offset + strm.total_in;
		inflateReset(&strm);
		fclose(dest);
	}

	inflateEnd(&strm);
	return Z_OK;
}
