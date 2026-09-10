#include "compare.h"
#include <string.h>

size_t smin(const size_t a, const size_t b);

char is_similar(const char* stra, const char* strb, float match) {
	const size_t size = smin(strlen(stra), strlen(strb));
	size_t similarities = 0;
	for (size_t c = 0; c < size; c++) {
		if (stra[c] == strb[c])
			similarities++;
	}
	float proportion = ((float)similarities) / ((float)size);
	return proportion >= match;
}


size_t smin(const size_t a, const size_t b) {
	return a < b ? a : b;
}