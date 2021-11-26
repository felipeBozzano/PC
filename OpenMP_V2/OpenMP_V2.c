#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <omp.h>
#include <unistd.h>

double funcion(double x);
double metodoRectangulo(double a, int n, double delta, int cantHilos, int chunk, double (*f)(double));
double metodoPMedio(double a, int n, double delta, int cantHilos, int chunk, double (*f)(double));
double metodoTrapecio(double a, double b, int n, double delta, int cantHilos, int chunk, double (*f)(double));
double metodoSimpson(double a, double b, int n, double delta, int cantHilos, int chunk, double (*f)(double));

int main() {

    /* VARIABLES DE MANEJO DE TIEMPO */
    clock_t inicio, fin;
    double inicio_omp, fin_omp;
    double tiempo_total;

    inicio_omp = omp_get_wtime(); 

    /* CONSTANTES */
    const int cantHilos = 4;

    /* DATOS DE INTEGRACION */
    double a = 0, b = 15;       // Intervalo de integracion
    int n = 90000000;           // Cantidad de sub-intervalos
    double delta = (b-a)/n;     // Paso de integracion

    /* DATOS DE HILOS */
    omp_set_nested(1);          // Habilitamos el paralelismo anidado
    int tid;                    // Id de cada hilo
    int chunk = n / cantHilos;  // Porciones de iteracion

    printf("\n-------------------------------------------------\n");
    printf("\nMetodos de integracion numerica - Version 2 OMP\n\n");
    printf("Funcion: 2x^2 + 3x - 1\n");
    printf("Rango de integracion: [%.2f, %.2f]\n", a, b);
    printf("Cantidad de intervalos: %d\n", n);
    printf("Delta de integracion: %f\n\n", delta);
    printf("-------------------------------------------------\n\n");

    inicio = clock();

    /* Comienza el paralelismo con OpenMP, donde cada hilo tiene su propio id */
    #pragma omp parallel default(shared) private(tid)
    {
        tid = omp_get_thread_num(); // Cada hilo obtiene su id
        if (tid == 0)
            printf("Numero de hilos = %i\n", omp_get_num_threads());

        /* Utilizamos secciones para cada método de integración. Cada una de ellas NO 
           espera la finalizacion del resto */
        #pragma omp sections nowait
        {
            #pragma omp section
            {
                printf("Hola, soy el hilo padre: %i - Metodo de Rectangulo\n", tid);
                printf("Hilo %i - Metodo de Rectangulo: %f\n", tid, metodoRectangulo(a, n, delta, cantHilos, chunk, funcion));
            }
            
            #pragma omp section
            {
                printf("Hola, soy el hilo padre: %i - Metodo de PMedio\n", tid);
                printf("Hilo %i - Metodo de PMedio: %f\n", tid, metodoPMedio(a, n, delta, cantHilos, chunk, funcion));
            }
                
            #pragma omp section
            {
                printf("Hola, soy el hilo padre: %i - Metodo de Trapecio\n", tid);
                printf("Hilo %i - Metodo de Trapecio: %f\n", tid, metodoTrapecio(a, b, n, delta, cantHilos, chunk, funcion));
            }

            #pragma omp section
            {
                printf("Hola, soy el hilo padre: %i - Metodo de Simpson\n", tid);
                printf("Hilo %i - Metodo de Simpson: %f\n", tid, metodoSimpson(a, b, n, delta, cantHilos, chunk, funcion));
            }
        }
    }

    fin = clock();
    tiempo_total = (fin-inicio)/(double)CLOCKS_PER_SEC;
    fin_omp = omp_get_wtime(); 

    printf("Tiempo de uso de CPU : %fs\n", tiempo_total);
    printf("Tiempo de ejecucion  total: %fs\n", fin_omp - inicio_omp);

    return 0;
}

double funcion(double x) {
    return 2*x*x + 3*x -1;
}

/* Regla del Rectangulo */
double metodoRectangulo(double a, int n, double delta, int cantHilos, int chunk, double (*f)(double)) {

    double x = 0.0, resultado = 0.0;
    int i, id;

    /* Seteamos la cantidad de hilos que se dispararan */
    omp_set_num_threads(cantHilos);
    
    /* Comienza el paralelismo. Las variables i, x, id son privadas de cada hilo mientras 
       que el resto son compartidas */
    #pragma omp parallel default(shared) private(i, x, id)
    {
        id = omp_get_thread_num(); // Cada hilo obtiene su id
        printf("Rectangulo - Hola, soy el hilo hijo: %i\n", id);

        /* Se dispara el for paralelo en donde se setea que se balancee la carga de los hilos,
           y los intervalos que toma cada uno. Además el resultado de cada hilo se suma a una
           variable "resultado" para unir el trabajo de cada uno */
        #pragma omp for schedule(dynamic,chunk) reduction(+:resultado)
        for(i = 0; i < n; i++) {
            x = delta * i + a;
            resultado += f(x);
        }
    }

    resultado *= delta;

    return resultado;
}

/* Regla del Punto Medio */
double metodoPMedio(double a, int n, double delta, int cantHilos, int chunk, double (*f)(double)) {

    double x = 0.0, resultado = 0.0;
    int i, id;

    omp_set_num_threads(cantHilos);
    
    #pragma omp parallel default(shared) private(i,x,id)
    {
        id = omp_get_thread_num();
        printf("PMedio - Hola, soy el hilo hijo: %i\n", id);
        
        #pragma omp for schedule(dynamic,chunk) reduction(+:resultado)
        for(i = 1; i <= n; i++) {
            x = (delta*(i-1) + delta*i + 2*a) / 2;
            resultado += f(x);
        }
    }

    resultado *= delta;
     
    return resultado;
}

/* Regla del Trapecio */
double metodoTrapecio(double a, double b, int n, double delta, int cantHilos, int chunk, double (*f)(double)) {

    double x = 0.0, resultado = 0.0;
    int i, id;

    omp_set_num_threads(cantHilos);
    
    #pragma omp parallel default(shared) private(i,x,id)
    {
        id = omp_get_thread_num();
        printf("Trapecio - Hola, soy el hilo hijo: %i\n", id);

        #pragma omp for schedule(dynamic,chunk) reduction(+:resultado)
        for(i = 1; i <= n-1; i++) {
            x = delta * i + a;
            resultado += 2*f(x);
        }
    }

    resultado += f(a) + f(b);
    resultado *= delta/2;

    return resultado;
}

/* Simpson 1/3 */
double metodoSimpson(double a, double b, int n, double delta, int cantHilos, int chunk, double (*f)(double)) {

    double x = 0.0, resultado = 0.0;
    int i, id;

    omp_set_num_threads(cantHilos);
    
    #pragma omp parallel default(shared) private(i,x,id)
    {
        id = omp_get_thread_num();
        printf("Simpson - Hola, soy el hilo hijo: %i\n", id);

        #pragma omp for schedule(dynamic,chunk) reduction(+:resultado)
        for(i = 1; i < n; i++) {
            x = a + delta * i;
        
            if(i % 2 != 0) {
                resultado += 4*f(x);
            } else {
                resultado += 2*f(x);
            }
        }
    }

    resultado += f(a) + f(b);
    resultado *= delta/3;
    
    return resultado;
}
