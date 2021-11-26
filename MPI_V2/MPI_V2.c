#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <mpi.h>
#include <unistd.h>

double funcion(double x);
double metodoRectangulo(double a, int n, double delta);
double metodoPMedio(double a, int n, double delta);
double metodoTrapecio(double a, double b, int n, double delta);
double metodoSimpson(double a, double b, int n, double delta);

int main (int argc, char** argv) {

    /* VARIABLES DE MANEJO DE TIEMPO */
    clock_t start, end;
    double tiempo_total;

    /* DATOS DE INTEGRACION */
    double a = 0, b = 15;                   // Intervalo de integracion
    int n = 90000000;                       // Cantidad de sub-intervalos
    double delta = (b-a)/n;                 // Paso de integracion
    double a_por_proceso, b_por_proceso;    // Intervalo de integracion por proceso
    int n_por_cluster;                      // Cantidad de sub-intervalos por cluster

    /* VARIABLES DE PROCESOS */
    int rank;                               // Identificador
    int cant_total;                         // Cantidad de procesos

    /* VARIABLES DE PROCESOS AGRUPADOS */
    int color;                              // Identificador de cluster
    int clustered_process_rank;             // Identificador de proceso dentro del cluster
    int clustered_cant_total;               // Cantidad de procesos dentro del cluster
    double resultado = 0.0, resultado1 = 0.0, resultado2 = 0.0, resultado3 = 0.0, resultado4 = 0.0;

    MPI_Status status;
    
    /*Inicialización del entorno de ejecución MPI*/
    MPI_Init(&argc,&argv);
    double time1 = MPI_Wtime();

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &cant_total);

    /*Si la cantidad de procesos es menor que 16 el programa se cierra*/
    if(cant_total < 16) {
        if (rank == 0) {
            printf("La cantidad de procesos debe ser por lo menos a 16\n");
            MPI_Finalize();
            exit(0);
        }
    } else {
        color = rank/4;

        /* Se divide el comunicador en 4 nuevos comunicadores (clusters) */
        MPI_Comm cluster_comm;
        MPI_Comm_split(MPI_COMM_WORLD, color, rank, &cluster_comm);

        MPI_Comm_rank(cluster_comm, &clustered_process_rank);
        MPI_Comm_size(cluster_comm, &clustered_cant_total);

        /* Variables locales para cada proceso según su identificador en cada cluster */
        n_por_cluster = n/clustered_cant_total;
        a_por_proceso = a + clustered_process_rank*n_por_cluster*delta;
        b_por_proceso = a_por_proceso + n_por_cluster*delta;

        /* El proceso root en el comunicador WORLD imprime los mensajes */
        if(rank == 0) {
            printf("-------------------------------------------------\n");
            printf("\nMetodos de integracion numerica - Version 2 MPI\n\n");
            printf("Funcion: 2x^2 + 3x - 1\n");
            printf("Rango de integracion: [%.2f, %.2f]\n", a, b);
            printf("Cantidad de intervalos: %d\n", n);
            printf("-------------------------------------------------\n\n");

            start = clock();
        }

        /* Primer Cluster - Color 0 */
        if(color == 0) {

            /* Cada proceso ejecuta su porcion del metodo */
            resultado = metodoRectangulo(a_por_proceso, n_por_cluster, delta);

            /* Se suman los resultados de cada proceso */
            MPI_Reduce(&resultado, &resultado1, 1, MPI_DOUBLE, MPI_SUM, 0, cluster_comm);

            /* Elejimos el proceso con mayor peso para mostrar el resultado */
            if(clustered_process_rank == 0)
                printf("-) Metodo de rectangulo %f\n", resultado1);
        }

        /* Segundo Cluster - Color 1 */
        if(color == 1) {

            resultado = metodoPMedio(a_por_proceso,n_por_cluster,delta);

            MPI_Reduce(&resultado, &resultado2, 1, MPI_DOUBLE, MPI_SUM, 0, cluster_comm);

            if(clustered_process_rank == 0)
                printf("-) Metodo punto medio: %f\n", resultado2);
        }

        /* Tercer Cluster - Color 2 */
        if(color == 2) {

            resultado = metodoTrapecio(a_por_proceso,b_por_proceso,n_por_cluster,delta);

            MPI_Reduce(&resultado, &resultado3, 1, MPI_DOUBLE, MPI_SUM, 0, cluster_comm);

            if(clustered_process_rank == 0)
                printf("-) Metodo de trapecio: %f\n", resultado3);
        }

        /* Cuarto Comunicador - Color 3 */
        if(color == 3) {

            resultado = metodoSimpson(a_por_proceso,b_por_proceso,n_por_cluster,delta);

            MPI_Reduce(&resultado, &resultado4, 1, MPI_DOUBLE, MPI_SUM, 0, cluster_comm);

            if(clustered_process_rank == 0)
                printf("-) Metodo de Simpson : %f\n", resultado4);
        }

        /* Se pone un stop a los procesos hasta que todos terminan para continuar la ejecución */
        MPI_Barrier(MPI_COMM_WORLD);

        if(rank == 0) {
            end = clock();

            tiempo_total = (end-start)/(double)CLOCKS_PER_SEC;

            double elapsed = MPI_Wtime() - time1;

            printf("\nTiempo de uso de CPU: %fs\n", tiempo_total);
            printf("Tiempo de ejecucion total: %fs\n", elapsed);
        }

        MPI_Comm_free(&cluster_comm);
        MPI_Finalize();
    }
}

double funcion(double x) {
    return 2*x*x + 3*x -1;
}

/* Regla del rectángulo */
double metodoRectangulo(double a, int n, double delta) {
    double x = 0.0, resultado = 0.0;
    int i;

    for(i = 0; i < n; i++) {
        x = delta * i + a;
        resultado += funcion(x);
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
        resultado += funcion(x);
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
        resultado += 2*funcion(x);
    }

    resultado += funcion(a) + funcion(b);
    resultado *= delta/2; 

    return resultado;
}

/* Simpson 1/3 */
double metodoSimpson(double a, double b, int n, double delta) {
    double x = 0.0, resultado = 0.0;
    int i;

    for(i = 1; i < n; i++) {
        x = a + delta * i;
        
        if(i % 2 != 0) {
            resultado += 4*funcion(x);
        } else {
            resultado += 2*funcion(x);
        }
    }

    resultado += funcion(a) + funcion(b);
    resultado *= delta/3;
    
    return resultado;
}
