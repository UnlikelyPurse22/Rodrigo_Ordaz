// Simpletron - Actualizacion 8: Punto flotante double
// C++ sencillo: funciones, arreglos, switch y printf (estilo del proyecto).
// Archivo completo y acumulativo: se usa como simpletron.cpp en cada commit.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <math.h>
#include <limits.h>
#include <cmath>

#define TAM_MEMORIA 1000
#define MINIMO -99999
#define MAXIMO 99998
#define BASE_INSTRUCCION 1000
#define FIN_CARGA 99999

// Los codigos de operacion tienen 2 digitos.
// El operando tiene 3 digitos.

int leerEntero(const char texto[], long long *numero)
{
    char *fin;
    errno = 0;
    while (isspace((unsigned char)*texto)) texto++;
    if (*texto == '\0') return 0;
    long long valor = strtoll(texto, &fin, 10);
    if (errno == ERANGE || fin == texto) return 0;
    while (isspace((unsigned char)*fin)) fin++;
    if (*fin != '\0') return 0;
    *numero = valor;
    return 1;
}

int leerLinea(char buffer[], int capacidad)
{
    if (fgets(buffer, capacidad, stdin) == NULL) return 0;
    size_t longitud = strlen(buffer);
    if (longitud > 0 && buffer[longitud - 1] == '\n') {
        buffer[--longitud] = '\0';
    } else if (!feof(stdin)) {
        // Evita aceptar una linea truncada; limpia el resto.
        int caracter;
        caracter = getchar();
        if (caracter != '\n' && caracter != EOF) {
            while ((caracter = getchar()) != '\n' && caracter != EOF) { }
            return 0;
        }
    }
    longitud = strlen(buffer);
    if (longitud > 0 && buffer[longitud - 1] == '\r') buffer[longitud - 1] = '\0';
    return 1;
}

// 1 = se cargo correctamente; 0 = error (no ejecutar nada).
// Si no existe programa.simp se usa carga interactiva.
int cargarPrograma(int memory[], int *instruccionesCargadas)
{
    FILE *archivo = fopen("programa.simp", "r");
    int temporal[TAM_MEMORIA] = {0};
    int contador = 0;
    char linea[2048];
    if (archivo != NULL) {
        int numeroLinea = 0;
        printf("Cargando programa desde programa.simp...\n");
        while (fgets(linea, sizeof(linea), archivo) != NULL) {
            numeroLinea++;
            size_t longitud = strlen(linea);
            if (longitud == sizeof(linea)-1 && linea[longitud-1] != '\n') {
                printf("ERROR: linea %d demasiado larga.\n", numeroLinea);
                fclose(archivo);
                return 0;
            }
            char *inicio = linea;
            while (isspace((unsigned char)*inicio)) inicio++;
            if (*inicio == '\0' || *inicio == '#') continue;
            long long valor;
            if (!leerEntero(inicio, &valor) || valor < MINIMO || valor > FIN_CARGA) {
                printf("ERROR: linea %d invalida en programa.simp.\n", numeroLinea);
                fclose(archivo);
                return 0;
            }
            if (valor == FIN_CARGA) break;
            if (contador >= TAM_MEMORIA) {
                printf("ERROR: programa supera las %d posiciones.\n", TAM_MEMORIA);
                fclose(archivo);
                return 0;
            }
            temporal[contador++] = (int)valor;
        }
        if (ferror(archivo)) {
            printf("ERROR: no se pudo leer todo el archivo.\n");
            fclose(archivo);
            return 0;
        }
        fclose(archivo);
    } else if (errno == ENOENT) {
        printf("No existe programa.simp. Carga interactiva.\n");
        printf("Ingrese instrucciones; %d para finalizar.\n", FIN_CARGA);
        while (contador < TAM_MEMORIA) {
            printf("%03d ? ", contador);
            fflush(stdout);
            if (!leerLinea(linea, sizeof(linea))) {
                printf("ERROR: entrada inexistente o demasiado larga.\n");
                return 0;
            }
            long long valor;
            if (!leerEntero(linea, &valor) || valor < MINIMO || valor > FIN_CARGA) {
                printf("Valor invalido. Rango %d a %d.\n", MINIMO, MAXIMO);
                continue;
            }
            if (valor == FIN_CARGA) break;
            temporal[contador++] = (int)valor;
        }
    } else {
        printf("ERROR: no se puede abrir programa.simp (codigo %d).\n", errno);
        return 0;
    }
    if (contador == 0) {
        printf("ERROR: el programa esta vacio.\n");
        return 0;
    }
    for (int i=0; i<TAM_MEMORIA; i++) memory[i] = temporal[i];
    *instruccionesCargadas = contador;
    printf("Se cargaron %d palabras de programa.\n", contador);
    return 1;
}



// Devuelve 1 cuando termina con ALTO; 0 ante error fatal.
int ejecutarPrograma(int memory[], int *accumulator, int *instructionCounter,
                     int *instructionRegister, int *operationCode, int *operand,
                     int instruccionesCargadas, double floatMemory[], double *floatAccumulator)
{
    int ejecutando = 1;
    while (ejecutando) {
        if (*instructionCounter < 0 || *instructionCounter >= TAM_MEMORIA) {
            printf("ERROR FATAL: contador fuera de memoria.\n");
            return 0;
        }
        *instructionRegister = memory[*instructionCounter];
        if (*instructionRegister < 0) {
            printf("ERROR FATAL: instruccion negativa no valida.\n");
            return 0;
        }
        *operationCode = *instructionRegister / BASE_INSTRUCCION;
        *operand = *instructionRegister % BASE_INSTRUCCION;
        (*instructionCounter)++;
        if (*operand < 0 || *operand >= TAM_MEMORIA) {
            printf("ERROR FATAL: direccion %d fuera de memoria.\n", *operand);
            return 0;
        }
        long long resultado = 0;
        switch (*operationCode) {
            case 10: {  // LEER entero
                char texto[256];
                long long valor;
                printf("Ingrese un entero: ");
                fflush(stdout);
                if (!leerLinea(texto, sizeof(texto)) || !leerEntero(texto, &valor)
                    || valor < MINIMO || valor > MAXIMO) {
                    printf("ERROR FATAL: entero invalido o fuera de rango.\n");
                    return 0;
                }
                memory[*operand] = (int)valor;
                break;
            }
            case 11:  // ESCRIBIR entero
                printf("Salida: %d\n", memory[*operand]);
                break;
            case 20:  // CARGAR
                *accumulator = memory[*operand];
                break;
            case 21:  // ALMACENAR
                memory[*operand] = *accumulator;
                break;
            case 30:  // SUMAR
                resultado = (long long)*accumulator + memory[*operand];
                if (resultado < MINIMO || resultado > MAXIMO) {
                    printf("ERROR FATAL: desbordamiento en suma.\n");
                    return 0;
                }
                *accumulator = (int)resultado;
                break;
            case 31:  // RESTAR
                resultado = (long long)*accumulator - memory[*operand];
                if (resultado < MINIMO || resultado > MAXIMO) {
                    printf("ERROR FATAL: desbordamiento en resta.\n");
                    return 0;
                }
                *accumulator = (int)resultado;
                break;
            case 32:  // DIVIDIR (division entera)
                if (memory[*operand] == 0) {
                    printf("ERROR FATAL: division entre cero.\n");
                    return 0;
                }
                resultado = (long long)*accumulator / memory[*operand];
                if (resultado < MINIMO || resultado > MAXIMO) {
                    printf("ERROR FATAL: desbordamiento en division.\n");
                    return 0;
                }
                *accumulator = (int)resultado;
                break;
            case 33:  // MULTIPLICAR
                resultado = (long long)*accumulator * memory[*operand];
                if (resultado < MINIMO || resultado > MAXIMO) {
                    printf("ERROR FATAL: desbordamiento en multiplicacion.\n");
                    return 0;
                }
                *accumulator = (int)resultado;
                break;
            case 12:  // NUEVA LINEA (el operando se ignora)
                putchar('\n');
                break;
            case 13: {  // LEER CADENA ASCII
                // Formato 1ra palabra LLAAA: LL=longitud (00..99), AAA=primer ASCII.
                // Cada caracter adicional se guarda en una palabra 00AAA.
                // Es un caracter por palabra: un entero de 5 digitos no admite dos ASCII de 3 digitos.
                char texto[1024];
                printf("Ingrese una cadena (hasta 99 caracteres ASCII): ");
                fflush(stdout);
                if (!leerLinea(texto, sizeof(texto))) {
                    printf("ERROR FATAL: cadena demasiado larga o entrada no disponible.\n");
                    return 0;
                }
                int longitud = (int)strlen(texto);
                if (longitud > 99 || (longitud > 0 && *operand + longitud > TAM_MEMORIA)) {
                    printf("ERROR FATAL: cadena fuera de los limites de memoria o de longitud.\n");
                    return 0;
                }
                for (int i=0; i<longitud; i++) {
                    if ((unsigned char)texto[i] > 127) {
                        printf("ERROR FATAL: solo se admite ASCII de 0 a 127.\n");
                        return 0;
                    }
                }
                // No sobrescribir instrucciones del programa.
                if (*operand < instruccionesCargadas) {
                    printf("ERROR FATAL: la cadena invadiria las instrucciones.\n");
                    return 0;
                }
                memory[*operand] = longitud * 1000 +
                                   (longitud > 0 ? (unsigned char)texto[0] : 0);
                for (int i=1; i<longitud; i++) {
                    memory[*operand + i] = (unsigned char)texto[i];
                }
                break;
            }
            case 14: {  // ESCRIBIR CADENA ASCII (sin agregar nueva linea)
                int palabra = memory[*operand];
                if (palabra < 0) {
                    printf("ERROR FATAL: cabecera de cadena invalida.\n");
                    return 0;
                }
                int longitud = palabra / 1000;
                int primero = palabra % 1000;
                if (longitud > 99 || (longitud == 0 && primero != 0) ||
                    (longitud > 0 && (*operand + longitud > TAM_MEMORIA || primero > 127))) {
                    printf("ERROR FATAL: longitud/direccion/ASCII de cadena invalida.\n");
                    return 0;
                }
                // Verificar toda la cadena ANTES de imprimir.
                for (int i=1; i<longitud; i++) {
                    if (memory[*operand+i] < 0 || memory[*operand+i] > 127) {
                        printf("ERROR FATAL: ASCII invalido en la cadena.\n");
                        return 0;
                    }
                }
                if (longitud > 0) putchar((char)primero);
                for (int i=1; i<longitud; i++) putchar((char)memory[*operand+i]);
                break;
            }
            case 34:  // RESIDUO: acumulador % memoria[operando]
                if (memory[*operand] == 0) {
                    printf("ERROR FATAL: modulo entre cero.\n");
                    return 0;
                }
                *accumulator = *accumulator % memory[*operand];
                break;
            case 35: {  // POTENCIA: acumulador ^ memoria[operando]
                int exponente = memory[*operand];
                if (exponente < 0) {
                    printf("ERROR FATAL: exponente negativo no admitido.\n");
                    return 0;
                }
                long long potencia = 1;  // 0^0 se define como 1 en esta maquina.
                for (int i=0; i<exponente; i++) {
                    potencia *= *accumulator;
                    if (potencia < MINIMO || potencia > MAXIMO) {
                        printf("ERROR FATAL: desbordamiento en potencia.\n");
                        return 0;
                    }
                }
                *accumulator = (int)potencia;
                break;
            }
            case 40:  // GOTO
                *instructionCounter = *operand;
                break;
            case 41:  // GOTO SI NEGATIVO
                if (*accumulator < 0) *instructionCounter = *operand;
                break;
            case 42:  // GOTO SI CERO
                if (*accumulator == 0) *instructionCounter = *operand;
                break;
            case 43:  // ALTO
                ejecutando = 0;
                break;
            case 50: {  // LEER FLOAT (double) hacia memoria flotante
                char texto[256];
                printf("Ingrese un numero decimal: ");
                fflush(stdout);
                if (!leerLinea(texto, sizeof(texto))) {
                    printf("ERROR FATAL: entrada decimal no disponible.\n");
                    return 0;
                }
                char *fin;
                errno = 0;
                const char *inicio = texto;
                while (isspace((unsigned char)*inicio)) inicio++;
                double valor = strtod(inicio, &fin);
                while (isspace((unsigned char)*fin)) fin++;
                if (fin == inicio || *fin != '\0' || errno == ERANGE || !std::isfinite(valor)) {
                    printf("ERROR FATAL: numero decimal invalido.\n");
                    return 0;
                }
                floatMemory[*operand] = valor;
                break;
            }
            case 51:  // ESCRIBIR FLOAT
                printf("Salida decimal: %.15g\n", floatMemory[*operand]);
                break;
            case 52:  // CARGAR FLOAT
                *floatAccumulator = floatMemory[*operand];
                break;
            case 53:  // ALMACENAR FLOAT
                floatMemory[*operand] = *floatAccumulator;
                break;
            case 54:  // SUMAR FLOAT
                *floatAccumulator += floatMemory[*operand];
                if (!std::isfinite(*floatAccumulator)) {
                    printf("ERROR FATAL: desbordamiento decimal.\n");
                    return 0;
                }
                break;
            case 55:  // RESTAR FLOAT
                *floatAccumulator -= floatMemory[*operand];
                if (!std::isfinite(*floatAccumulator)) {
                    printf("ERROR FATAL: desbordamiento decimal.\n");
                    return 0;
                }
                break;
            case 56:  // MULTIPLICAR FLOAT
                *floatAccumulator *= floatMemory[*operand];
                if (!std::isfinite(*floatAccumulator)) {
                    printf("ERROR FATAL: desbordamiento decimal.\n");
                    return 0;
                }
                break;
            case 57:  // DIVIDIR FLOAT
                if (floatMemory[*operand] == 0.0) {
                    printf("ERROR FATAL: division decimal entre cero.\n");
                    return 0;
                }
                *floatAccumulator /= floatMemory[*operand];
                if (!std::isfinite(*floatAccumulator)) {
                    printf("ERROR FATAL: desbordamiento decimal.\n");
                    return 0;
                }
                break;
            default:
                printf("ERROR FATAL: codigo de operacion invalido: %d.\n", *operationCode);
                return 0;
        }
    }
    printf("\nTermino la ejecucion de Simpletron.\n");
    return 1;
}

void vaciadoMemoria(int memory[], int accumulator, int instructionCounter,
                    int instructionRegister, int operationCode, int operand, double floatMemory[], double floatAccumulator)
{
    printf("\n=============== VACIADO DE SIMPLETRON ===============\n");
    printf("REGISTROS:\n");
    printf("Acumulador:              %+06d\n", accumulator);
    printf("Contador de instrucciones: %03d\n", instructionCounter);
    printf("Registro de instruccion: %+06d\n", instructionRegister);
    printf("Codigo de operacion:    %02d\n", operationCode);
    printf("Operando:               %03d\n", operand);
    printf("Acumulador flotante: %.15g\n", floatAccumulator);
    printf("MEMORIA FLOTANTE (celdas diferentes de cero):\n");
    int total = 0;
    for (int i=0; i<TAM_MEMORIA; i++) {
        if (floatMemory[i] != 0.0) {
            printf("  [%03d] = %.15g\n", i, floatMemory[i]);
            total++;
        }
    }
    if (!total) printf("  (sin valores no nulos)\n");
    printf("MEMORIA ENTERA:\n        ");
    for (int i=0; i<10; i++) printf("%8d", i);
    printf("\n");
    for (int i=0; i<TAM_MEMORIA; i++) {
        if (i % 10 == 0) printf("%03d     ", i);
        printf("%+06d  ", memory[i]);
        if (i % 10 == 9) printf("\n");
    }
    printf("=====================================================\n");
}

int main()
{
    int memory[TAM_MEMORIA] = {0};
    int accumulator = 0;
    int instructionCounter = 0;
    int instructionRegister = 0;
    int operationCode = 0;
    int operand = 0;
    int instruccionesCargadas = 0;
    double floatMemory[TAM_MEMORIA] = {0.0};
    double floatAccumulator = 0.0;
    printf("*** Bienvenido a Simpletron - Actualizacion 8 ***\n");
    if (!cargarPrograma(memory, &instruccionesCargadas)) return 1;
    printf("Comienza la ejecucion de Simpletron.\n");
    int exito = ejecutarPrograma(memory, &accumulator, &instructionCounter,
                                &instructionRegister, &operationCode, &operand,
                                instruccionesCargadas, floatMemory, &floatAccumulator);
    vaciadoMemoria(memory, accumulator, instructionCounter,
                   instructionRegister, operationCode, operand, floatMemory, floatAccumulator);
    return exito ? 0 : 1;
}