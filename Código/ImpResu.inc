; Al llegar aquí, se imprime el resultado en la página 3.
desplegar_resultado:
	; Determino qué tengo que imprimir:
	; ganador es 1, rojo es el ganador, si es 2, celeste es el ganador, si es 3, empate.
	Mov AH, FAmarillo
	Mov BX, Offset string_resultado
	Mov DI, Pagina*3
	Add DI, 3360 ; En la línea 2.
	Call Pintar
	
	Add DI, 160 ; Cambio de línea.
	Push DI
	
	Cmp ganador, 3 ; ¿Empate?
	JE resultado_empate
	
	Mov BX, Offset string_resultado_ganador
	Call Pintar
	Add DI, 28 ; Me muevo hacia la derecha.
	
	Cmp ganador, 2
	JE ganador_rojo
	
ganador_celeste:
	Mov BX, Offset string_jugador_celeste
	Jmp pintar_resultado

ganador_rojo:
	Mov BX, Offset string_jugador_rojo
	Jmp pintar_resultado

resultado_empate:
	Mov BX, Offset string_resultado_empate	
	
pintar_resultado:
	Call Pintar
	Pop DI
	
	Add DI, 160
	Mov BX, Offset string_resultado2
	Call Pintar

	Mov AX, 0503h ; Veo página 3
	Int BIOS

ciclo_siguiente_accion_resultado:
	Call leer
	Cmp AL, 27 ; Comparo si es Esc
	JE salir
	
	Cmp AL, 00
	JNE ciclo_siguiente_accion_resultado
	
	Call leer
	Cmp AL, 44h
	JE reiniciar ; Error al reiniciar.
	
	Jmp ciclo_siguiente_accion_resultado