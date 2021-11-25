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

    double a = 3, b = 30;

    /*Cantidad de intervalos */
    int n = 12800;

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
    
    /*Proceso root*/
    if(rank == 0) {
        start = clock();

        printf("\nVersion 1 MPI\n");
    
        printf("\nMetodos de integracion numerica\n");
      
        printf("Funcion: x*x");
        printf("\nRango [%.2f, %.2f] con %d intervalos\n\n", a, b, n);

        for(int i = 1; i < cant; i++) {
            MPI_Recv(&resultado, 1, MPI_DOUBLE, MPI_ANY_SOURCE, tag, MPI_COMM_WORLD, &status);
            arreglo[status.MPI_SOURCE-1] = resultado;
        }
        
        printf("-Metodo de rectangulo: %f\n", arreglo[0]);
        printf("-Metodo punto medio: %f\n", arreglo[1]);
        printf("-Metodo de trapecio: %f\n", arreglo[2]);
        printf("-Metodo de Simpson: %f\n\n", arreglo[3]);

        end = clock();

        tiempo_total = (end-start)/(double)CLOCKS_PER_SEC; //solo se mide tiempo de cpu

        double elapsed = MPI_Wtime() - time1;

        printf("Tiempo de uso de CPU : %fs\n", tiempo_total);
        printf("Tiempo de ejecucion  total: %fs\n", elapsed);
    }

    /*Cada proceso realiza un método de integración distinto y luego envía el resultado al proceso root*/
    if(rank == 1) {
        printf("Hola, soy el proceso: %i\n", rank);
        resultado = metodoRectangulo(a,n,delta);
        MPI_Send(&resultado, 1, MPI_DOUBLE, dest, tag, MPI_COMM_WORLD);
    }

    if(rank == 2) {
        printf("Hola, soy el proceso: %i\n", rank);
        resultado = metodoPMedio(a,n,delta);
        MPI_Send(&resultado, 1, MPI_DOUBLE, dest, tag, MPI_COMM_WORLD);
    }

    if(rank == 3) {
        printf("Hola, soy el proceso: %i\n", rank);
        resultado = metodoTrapecio(a,b,n,delta);
        MPI_Send(&resultado, 1, MPI_DOUBLE, dest, tag, MPI_COMM_WORLD);
    }

    if(rank == 4) {
        printf("Hola, soy el proceso: %i\n", rank);
        resultado = metodoSimpson(a,b,n,delta);
        MPI_Send(&resultado, 1, MPI_DOUBLE, dest, tag, MPI_COMM_WORLD);
    }
    MPI_Finalize();
}

/*Función que calcula la f(x) a integrar valuada en la x pasada como parámetro*/
double function(double x) {
    /* return sin(x); */
    return x*x;
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