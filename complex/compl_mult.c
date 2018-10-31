#include "complex.h"


mod_info_t INFO_VAR = {
	.fname = "compl_mult",
	.opname = "Multiplication",
	.opsym = '*' };


// Умножение комплексных чисел
complex compl_mult(const complex a, const complex b)
{
	complex t;
	t.re = a.re * b.re - a.im * b.im;
	t.im = a.re * b.im + a.im * b.re;
	return t;
}

