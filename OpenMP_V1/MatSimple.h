#ifndef _MATSIMPLE_
#define _MATSIMPLE_

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

double metodoRectangulo(double a, int n, double delta, double (*f)(double));
double metodoPMedio(double a, int n, double delta, double (*f)(double));
double metodoTrapecio(double a, double b, int n, double delta, double (*f)(double));
double metodoSimpson(double a, double b, int n, double delta, double (*f)(double));

#endif
