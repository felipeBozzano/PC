#include "MatSimple.h"

/* Regla del rectángulo */
double metodoRectangulo(double a, int n, double delta, double (*f)(double)) {

    double x = 0.0, resultado = 0.0;

    for(int i = 0; i < n; i++) {
        x = delta * i + a;
        resultado += f(x);
    }

    resultado *= delta;

    return resultado;
}

/* Regla del Punto Medio */
double metodoPMedio(double a, int n, double delta, double (*f)(double)) {

    double x = 0.0, resultado = 0.0;
    int i;

    for(i = 1; i <= n; i++) {
        x = (delta*(i-1) + delta*i + 2*a) / 2;
        resultado += f(x);
    }

    resultado *= delta;

    return resultado;
}

/* Regla del Trapecio */
double metodoTrapecio(double a, double b, int n, double delta, double (*f)(double)) {

    double x = 0.0, resultado = 0.0;
    
    int i;

    for(i = 1; i <= n-1; i++) {
        x = delta * i + a;
        resultado += 2*f(x);
    }

    resultado += f(a) + f(b);
    resultado *= delta/2;

    return resultado;
}

/* Simpson 1/3 */
double metodoSimpson(double a, double b, int n, double delta, double (*f)(double)) {

    double x = 0.0, resultado = 0.0;
    int i;

    for(i = 1; i < n; i++) {
        x = a + delta * i;
        
        if(i % 2 != 0) {
            resultado += 4*f(x);
        } else {
            resultado += 2*f(x);
        }
    }

    resultado += f(a) + f(b);
    resultado *= delta/3;
    
    return resultado;
}
