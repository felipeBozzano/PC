#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <mpi.h>
#include <unistd.h>

double function(double x);
double metodoRectangulo(double a, int n, double delta);
double metodoPMedio(double a, int n, double delta);
double metodoTrapecio(double a, double b, int n, double delta);
double metodoSimpson(double a, double b, int n, double delta);

int main (int argc, char** argv) {
    clock_t start, end;
    double tiempo_total;

    const double pi = 3.14159265359;

    /*Rango de integración */

    double a = 0, b = 15;

    /*Cantidad de intervalos */
    int n = 9000;

    double delta = (b-a)/n;
    double arreglo[4];
    double resultado = 0.0;
    int rank, cant, source, dest = 0, tag = 0;
    MPI_Status status;
    
    /*Inicialización del entorno MPI*/
    MPI_Init(&argc,&argv);
    double time1 = MPI_Wtime();

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &cant);

    /* Solo permite que sean 5 procesos (el root mas los MN) que es lo que pide el enunciado*/
    if(cant != 5) {
        if (rank == 0) {
            printf("La cantidad de procesos debe ser igual a 5\n");
            MPI_Finalize();
            exit(0);
        }
    }
    
    switch (rank)
    {
    case 0:
        start = clock();
    
        printf("-------------------------------------------------\n");
        printf("\nMetodos de integracion numerica - Version 1 MPI\n\n");
        printf("Funcion: 2x^2 + 3x - 1\n");
        printf("Rango de integracion: [%.2f, %.2f]\n", a, b);
        printf("Cantidad de intervalos: %d\n", n);
        printf("-------------------------------------------------\n\n");

        for(int i = 1; i < cant; i++) {
            MPI_Recv(&resultado, 1, MPI_DOUBLE, MPI_ANY_SOURCE, tag, MPI_COMM_WORLD, &status);
            arreglo[status.MPI_SOURCE-1] = resultado;
            switch (status.MPI_SOURCE)
            {
            case 1:
                printf("-Soy el proceso %i y calcule el Metodo de rectangulo: %f\n", status.MPI_SOURCE, arreglo[0]);
                break;

            case 2:
                printf("-Soy el proceso %i y calcule el Metodo punto medio: %f\n", status.MPI_SOURCE, arreglo[1]);
                break;

            case 3:
                printf("-Soy el proceso %i y calcule el Metodo de trapecio: %f\n", status.MPI_SOURCE, arreglo[2]);
                break;

            case 4:
                printf("-Soy el proceso %i y calcule el Metodo de Simpson: %f\n\n", status.MPI_SOURCE, arreglo[3]);
                break;
            
            default:
                break;
            }
        }

        end = clock();

        tiempo_total = (end-start)/(double)CLOCKS_PER_SEC; //solo se mide tiempo de cpu

        double elapsed = MPI_Wtime() - time1;

        printf("Tiempo de uso de CPU : %fs\n", tiempo_total);
        printf("Tiempo de ejecucion  total: %fs\n", elapsed);
        break;

    case 1: 
        printf("Hola, soy el proceso: %i\n", rank);
        resultado = metodoRectangulo(a,n,delta);
        MPI_Send(&resultado, 1, MPI_DOUBLE, dest, tag, MPI_COMM_WORLD);
        break;
    
    case 2: 
        printf("Hola, soy el proceso: %i\n", rank);
        resultado = metodoPMedio(a,n,delta);
        MPI_Send(&resultado, 1, MPI_DOUBLE, dest, tag, MPI_COMM_WORLD);
        break;

    case 3:
        printf("Hola, soy el proceso: %i\n", rank);
        resultado = metodoTrapecio(a,b,n,delta);
        MPI_Send(&resultado, 1, MPI_DOUBLE, dest, tag, MPI_COMM_WORLD);
        break;

    case 4:
        printf("Hola, soy el proceso: %i\n", rank);
        resultado = metodoSimpson(a,b,n,delta);
        MPI_Send(&resultado, 1, MPI_DOUBLE, dest, tag, MPI_COMM_WORLD);
        break;
        
    default:
        break;
    }
    MPI_Finalize();
}

/*Función que calcula la f(x) a integrar valuada en la x pasada como parámetro*/
double function(double x) {
    /* return sin(x); */
    return 2*x*x + 3*x -1;
}

/* Regla del rectángulo */
double metodoRectangulo(double a, int n, double delta) {
    double x = 0.0, resultado = 0.0;
    int i;

    for(i = 0; i < n; i++) {
        x = delta * i + a;
        resultado += function(x);
    }

    resultado *= delta;

    return resultado;
}

/* Regla del Punto Medio */
double metodoPMedio(double a, int n, double delta) {
    double x = 0.0, resultado = 0.0;
    int i;

    for(i = 1; i <= n; i++) {
        x = (delta*(i-1) + delta*i + 2*a) / 2;
        resultado += function(x);
    }

    resultado *= delta;
     
    return resultado;
}

/* Regla del Trapecio */
double metodoTrapecio(double a, double b, int n, double delta) {
    double x = 0.0, resultado = 0.0;
    int i;
    
    for(i = 1; i <= n-1; i++) {
        x = delta * i + a;
        resultado += 2*function(x);
    }

    resultado += function(a) + function(b);
    resultado *= delta/2; 

    return resultado;
}

/* Regla Simpson 1/3 */
double metodoSimpson(double a, double b, int n, double delta) {
    double x = 0.0, resultado = 0.0;
    int i;

    for(i = 1; i < n; i++) {
        x = a + delta * i;
        
        if(i % 2 != 0) {
            resultado += 4*function(x);
        } else {
            resultado += 2*function(x);
        }
    }

    resultado += function(a) + function(b);
    resultado *= delta/3;
    
    return resultado;
}