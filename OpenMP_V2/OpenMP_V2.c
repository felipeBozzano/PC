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
    clock_t start, end;
    double start1, end1;
    double tiempo_total;

    start1 = omp_get_wtime(); 

    /*Paralelismo anidado*/
    omp_set_nested(1);

    const double pi = 3.14159265359;
    const int cantHilos = 4;

    /*Rango de integración */
    double a = 3, b = 30;

    /*Cantidad de intervalos */
    int n = 12800; 

    double delta = (b-a)/n;
    int tid, chunk = n / cantHilos;

    printf("\nOpenMP_V2\n");

    printf("Funcion: x*x\n");
    printf("Rango [%.2f, %.2f] con %d intervalos\n\n", a, b, n);

    start = clock();

    /*Ejecución de los 4 métodos en paralelo, cada uno en un hilo*/
    #pragma omp parallel default(shared) private(tid)
    {
        tid = omp_get_thread_num();
        if (tid == 0)
            printf("Numero de hilos = %i\n", omp_get_num_threads());

        /*Cada hilo ingresa en una section*/
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

    end = clock();
    tiempo_total = (end-start)/(double)CLOCKS_PER_SEC;
    end1 = omp_get_wtime(); 

    printf("Tiempo de uso de CPU : %fs\n", tiempo_total);
    printf("Tiempo de ejecucion  total: %fs\n", end1 - start1);

    return 0;
}

/*Función que calcula la f(x) a integrar valuada en la x pasada como parámetro*/
double funcion(double x) {
    return x*x;
}

/* Regla del rectángulo */
double metodoRectangulo(double a, int n, double delta, int cantHilos, int chunk, double (*f)(double)) {

    double x = 0.0, resultado = 0.0;
    int i, id;

    /*Seteo cantidad de hilos que van a trabajar*/
    omp_set_num_threads(cantHilos);
    
    /*Ejecución*/
    /* Se hace el default para que sean shared todas las variables menos las que estan en private*/
    #pragma omp parallel default(shared) private(i,x,id)
    {
        id = omp_get_thread_num();
        printf("Rectangulo - Hola, soy el hilo hijo: %i\n", id);

    /* Se hace un for en donde se va a dividir el arreglo en 4 partes iguales*/
        #pragma omp for schedule(dynamic,chunk) reduction(+:resultado)
        for(i = 0; i < n; i++) {
            x = delta * i + a; //xi respectivas
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
        printf("PM - Hola, soy el hilo hijo: %i\n", id);
        
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
