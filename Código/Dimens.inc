captar_dimensiones Proc Near

	; Página 1 probablemente la modifique para informar de errores de escritura.
	Mov AX, 0501h ; Modo 25x80. Página 1 porque AL tiene un 1.
	Int BIOS

	; Dimensiones filas
	Mov DI, Pagina ; Escribir en el inicio de la página 1.
	Mov AH, LBlanca + FAzul ; Letra blanca, fondo azul.
	Mov BX, Offset string_filas ; Lo que voy a escribir
	Call pintar
	
	Call captar_numero	; Capta un número de dos dígitos, lo guarda en EAX
	Mov filas, EAX
	
	Add DI, 160 ; cambio de línea
	Mov AH, LBlanca + FAzul ; Letra blanca, fondo azul.
	Mov BX, Offset string_columnas ; Lo que voy a escribir
	Call pintar
	
	Call captar_numero	; Capta un número de dos dígitos, lo guarda en EAX
	Mov columnas, EAX
	
	Mul filas ; EAX = filas * columnas
	Imul EAX, EAX, 2 ; tamano_tablero_bytes * 2 (por ser un DW)
	Mov tamano_tablero_bytes, EAX

	Ret
captar_dimensiones EndP

; Evita errores de escritura como números grandes o letras.
captar_numero Proc Near ; Retorna el número en EAX
	Push DI
primer_digito:

	Call leer ; Leo primer número, se guarda en Al	
	Call identificar_numero ; Devuelve en AL el número que se digitó.
	
	; El primer número tiene que ser 1 o 0.
	Cmp AL, 1
	JE fin_segundo_digito
	Cmp AL, 0
	JE fin_segundo_digito
	
	Call imprimir_mensaje_tecla_invalida
	Xor EAX, EAX
	Jmp primer_digito ; Tecla inválida.
	
fin_segundo_digito:
	
	Imul EAX, EAX, 10 ; AL * 10 para el primer dígito.
	Mov ECX, EAX ; A este punto, ECX vale 0 o 10.

	; Limpio posible mensaje de error.
	Mov DI, Pagina ; Página 1
	Add DI, 480 ; Línea 3
	Call limpiar_linea
	
segundo_digito:
	
	Call leer ; Segundo número
	Call identificar_numero
	
	Cmp AL, 0
	JL mal_segundo_digito ; Si es menor a 0, tecla incorrecta.
	
	Cmp AL, 9 ; Si es mayor a 9, error.
	JA mal_segundo_digito ; Si es mayor a 9, error.
	
	Cmp ECX, 0 ; Verifico si se digitó 0 o 1 anteriormente.
	JE menor_5 ; Si se digitó 0, el número tiene que ser mayor o igual a 5
	
	Jmp terminar_lectura_dimensiones ; Si se llega hasta aquí, todo correcto.
	
menor_5:
	Cmp AL, 5
	JL mal_segundo_digito ; Si es menor a 5, se digitó un número menor a 5, incorrecto.
	Jmp terminar_lectura_dimensiones ; Todo correcto, número mayor o igual a 5.
	
mal_segundo_digito:
	Call imprimir_mensaje_tecla_invalida ; Tecla incorrecta
	Xor EAX, EAX
	Jmp segundo_digito
	
terminar_lectura_dimensiones: ; Las teclas captadas son correctas.

	Add ECX, EAX ; Obtengo el número digitado.
	; Limpio posible mensaje de error.
	Mov DI, Pagina ; Página 1
	Add DI, 480 ; Línea 3
	Call limpiar_linea
	
	Mov EAX, ECX
	
	Pop DI ; Para no modificar DI
	Ret
captar_numero EndP

; En AL debo de tener 730h / 739h
identificar_numero Proc Near
	Sub EAX, 730h
	Ret
identificar_numero EndP

imprimir_mensaje_tecla_invalida Proc Near
	Mov AH, FAmarillo
	Mov BX, Offset string_error_dimensiones
	Mov DI, Pagina ; Página 1
	Add DI, 480 ; Línea 3
	
	Call Pintar

	Ret
imprimir_mensaje_tecla_invalida EndP












