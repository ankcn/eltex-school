#include "complex.h"


mod_info_t INFO_VAR = {
	.fname = "compl_divide",
	.opname = "Division",
	.opsym = '/' };


// Деление комплексных чисел
complex compl_divide(const complex a, const complex b)
{
	complex t;
	if (b.re || b.im) {
		t.re = (a.re * b.re + a.im * b.im) / (b.re * b.re + b.im * b.im);
		t.im = (a.im * b.re + a.re * b.im) / (b.re * b.re + b.im * b.im);
	}
	else
;//		printf("Division by zero\n");
	return t;
}
