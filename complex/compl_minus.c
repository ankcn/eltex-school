#include "complex.h"


mod_info_t INFO_VAR = {
	.fname = "compl_minus",
	.opname = "Subtraction",
	.opsym = '-' };


// Вычитание комплексных чисел
complex compl_minus(const complex a, const complex b)
{
	complex t;
	t.re = a.re - b.re;
	t.im = a.im - b.im;
	return t;
}
