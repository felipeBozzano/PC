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

    /* VARIABLES DE MANEJO DE TIEMPO */
    clock_t inicio, fin;
    double inicio_omp , fin_omp;
    double tiempo_total;

    inicio_omp = omp_get_wtime();

    /* DATOS DE INTEGRACION */
    double a = 0, b = 15;       // Intervalo de integracion
    int n = 90000000;           // Cantidad de sub-intervalos
    double delta = (b-a)/n;     // Paso de integracion

    /* DATOS DE HILOS */
    int tid;                    // Id de cada hilo

    printf("\nOpenMP_V1\n");
    printf("\nFuncion: 2*x^2 + 3*x -1\n");
    printf("Intervalo [%.2f, %.2f]\n", a, b);
    printf("Cantidad de sub-intervalos: %d\n", n);
    printf("Delta de integracion: %f\n\n", delta);

    inicio = clock();

    /* Comienza el paralelismo con OpenMP, donde cada hilo tiene su propio id */
    #pragma omp parallel default(shared) private(tid)
    {
        tid = omp_get_thread_num(); // Cada hilo obtiene su id
        if (tid == 0)
            printf("Numero de hilos = %d\n", omp_get_num_threads());

        /* Utilizamos secciones para cada método de integración. Cada una de ellas NO 
           espera la finalizacion del resto */
        #pragma omp sections nowait
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

    fin = clock();
    tiempo_total = (fin-inicio)/(double)CLOCKS_PER_SEC;
    fin_omp = omp_get_wtime();

    printf("\nTiempo de uso de CPU: %fs\n", tiempo_total);
    printf("Tiempo de ejecucion total: %fs\n", fin_omp - inicio_omp);
    
    return 0;
}

double funcion(double x) {
    return 2*x*x + 3*x -1;
}
