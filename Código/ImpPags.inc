; Este proceso escribe las páginas que no cambian en el programa.
; Note que usa las variables de Segmento de datos, que se encuentran en el archivo principal.
escribir_paginas_estaticas Proc Near

	; Página 4
	Mov DI, Pagina*4
	Mov AH, LBlanca + FAzul ; Letra blanca, fondo azul.
	Mov BX, Offset string_salir
	Call pintar
	
	; Página 2
	Mov DI, Pagina*2
	Mov AH, LVerde + 0; Letra verde, fondo negro
	; pintar al retornar, devuelve DI con el valor original con el que se llamó.
	;Texto ayuda:
	Mov BX, Offset string_ayuda1
	Call pintar
	Add DI, 320
	Mov BX, Offset string_ayuda2
	Call pintar
	Add DI, 160
	Mov BX, Offset string_ayuda3
	Call pintar
	Add DI, 160
	Mov BX, Offset string_ayuda4
	Call pintar
	Add DI, 160
	Mov BX, Offset string_ayuda5
	Call pintar
	Add DI, 160
	Mov BX, Offset string_ayuda6
	Call pintar
	Add DI, 160
	Mov BX, Offset string_ayuda7
	Call pintar
	Add DI, 320
	Mov BX, Offset string_ayuda8
	Call pintar
	Add DI, 320
	Mov BX, Offset string_ayuda9
	Call pintar
	Add DI, 160
	Mov BX, Offset string_ayuda10
	Call pintar
	Add DI, 160
	Mov BX, Offset string_ayuda11
	Call pintar
	Add DI, 160
	Mov BX, Offset string_ayuda12
	Call pintar
	Add DI, 320
	Mov BX, Offset string_ayuda13
	Call pintar
	Add DI, 350 ; Con 351 muestra en consola cosas no esperadas.
	Mov BX, Offset string_ayuda14
	Call pintar
	
	Ret
escribir_paginas_estaticas EndP