#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <omp.h>
#include <sys/time.h>
#include <unistd.h>

#include "MatSimple.h"

double funcion(double x);

int main() {
    clock_t start, end;
    double start1 , end1;
    double tiempo_total;

    start1 = omp_get_wtime();

    const double pi = 3.14159265359;

    /*Rango de integración */
    double a = 3, b = 30;

    /*Cantidad de intervalos */
    int n = 12800;

    double delta = (b-a)/n;
    int tid;

    printf("\nOpenMP_V1\n");

    printf("\nFuncion: x*x\n");
    printf("Rango [%.2f, %.2f] con %d intervalos\n\n", a, b, n);

    start = clock();

    /*Ejecución de los métodos en paralelo, cada uno en un hilo*/
    #pragma omp parallel default(shared) private(tid)
    {
        tid = omp_get_thread_num();
        if (tid == 0)
            printf("Numero de hilos = %d\n", omp_get_num_threads());

        #pragma omp sections nowait //Cada seccion se ejecuta por un thread
        {
            #pragma omp section
            {
                printf("Hola, soy el hilo: %i - Metodo de Rectangulo\n", tid);
                printf("Hilo %i - Metodo de Rectangulo: %f\n", tid, metodoRectangulo(a, n, delta, funcion));
            }
                
            #pragma omp section
            {
                printf("Hola, soy el hilo: %i - Metodo de PMedio\n", tid);
                printf("Hilo %i - Metodo de PMedio: %f\n", tid, metodoPMedio(a, n, delta, funcion));
            }
                
            #pragma omp section
            {
                printf("Hola, soy el hilo: %i - Metodo de Trapecio\n", tid);
                printf("Hilo %i - Metodo de Trapecio: %f\n", tid, metodoTrapecio(a, b, n, delta, funcion));
            }

            #pragma omp section
            {
                printf("Hola, soy el hilo: %i - Metodo de Simpson\n", tid);
                printf("Hilo %i - Metodo de Simpson: %f\n", tid, metodoSimpson(a, b, n, delta, funcion));
            }
        }
    }

    end = clock();
    tiempo_total = (end-start)/(double)CLOCKS_PER_SEC;
    end1 = omp_get_wtime();

    printf("\nTiempo de uso de CPU : %fs\n", tiempo_total);
    printf("Tiempo de ejecucion total: %fs\n", end1 - start1);
    
    return 0;
}

/*Función que calcula la f(x) a integrar valuada en la x pasada como parámetro*/
double funcion(double x) {
    return x*x;
}
