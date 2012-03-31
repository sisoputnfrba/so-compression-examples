#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <lzo/lzo1x.h>
#include <snappy-c.h>
#include <assert.h>

void test_snappy(const char* data, size_t data_size) {
	snappy_status ret;

	size_t outlen = snappy_max_compressed_length(data_size);
	char *out = malloc(outlen);

	ret = snappy_compress(data, data_size, out, &outlen);

	assert( ret == SNAPPY_OK);

	assert( outlen <= snappy_max_compressed_length( data_size ));

	printf("[Snappy] - compressed %lu bytes into %lu bytes\n", (unsigned long) data_size, (unsigned long) outlen);

	char *buff = malloc(data_size);
	size_t bufflen = data_size;
	ret = snappy_uncompress(out, outlen, buff, &bufflen);

	assert( ret == SNAPPY_OK);
	assert( bufflen == data_size);
	assert( memcmp(data, buff, data_size) == 0);

	free(buff);
	free(out);
}

void test_lzo(const char* data, size_t data_size) {
	void *wrkmem = malloc(LZO1X_1_MEM_COMPRESS);

	size_t outlen = data_size + data_size / 16 + 64 + 3;
	char *out = malloc(outlen);

	int ret = lzo1x_1_compress((lzo_bytep) data, (lzo_uint) data_size, (lzo_bytep) out, (lzo_uintp) &outlen, (lzo_voidp) wrkmem);

	assert( ret == LZO_E_OK);

	printf("[LZO] - compressed %lu bytes into %lu bytes\n", (unsigned long) data_size, (unsigned long) outlen);

	char *buff = malloc(data_size);
	size_t bufflen = data_size;
	ret = lzo1x_decompress((lzo_bytep) out, (lzo_uint) outlen, (lzo_bytep) buff, (lzo_uintp) &bufflen, NULL);

	assert( ret == LZO_E_OK);
	assert( bufflen == data_size);
	assert( memcmp(data, buff, data_size) == 0);

	free(wrkmem);
	free(buff);
	free(out);
}

int main(void) {
	size_t data_size = 512 * 1024;
	const char *data = malloc(data_size);

	// Esta función solo debe ser invocada una sola vez
	lzo_init();

	test_snappy(data, data_size);
	test_lzo(data, data_size);

	free(data);

	return EXIT_SUCCESS;
}
