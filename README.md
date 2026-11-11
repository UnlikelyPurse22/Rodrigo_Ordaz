# Rodrigo_Ordaz
# Simpletron en C++

Este es mi proyecto de Simpletron, un simulador de una computadora sencilla que hice en C++. La idea es que pueda leer instrucciones en lenguaje SML, ejecutarlas y mostrar qué pasa con la memoria y los registros.

Fui agregando varias funciones al programa y esta es la **actualización 8**, donde también se pueden usar números decimales.

## ¿Qué hace el programa?

- Tiene **1,000 posiciones de memoria** para guardar instrucciones y números enteros.
- Puede cargar instrucciones desde un archivo llamado `programa.simp`. Si no encuentra ese archivo, deja escribirlas una por una en la consola.
- Permite leer, mostrar y hacer operaciones con números enteros.
- También puede leer y mostrar cadenas de texto usando caracteres ASCII.
- Incluye saltos para cambiar el orden en el que se ejecutan las instrucciones.
- Tiene otra memoria de **1,000 posiciones** y un acumulador aparte para trabajar con números decimales (`double`).
- Revisa errores como dividir entre cero, usar instrucciones incorrectas o salirse del rango permitido.
- Al terminar, muestra el estado de los
