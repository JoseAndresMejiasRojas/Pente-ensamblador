; Determina el ganador moviéndose a partir de la posición actual donde se puso la ficha,
; se recorre en todas las direcciones (arriba-abajo, derecha_izquierda, diagonales)
; Incluye los procesos requeridos para hacer el recorrido (determinar límtes, determinar jugador, etc).

derecha_izquierda Proc Near	
	Call determinar_limite_izquierdo_derecho
	Call determinar_jugador_actual ; Lo guarda en DX.
	
	; EAX = limite izquierdo
	; EBX = limite derecho
	; DX = jugador actual, 1 celeste, 2 rojo.
	Mov ECX, 4 ; Contador de fichas
	Mov ESI, posicion_actual_cursor
	
ciclo_ganador_derecha: ; DERECHA
	; ¿Gané?
	JECXZ ganador_derecha_izquierda ; Si el contador (ECX) es 0, encontré ganador.
	
	; Me muevo
	Inc ESI
	Inc ESI
	Cmp ESI, EBX
	JA fin_ciclo_ganador_derecha ; Si ESI se salió del límite derecho, termino ciclo.
	
	; ¿La siguiente ficha es mía?
	Cmp DX, Matriz + ESI ; Comparo el jugador actual, con el de Matriz según ESI.
	JNE fin_ciclo_ganador_derecha ; Si no son iguales, termino el ciclo.
	Dec ECX ; Si son iguales, descuento el contador.
	JMP ciclo_ganador_derecha
	
fin_ciclo_ganador_derecha:
	Mov ESI, posicion_actual_cursor ; Restauro la posición, para moverme hacia la izquierda.
ciclo_ganador_izquierda: ; IZQUIERDA
	JECXZ ganador_derecha_izquierda ; Si el contador (ECX) es 0, encontré ganador.
	
	Dec ESI
	Dec ESI
	Cmp ESI, EAX
	JL terminar_derecha_izquierda ; Si me salí del límite izquierdo, termino proceso.
	
	Cmp DX, Matriz + ESI
	JNE terminar_derecha_izquierda ; Si no son iguales, termino proceso.
	Dec ECX ; Si son iguales, decremento.  Note que el registro empieza desde el ciclo hacia la derecha.
	Jmp ciclo_ganador_izquierda
	
ganador_derecha_izquierda:
	Mov ganador, DX
	Ret
terminar_derecha_izquierda:
	Ret
derecha_izquierda EndP

arriba_abajo Proc Near
	; Los límites son: arriba: valores negativos, abajo: número más grande que filas*columnas*2
	Mov EAX, filas
	Mul columnas
	Sal EAX, 1 
	Dec EAX
	Dec EAX ; tamano_matriz * 2 - 2: Obtengo la última posición de Matriz.
	Mov ECX, 4 ; Contador de fichas iguales.
	Mov ESI, posicion_actual_cursor
	
	Call determinar_jugador_actual ; Lo guarda en DX.
	Mov EBX, columnas
	Sal EBX, 1
	; EAX = límite inferior.
	; EBX = columnas (con 2 bytes).
	; ECX = contador de fichas
	; ESI = para movilizarme
	; DX = jugador actual
ciclo_ganador_arriba: ; ARRIBA
	JECXZ ganador_arriba_abajo ; Si el contador (ECX) es 0, encontré ganador.
	
	Sub ESI, EBX ; Resto el tamaño de columnas, para moverme para arriba.
	Cmp ESI, 0
	JL fin_ciclo_ganador_arriba ; Si me salí del límite superior, termino ciclo.
	
	Cmp DX, Matriz + ESI
	JNE fin_ciclo_ganador_arriba
	Dec ECX
	Jmp ciclo_ganador_arriba
	
fin_ciclo_ganador_arriba:
	
	Mov ESI, posicion_actual_cursor
ciclo_ganador_abajo: ; ABAJO
	JECXZ ganador_arriba_abajo ; Si el contador (ECX) es 0, encontré ganador.
	
	Add ESI, EBX ; Sumo el tamaño de columnas, para moverme para abajo.
	Cmp ESI, EAX
	JA terminar_arriba_abajo ; Si me salí del límite superior, termino ciclo.
	
	Cmp DX, Matriz + ESI
	JNE terminar_arriba_abajo
	Dec ECX
	Jmp ciclo_ganador_abajo

ganador_arriba_abajo:
	Mov ganador, DX
terminar_arriba_abajo:
	Ret
arriba_abajo EndP

diagonal_creciente Proc Near
	; Límites:
	Call determinar_limite_izquierdo_derecho ; Se guarda en EAX y EBX respectivamente.
	Call determinar_jugador_actual ; DX
	Mov ECX, 4 ; Contador fichas
	Mov ESI, posicion_actual_cursor
	Mov EDI, columnas
	Sal EDI, 1
	
ciclo_arriba_derecha:
	JECXZ ganador_diagonal_creciente ; Si el contador (ECX) es 0, encontré ganador.
	
	; Me muevo hacia la derecha
	Inc ESI
	Inc ESI
	Cmp ESI, EBX
	JA fin_ciclo_arriba_derecha
	
	; Me muevo hacia arriba
	Sub ESI, EDI ; Resto el tamaño de columnas, para moverme para arriba.
	Cmp ESI, 0
	JL fin_ciclo_arriba_derecha ; Si me salí del límite superior, termino ciclo.
	
	; Actualizo límite derecho
	Sub EBX, EDI ; límite izquierdo - columnas (todo en 2 bytes).
	
	Cmp DX, Matriz + ESI ; Comparo el jugador actual, con el de Matriz según ESI.
	JNE fin_ciclo_arriba_derecha ; Si no son iguales, termino el ciclo.
	Dec ECX ; Si son iguales, descuento el contador.
	JMP ciclo_arriba_derecha

fin_ciclo_arriba_derecha:
	
	Mov ESI, posicion_actual_cursor
	; Para no perder en valor de las variables.
	Push EAX
	Push DX
	
	Mov EAX, filas
	Mul EDI
	Dec EAX
	Dec EAX ; tamano_matriz * 2 - 2: Obtengo la última posición de Matriz.
	Mov EBX, EAX ; EBX ya no lo ocupo, ahora lo uso para el límite inferior.
	
	Pop DX
	Pop EAX
	; EBX = límite inferior.
ciclo_abajo_izquierda:

	JECXZ ganador_diagonal_creciente ; Si el contador (ECX) es 0, encontré ganador.
	
	; Me muevo hacia la izquierda
	Dec ESI
	Dec ESI
	Cmp ESI, EAX
	JL terminar_diagonal_creciente
	
	; Me muevo hacia abajo
	Add ESI, EDI
	Cmp ESI, EBX
	JA terminar_diagonal_creciente 
	
	; Actualizo límite izquierdo
	Add EAX, EDI ; límite izquierdo + columnas (todo en 2 bytes).
	
	Cmp DX, Matriz + ESI ; Comparo el jugador actual, con el de Matriz según ESI.
	JNE terminar_diagonal_creciente ; Si no son iguales, termino el ciclo.
	Dec ECX ; Si son iguales, descuento el contador.
	JMP ciclo_abajo_izquierda


ganador_diagonal_creciente:
	Mov ganador, DX
terminar_diagonal_creciente:
	Ret
	
diagonal_creciente EndP

diagonal_decreciente Proc Near
	; Límites:
	Call determinar_limite_izquierdo_derecho ; Se guarda en EAX y EBX respectivamente.
	Call determinar_jugador_actual ; DX
	Mov ECX, 4 ; Contador fichas
	Mov ESI, posicion_actual_cursor
	Mov EDI, columnas
	Sal EDI, 1
	
ciclo_arriba_izquierda:
	JECXZ ganador_diagonal_decreciente ; Si el contador (ECX) es 0, encontré ganador.
	
	; Me muevo hacia la izquierda
	Dec ESI
	Dec ESI
	Cmp ESI, EAX
	JL fin_ciclo_arriba_izquierda
	
	; Hacia arriba
	Sub ESI, EDI ; Resto el tamaño de columnas, para moverme para arriba.
	Cmp ESI, 0
	JL fin_ciclo_arriba_izquierda ; Si me salí del límite superior, termino ciclo.
	
	; Actualizo límite izquierdo
	Sub EAX, EDI ; límite izquierdo - columnas (todo en 2 bytes).
	
	Cmp DX, Matriz + ESI ; Comparo el jugador actual, con el de Matriz según ESI.
	JNE fin_ciclo_arriba_izquierda ; Si no son iguales, termino el ciclo.
	Dec ECX ; Si son iguales, descuento el contador.
	JMP ciclo_arriba_izquierda
	
fin_ciclo_arriba_izquierda:
	Mov ESI, posicion_actual_cursor
	
	; Para no perder en valor de las variables.
	Push DX
	
	Mov EAX, filas
	Mul EDI
	Dec EAX
	Dec EAX ; tamano_matriz * 2 - 2: Obtengo la última posición de Matriz.
	; EAX ya no lo ocupo, ahora lo uso para el límite inferior.
	
	Pop DX
	; EAX = límite inferior
	
ciclo_abajo_derecha:
	JECXZ ganador_diagonal_decreciente ; Si el contador (ECX) es 0, encontré ganador.

	; Hacia la derecha
	Inc ESI
	Inc ESI
	Cmp ESI, EBX
	JA terminar_diagonal_decreciente

	; Hacia abajo
	Add ESI, EDI
	Cmp ESI, EAX
	JA terminar_diagonal_decreciente
	
	; Actualizo límite derecho
	Add EBX, EDI ; límite izquierdo - columnas (todo en 2 bytes).
	
	Cmp DX, Matriz + ESI ; Comparo el jugador actual, con el de Matriz según ESI.
	JNE terminar_diagonal_decreciente ; Si no son iguales, termino el ciclo.
	Dec ECX ; Si son iguales, descuento el contador.
	JMP ciclo_abajo_derecha

ganador_diagonal_decreciente:
	Mov ganador, DX
terminar_diagonal_decreciente:
	Ret
	
diagonal_decreciente EndP

empate Proc Near
	; Recorre la Matriz en busca de algún 0, si hay un 0, aún no hay empate.
	Xor ESI, ESI ; Para moverme en Matriz.
	
	; Límite único (el derecho inferior)
	Mov EAX, filas
	Mul columnas
	Sal EAX, 1 ; filas*columnas*2 (2 bytes).
	; Este límite NO pueder ser igual a ESI, es un límite inválido.

ciclo_empate: ; No es necesario verificar cursor sin ficha, ya que este proceso siempre se invoca al poner una ficha.
	Cmp Matriz + ESI, 0 
	JE salir_empate ; Si no hay ficha, no hay empate.
	
	; Hay ficha
	Inc ESI
	Inc ESI
	
	; ¿Me salí del límite derecho inferior?
	Cmp ESI, EAX
	JAE empate_determinado ; Si no encontré una casilla sin ficha y terminé de recorrer Matriz, hay empate.
	Jmp ciclo_empate ; Aún no he terminado de recorrer Matriz.
	
empate_determinado:
	Mov ganador, 3
salir_empate:
	Ret
	
empate EndP


determinar_jugador_actual Proc Near
	; Averiguo cuál ficha es (color)
	; Note que al hacer el Call a determinar_ganador, ya el turno se cambió, de ahí que hay que invertir.
	Xor EDX, EDX
	Cmp turno_jugador, 1
	JE determino_rojo ; Si es turno del celeste, la ficha donde estoy es roja.
	
	Mov DX, 1 ; Si es turno del rojo, la ficha es celeste.
	Jmp salir_determinar_jugador
	
determino_rojo:
	Mov DX, 2
salir_determinar_jugador:
	Ret
determinar_jugador_actual EndP

; EAX = limite izquierdo
; EBX = limite derecho
determinar_limite_izquierdo_derecho Proc Near

	Mov EAX, posicion_actual_cursor
	Sar EAX, 1 ; Shift a la derecha, divido entre 2 a EAX, recordando que el tablero es de 2 bytes, por lo que es necesario hacer la división.
	Xor EDX, EDX ; Limpio EDX, ya que con Div, ese registro también lo usa.
	Div columnas ; Divido EAX/columnas, para obtener en EAX el #fila donde está el cursor.
	
	Mul columnas; EAX = Límite izquierdo = #Fila*columna
	Sal EAX, 1 ; Multiplico EAX*2 porque el tablero es DW.
	
	Mov EBX, columnas
	Sal EBX, 1 ; Multiplico EBX*2, por tener en Matriz un DW.
	Add EBX, EAX
	Dec EBX 
	Dec EBX ;  Límite izquierdo + columnas - 2 = Límite derecho = EBX
	
	Ret
	
determinar_limite_izquierdo_derecho EndP