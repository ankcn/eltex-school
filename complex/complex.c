#include <stdio.h>
#include "complex.h"


complex compl_plus(const complex a, const complex b)
{
	complex t;
	t.re = a.re + b.re;
	t.im = a.im + b.im;
	return t;
}


complex compl_minus(const complex a, const complex b)
{
	complex t;
	t.re = a.re - b.re;
	t.im = a.im - b.im;
	return t;
}


complex compl_mult(const complex a, const complex b)
{
	complex t;
	t.re = a.re * b.re - a.im * b.im;
	t.im = a.re * b.im + a.im * b.re;
	return t;
}


complex compl_divide(const complex a, const complex b)
{
	complex t;
	if (b.re || b.im) {
		t.re = (a.re * b.re + a.im * b.im) / (b.re * b.re + b.im * b.im);
		t.im = (a.im * b.re + a.re * b.im) / (b.re * b.re + b.im * b.im);
	}
	else
		printf("Division by zero\n");
	return t;
}


void compl_print(const complex a)
{
	printf("%.3f", a.re);
	float t = a.im;
	if (t < 0)
		t = -a.im;
	if (t)
		printf(" %c j%.3f", (a.im < 0) ? '-' : '+', t);
}
