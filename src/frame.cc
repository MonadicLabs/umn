
#include "frame.h"

uint32_t umn::Frame::adler32(unsigned char *data, size_t len)
{
	const int MOD_ADLER = 65521;
	uint32_t a = 1, b = 0;
    	size_t index;
    	/* Process each byte of the data in order */
    	for (index = 0; index < len; ++index)
    	{
        	a = (a + data[index]) % MOD_ADLER;
        	b = (b + a) % MOD_ADLER;
    	}
    	return (b << 16) | a;
}
