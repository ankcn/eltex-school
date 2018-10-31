#include "complex.h"


mod_info_t INFO_VAR = {
	.fname = "compl_plus",
	.opname = "Addition",
	.opsym = '+' };


// Сложение двух комплексных чисел
complex compl_plus(const complex a, const complex b)
{
	complex t;
	t.re = a.re + b.re;
	t.im = a.im + b.im;
	return t;
}

