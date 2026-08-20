#include <stdio.h>
#define STSE_OMIT_NS
#define STSE_IMPLEMENTATION
#include "../stse.h"
#include <stdint.h>
#include <math.h>


int main(void) {
	float d1 = 8.2f;
	for (float n = -d1 - 0.01f; n < 3*d1; n += 0.069f) {
		float m = fmodf_cycling(n, d1);
		float diff = m - (n < 0 ? d1 + fmodf(n, d1) : fmodf(n, d1));
		if (diff != 0) printf("float error on n=%f, d=%f, m=%f, math.h fmodf=%f\n", n, d1, m, fmodf(n, d1));
	}
	double d2 = 8.2;
	for (double n = -d2 - 0.01; n < 3*d2; n += 0.069) {
		double m = fmod_cycling(n, d2);
		double diff = m - (n < 0 ? d2 + fmod(n, d2) : fmod(n, d2));
		if (diff != 0) printf("double error on n=%f, d=%f, m=%f, math.h fmod=%f\n", n, d2, m, fmod(n, d2));
	}
#ifndef _MSC_VER // microsoft doesn't do long doubles
#ifdef __MINGW32__
  #define PRINTF __mingw_printf
#else
  #define PRINTF printf
#endif
	long double d3 = 8.2;
	for (long double n = -d3 - 0.01; n < 3*d3; n += 0.069) {
		long double m = fmodl_cycling(n, d3);
		long double diff = m - (n < 0 ? d3 + fmodl(n, d3) : fmodl(n, d3));
		if (diff != 0) PRINTF("long double error on n=%Lf, d=%Lf, m=%Lf, math.h fmodl=%Lf\n", n, d3, m, fmodl(n, d3));
	}
#endif /* _MSC_VER */
}
