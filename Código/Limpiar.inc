; Pone en negro una línea en específico.
limpiar_linea Proc Near
; Recibo como parámetro: DI como el principio de la línea.
	Xor AH, AH ; Negro
	Xor ESI, ESI ; Contador
	
ciclo_linea:
	Mov BX, Offset string_espacio ; Lo que voy a escribir
	Call pintar
	Inc DI
	Inc DI ; Siguiente caracter
	Inc ESI
	
	Cmp ESI, 80
	JL ciclo_linea ; Si no he limpiado todos los caractares de la línea, continúo borrando.

	Ret
limpiar_linea EndP

; ECX = cantidad de líneas que se quiere borrar.
; DI = principio de la página que se quiere borrar.
limpiar_pag Proc Near

	; Para limpiar la página acorde el parámetro de ECX.
	Xor ESI, ESI ; Contador

ciclo_pag:
	Push ESI
	Push DI
	Call limpiar_linea
	Pop DI ; Para no perder el valor original de DI.
	Pop ESI ; limpiar_linea modifica ESI, por lo que necesito mandarlo a pila para no perder el valor original.
	
	Inc ESI ; Línea borrada, incremento el contador.
	Add DI, 160 ; Cambio de línea.
	
	Cmp ESI, ECX ; ¿Ya borré todas las líneas solicitadas?
	JL ciclo_pag ; Si aún no he borrado las líneas, continúo.
	
	Ret
limpiar_pag EndP