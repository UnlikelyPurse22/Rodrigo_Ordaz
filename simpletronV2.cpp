#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define TAM_MEMORIA 1000
#define MINIMO -99999
#define MAXIMO 99999

void cargarPrograma(int memory[]);
void ejecutarPrograma(int memory[], int *accumulator, int *instructionCounter,
                      int *instructionRegister, int *operationCode, int *operand);
void vaciadoMemoria(int memory[], int accumulator, int instructionCounter,
                    int instructionRegister, int operationCode, int operand);

int main()
{
    int memory[TAM_MEMORIA];

    int accumulator = 0;
    int instructionCounter = 0;
    int instructionRegister = 0;
    int operationCode = 0;
    int operand = 0;

    int i;

    for (i = 0; i < TAM_MEMORIA; i++)
    {
        memory[i] = 0;
    }

    printf("*** Bienvenido a Simpletron (Edicion Extendida 1000 palabras) ***\n");

    cargarPrograma(memory);

    printf("\n*** Se termino de cargar el programa ***\n");
    printf("*** Comienza la ejecucion de Simpletron ***\n\n");

    ejecutarPrograma(memory, &accumulator, &instructionCounter,
                     &instructionRegister, &operationCode, &operand);

    vaciadoMemoria(memory, accumulator, instructionCounter,
                    instructionRegister, operationCode, operand);

    return 0;
}

/* Funcion para cargar el programa en memoria (desde archivo o teclado) */
void cargarPrograma(int memory[])
{
    FILE *archivo = fopen("programa.simp", "r");
    int contador = 0;
    int valor;

    if (archivo != NULL)
    {
        printf("*** Leyendo instruccion desde programa.simp ***\n");
        while (fscanf(archivo, "%d", &valor) == 1 && contador < TAM_MEMORIA)
        {
            if (valor == 99999 || valor == -99999)
            {
                break;
            }

            if (valor >= MINIMO && valor <= MAXIMO)
            {
                memory[contador++] = valor;
            }
            else
            {
                printf("Error en programa.simp linea %d: Valor fuera de rango (%d).\n", contador, valor);
            }
        }
        fclose(archivo);
        return;
    }

    /* Modo interactivo por teclado si no existe programa.simp */
    printf("*** No se encontro programa.simp. Modo Interactivo activo. ***\n");
    printf("*** Por favor introduzca su programa una instruccion a la vez ***\n");
    printf("*** Cada instruccion debe ser un numero de 5 digitos ***\n");
    printf("*** Para terminar de cargar introduzca 99999 ***\n\n");

    while (contador < TAM_MEMORIA)
    {
        printf("%03d ? ", contador);
        if (scanf("%d", &valor) != 1)
        {
            printf("Entrada invalida. Intente de nuevo.\n");
            while (getchar() != '\n'); // Limpiar buffer
            continue;
        }

        if (valor == 99999)
        {
            break;
        }

        if (valor >= MINIMO && valor <= MAXIMO)
        {
            memory[contador] = valor;
            contador++;
        }
        else
        {
            printf("Valor invalido. Debe estar entre -99999 y 99999.\n");
        }
    }

    if (contador == TAM_MEMORIA)
    {
        printf("\nLa memoria esta llena.\n");
    }
}

/* Funcion principal de ejecucion */
void ejecutarPrograma(int memory[], int *accumulator, int *instructionCounter,
                      int *instructionRegister, int *operationCode, int *operand)
{
    int ejecutando = 1;
    long long resultado; // Prevenir overflow directo en C
    char buffer_cadena[500];

    while (ejecutando)
    {
        /* Buscar la siguiente instruccion */
        if (*instructionCounter < 0 || *instructionCounter >= TAM_MEMORIA)
        {
            printf("\n*** ERROR FATAL ***\n");
            printf("El contador de instrucciones esta fuera de memoria.\n");
            return;
        }

        *instructionRegister = memory[*instructionCounter];

        /* Separar codigo de operacion (2 digitos) y operando (3 digitos) */
        *operationCode = abs(*instructionRegister) / 1000;
        *operand = abs(*instructionRegister) % 1000;

        (*instructionCounter)++;

        switch (*operationCode)
        {
            /* 10 - Leer Entero / Flotante escalado */
            case 10:
            {
                printf("Ingrese un numero: ");
                if (scanf("%lld", &resultado) != 1) {
                    printf("\n*** ERROR FATAL ***\nEntrada no valida.\n");
                    return;
                }

                if (resultado < MINIMO || resultado > MAXIMO)
                {
                    printf("\n*** ERROR FATAL ***\nValor fuera de rango.\n");
                    return;
                }

                if (*operand < 0 || *operand >= TAM_MEMORIA)
                {
                    printf("\n*** ERROR FATAL ***\nOperando fuera de memoria.\n");
                    return;
                }

                memory[*operand] = (int)resultado;
                break;
            }

            /* 11 - Escribir */
            case 11:
            {
                if (*operand < 0 || *operand >= TAM_MEMORIA)
                {
                    printf("\n*** ERROR FATAL ***\nOperando fuera de memoria.\n");
                    return;
                }

                printf("Salida: %d\n", memory[*operand]);
                break;
            }

            /* 12 - Salida de Nueva Linea */
            case 12:
            {
                printf("\n");
                break;
            }

            /* 13 - Entrada de Cadena */
            case 13:
            {
                if (*operand < 0 || *operand >= TAM_MEMORIA)
                {
                    printf("\n*** ERROR FATAL ***\nOperando fuera de memoria.\n");
                    return;
                }

                printf("Ingrese una cadena: ");
                while (getchar() != '\n'); // Limpiar salto previo
                if (fgets(buffer_cadena, sizeof(buffer_cadena), stdin) != NULL)
                {
                    size_t len = strlen(buffer_cadena);
                    if (len > 0 && buffer_cadena[len - 1] == '\n') {
                        buffer_cadena[--len] = '\0';
                    }

                    if (*operand + (int)len >= TAM_MEMORIA) {
                        printf("\n*** ERROR FATAL ***\nCadena excede el limite de memoria.\n");
                        return;
                    }

                    int primer_ascii = (len > 0) ? (unsigned char)buffer_cadena[0] : 0;
                    memory[*operand] = ((int)len * 1000) + primer_ascii;

                    for (size_t k = 1; k < len; k++) {
                        memory[*operand + (int)k] = (unsigned char)buffer_cadena[k];
                    }
                }
                break;
            }

            /* 14 - Salida de Cadena */
            case 14:
            {
                if (*operand < 0 || *operand >= TAM_MEMORIA)
                {
                    printf("\n*** ERROR FATAL ***\nOperando fuera de memoria.\n");
                    return;
                }

                int longitud = memory[*operand] / 1000;
                int char_inicial = memory[*operand] % 1000;

                if (longitud < 0 || *operand + longitud > TAM_MEMORIA) {
                    printf("\n*** ERROR FATAL ***\nEstructura de cadena invalida o fuera de memoria.\n");
                    return;
                }

                printf("Salida Cadena: ");
                if (longitud > 0) {
                    printf("%c", (char)char_inicial);
                    for (int k = 1; k < longitud; k++) {
                        int c = memory[*operand + k] % 1000;
                        printf("%c", (char)c);
                    }
                }
                printf("\n");
                break;
            }

            /* 20 - Cargar */
            case 20:
            {
                if (*operand < 0 || *operand >= TAM_MEMORIA)
                {
                    printf("\n*** ERROR FATAL ***\nOperando fuera de memoria.\n");
                    return;
                }

                *accumulator = memory[*operand];
                break;
            }

            /* 21 - Almacenar */
            case 21:
            {
                if (*operand < 0 || *operand >= TAM_MEMORIA)
                {
                    printf("\n*** ERROR FATAL ***\nOperando fuera de memoria.\n");
                    return;
                }

                memory[*operand] = *accumulator;
                break;
            }

            /* 30 - Sumar */
            case 30:
            {
                if (*operand < 0 || *operand >= TAM_MEMORIA)
                {
                    printf("\n*** ERROR FATAL ***\nOperando fuera de memoria.\n");
                    return;
                }

                resultado = (long long)*accumulator + memory[*operand];

                if (resultado < MINIMO || resultado > MAXIMO)
                {
                    printf("\n*** ERROR FATAL ***\nDesbordamiento del acumulador.\n");
                    return;
                }

                *accumulator = (int)resultado;
                break;
            }

            /* 31 - Restar */
            case 31:
            {
                if (*operand < 0 || *operand >= TAM_MEMORIA)
                {
                    printf("\n*** ERROR FATAL ***\nOperando fuera de memoria.\n");
                    return;
                }

                resultado = (long long)*accumulator - memory[*operand];

                if (resultado < MINIMO || resultado > MAXIMO)
                {
                    printf("\n*** ERROR FATAL ***\nDesbordamiento del acumulador.\n");
                    return;
                }

                *accumulator = (int)resultado;
                break;
            }

            /* 32 - Dividir */
            case 32:
            {
                if (*operand < 0 || *operand >= TAM_MEMORIA)
                {
                    printf("\n*** ERROR FATAL ***\nOperando fuera de memoria.\n");
                    return;
                }

                if (memory[*operand] == 0)
                {
                    printf("\n*** ERROR FATAL ***\nIntento de dividir entre cero.\n");
                    return;
                }

                *accumulator = *accumulator / memory[*operand];
                break;
            }

            /* 33 - Multiplicar */
            case 33:
            {
                if (*operand < 0 || *operand >= TAM_MEMORIA)
                {
                    printf("\n*** ERROR FATAL ***\nOperando fuera de memoria.\n");
                    return;
                }

                resultado = (long long)*accumulator * memory[*operand];

                if (resultado < MINIMO || resultado > MAXIMO)
                {
                    printf("\n*** ERROR FATAL ***\nDesbordamiento del acumulador.\n");
                    return;
                }

                *accumulator = (int)resultado;
                break;
            }

            /* 34 - Modulo / Residuo */
            case 34:
            {
                if (*operand < 0 || *operand >= TAM_MEMORIA)
                {
                    printf("\n*** ERROR FATAL ***\nOperando fuera de memoria.\n");
                    return;
                }

                if (memory[*operand] == 0)
                {
                    printf("\n*** ERROR FATAL ***\nModulo entre cero indefinido.\n");
                    return;
                }

                *accumulator = *accumulator % memory[*operand];
                break;
            }

            /* 35 - Exponenciacion (A^B) */
            case 35:
            {
                if (*operand < 0 || *operand >= TAM_MEMORIA)
                {
                    printf("\n*** ERROR FATAL ***\nOperando fuera de memoria.\n");
                    return;
                }

                int exp = memory[*operand];
                if (exp < 0) {
                    printf("\n*** ERROR FATAL ***\nExponente negativo no soportado.\n");
                    return;
                }

                double res = pow((double)*accumulator, (double)exp);

                if (res < MINIMO || res > MAXIMO) {
                    printf("\n*** ERROR FATAL ***\nDesbordamiento en exponenciacion.\n");
                    return;
                }

                *accumulator = (int)res;
                break;
            }

            /* 40 - Bifurcar */
            case 40:
            {
                if (*operand < 0 || *operand >= TAM_MEMORIA)
                {
                    printf("\n*** ERROR FATAL ***\nBifurcacion fuera de memoria.\n");
                    return;
                }

                *instructionCounter = *operand;
                break;
            }

            /* 41 - Bifurcar si es negativo */
            case 41:
            {
                if (*operand < 0 || *operand >= TAM_MEMORIA)
                {
                    printf("\n*** ERROR FATAL ***\nBifurcacion fuera de memoria.\n");
                    return;
                }

                if (*accumulator < 0)
                {
                    *instructionCounter = *operand;
                }
                break;
            }

            /* 42 - Bifurcar si es cero */
            case 42:
            {
                if (*operand < 0 || *operand >= TAM_MEMORIA)
                {
                    printf("\n*** ERROR FATAL ***\nBifurcacion fuera de memoria.\n");
                    return;
                }

                if (*accumulator == 0)
                {
                    *instructionCounter = *operand;
                }
                break;
            }

            /* 43 - Alto */
            case 43:
            {
                ejecutando = 0;
                break;
            }

            default:
            {
                printf("\n*** ERROR FATAL ***\nCodigo de operacion invalido: %d\n", *operationCode);
                return;
            }
        }
    }

    printf("\n*** Termino la ejecucion de Simpletron ***\n");
}

/* Funcion para mostrar los registros y toda la memoria */
void vaciadoMemoria(int memory[], int accumulator, int instructionCounter,
                    int instructionRegister, int operationCode, int operand)
{
    int i;

    printf("\n============================================\n");
    printf("            VACIADO DE SIMPLETRON\n");
    printf("============================================\n");

    printf("REGISTROS:\n");
    printf("Acumulador               %+06d\n", accumulator);
    printf("Contador de instrucciones %03d\n", instructionCounter);
    printf("Registro de instruccion   %+06d\n", instructionRegister);
    printf("Codigo de operacion       %02d\n", operationCode);
    printf("Operando                  %03d\n", operand);

    printf("\nMEMORIA:\n        ");
    for (i = 0; i < 10; i++)
    {
        printf("%7d", i);
    }
    printf("\n");

    for (i = 0; i < TAM_MEMORIA; i++)
    {
        if (i % 10 == 0)
        {
            printf("%03d   ", i);
        }

        printf("%+06d ", memory[i]);

        if (i % 10 == 9)
        {
            printf("\n");
        }
    }
    printf("============================================\n");
}