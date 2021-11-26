#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>

#include "MatSimple.h"

double funcion(double x);

int main() {

    /* VARIABLES DE MANEJO DE TIEMPO */
    struct timeval start, end;

    /* DATOS DE INTEGRACION */
    double a = 0, b = 15;       // Intervalo de integracion
    int n = 90000000;           // Cantidad de sub-intervalos
    double delta = (b-a)/n;     // Paso de integracion

    /* Comienza a tomar el tiempo total */
    gettimeofday(&start, 0);

    printf("-------------------------------------------------\n");
    printf("\nMetodos de integracion numerica - Version Secuencial\n\n");
    printf("Funcion: 2x^2 + 3x - 1\n");
    printf("Rango de integracion: [%.2f, %.2f]\n", a, b);
    printf("Cantidad de intervalos: %d\n", n);
    printf("-------------------------------------------------\n\n");

    printf("-Metodo de rectangulo: %f\n", metodoRectangulo(a, n, delta, funcion));
    printf("-Metodo punto medio: %f\n", metodoPMedio(a, n, delta, funcion));
    printf("-Metodo de trapecio: %f\n", metodoTrapecio(a, b, n, delta, funcion));
    printf("-Metodo de Simpson: %f\n\n", metodoSimpson(a, b, n, delta, funcion));

    gettimeofday(&end, 0);
    long seconds = end.tv_sec - start.tv_sec;
    long microseconds = end.tv_usec - start.tv_usec;
    double elapsed = seconds + microseconds*1e-6;

    printf("\nTiempo de ejecucion total: %fs\n", elapsed);
    return 0;
}

double funcion(double x) {
    return 2*x*x + 3*x -1;
}
