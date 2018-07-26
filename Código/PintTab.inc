; Toma Matriz, y pinta de acuerdo a los valores que tenga:
; 0 pinto amarillo, 1 pinto negro, 2 pinto blanco, 20  asterisco sin fondo, 21 asterisco con fondo negro, 22 asterisco con fondo blanco.
; Como aún no se sabe otros colores, se usará el negro como nada, el amarillo y azul como colores adversarios.
; Empiezo pintando la segunda línea, y cada dos líneas pinto.
; Recibe como parámetro: EBX, el Offset de la matriz.  Note que Matriz es un DW, por lo que me tengo que mover de 2 en 2.
pintar_estado_actual_tablero Proc Near
	Xor ESI, ESI ; Contador para Matriz
	Mov DI, Pagina*3 + 40 ; Inicio de la página 3 + más o menos la mitad (para centrar).
	
cambio_fila:
	Xor EDX, EDX ; Contador de columnas.
	; Actualizo la memoria de video.
	Add DI, 160 ; Cambio de línea en consola.
	Push DI ; Para no perder el valor de DI
ciclo_pintar_fila:
;NOTA: Puedo hacerlo más eficiente si aplico CX mod 10, así de un solo tiro puedo determinar color.
	
	Mov CX, [EBX] + ESI ; Guarda el valor del elemento de la matriz de la posición ESI.  Registro de 2 bytes por ser un DW.
	; Comparo el valor para determinar si es:
	
	; Cursor:
	Mov AL, "*" ; Supongo que hay cursor y solo determino color, en caso contrario lo cambiaré después.
	Cmp CX, 20 ; Cursor sin ficha
	JE cursor_sin_ficha
	
	Cmp CX, 21 ; Cursor con ficha celeste
	JE cursor_celeste
	
	Cmp CX, 22 ; Cursor con ficha rojas
	JE cursor_rojo
	
	; No hay cursor
	Mov AL, " " ;
	Cmp CX, 0 ; sin ficha
	JE sin_ficha
	
	Cmp CX, 1 ; Ficha celeste
	JE ficha_celeste
	
	Cmp CX, 2 ; Ficha roja
	JE ficha_rojo
	
	Jmp terminar_pintado_tablero ; Para evitar posible error en Matriz.
	
cursor_sin_ficha:
	Mov AH, LBlanca + FVerde ; El fondo verde significa sin ficha.
	Jmp pintar_casilla
cursor_celeste:
	Mov AH, LBlanca + FCeleste
	Jmp pintar_casilla
cursor_rojo:
	Mov AH, LBlanca + FRojo
	Jmp pintar_casilla
	
sin_ficha:
	Mov AH, FVerde
	Jmp pintar_casilla
ficha_celeste:
	Mov AH, FCeleste
	Jmp pintar_casilla
ficha_rojo:
	Mov AH, FRojo

pintar_casilla:
	; Note que desde acá ya se los valores necesario de AL y AH para pintar en consola.	
	Mov Word ptr ES:[DI], AX ;Imprimo
	Inc DI
	Inc DI ; Siguiente caracter en la memoria de video.
	
	; Muevo el contador de Matriz.
	; Note que me muevo de 2 en 2 porque Matriz es un DW.
	Inc ESI 
	Inc ESI 
	
	Inc EDX ; Cuento la casilla que pinté.
	
	Cmp tamano_tablero_bytes, ESI ; Comparo el contador de Matriz con el tamaño de la Matriz (NO me deja filas*columnas) 
	JE terminar_pintado_tablero ; Si ESI es mayor al tamaño de la matriz, termine., se salió del límite brindados por la Matriz.
	
	Cmp EDX, columnas ; Comparo el contador de columnas con la cantidad de veces que me puedo mover hacia la derecha (como una Matriz).
	JL ciclo_pintar_fila ; Si es menor, aún tengo que pintar casillas en la fila actual.
	Pop DI ; Recupero el valor de DI, para hacer el cambio de línea en memoria.
	Jmp cambio_fila ; Si no, ya terminé de pintar todas las casillas de la fila actual, pinto la fila siguiente.
	
terminar_pintado_tablero:
	Pop DI ; Al terminar, el último Pop de DI no llega, por lo que se tiene que poner aquí para tener en pila dónde retornar.
	ret
pintar_estado_actual_tablero EndP