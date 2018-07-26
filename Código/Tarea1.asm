; Páginas:
; 0: no se usa.
; 1: captar dimensiones.
; 2: ayuda.
; 3: tablero, principal.
; 4: salir.
; 5: solicitar nombres.
DOS			Equ 21H
Bios		Equ 10H
SegVideo	Equ 0B800h
Pagina		Equ 2*2*1024 ; Multiplico n por Pagina y escribe en el inicio de la página n.

; Colores (Tomado del archivo Video0.asm)
; Fondos (Background)
FNegro		Equ 0h
FAmarillo	Equ 0E0h
FVerde		Equ	0A0h
FAzul		Equ 10h
FCeleste	Equ 030h
FRojo		Equ 040h

; Letras (Foreground)
LAmarilla	Equ FAmarillo/16
LVerde		Equ FVerde/16
LAzul		Equ FAzul/ 16
LCeleste	Equ FCeleste/16
LBlanca		Equ 0Fh

; Segmentos de datos
Datos Segment

; Primera posición en 20, el resto en 0.  19*19 es el máximo tamaño posible.
; 02D0 primera casilla, FINAL DE MATRIZ (en el debugger).  Note los últimos 2 bytes no se usan.
Matriz	DW 20
		DW 19 * 19 Dup(0)
		; No funciona con 19 * 19 -1 ¿Por qué?

; tamaño que se va a manipular acorde al usuario.
; (4 bytes)
filas 					DD 5
columnas				DD 5
tamano_tablero_bytes 	DD ? ; Sin valor, ya que se asigna al solicitar las dimensiones del usuario.
posicion_actual_cursor	DD 0 ; Por default, el cursor está en 0.
turno_jugador			DD 1 ; Inicia por default en celeste. 1 celeste, 2 rojo.

ganador	DW 0 ; Si hay ganador, la variable estará en 1 o 2, dependiendo del jugador, si hay un 3, es empate.

Include Strings.inc ; Strings necesarios para imprimir en la pantalla.
		
EndS Datos

; Pila
Pila Segment Stack "Stack"
	DW 100 Dup (0)
Pila EndS

Codigo Segment
.486
Assume CS:Codigo, DS:Datos, SS:Pila

; Archivos incluidos:
;Al parecer, nombres con más de 7 caracteres, da error.
Include ImpPags.inc ; Imprimir páginas estáticas.
Include PintTab.inc ; Pintar estado actual del tablero.
Include Limpiar.inc ; Limpia las páginas 1, 3 y 5, incluye el proceso para limpiar una línea.
Include Dimens.inc ; Asigna las dimensiones, contienelos métodos necesarios para leer los números.
Include ImpResu.inc ; Imprime el resultado: ganador celeste, ganador rojo o empate.
Include Ganador.inc ; Procesos para determinar el ganador en una partida.

main:
	; Tomado de vidoe0.asm, ¿por qué hace eso?
	Mov AX, Datos
	Mov DS, AX
	Mov AX, SegVideo
	Mov ES, AX

	Call escribir_paginas_estaticas
	
inicio_juego:
	Mov AX, 0501h ; ; El juego inicia en la página 1
	
	Call captar_dimensiones
	
solicitar_nombres:
	; ROJO
	Mov DI, Pagina*5
	Mov AH, LBlanca + FRojo	; Letra blanca, fondo rojo.
	Mov BX, Offset string_rojo
	Call pintar
	Mov AX, 0505h ; Veo página 5.
	Int BIOS
	Mov ECX, Offset string_jugador_rojo ; Parámetro para guardar_nombre
	Call guardar_nombre
	
	; CELESTE
	Mov AH, LBlanca + FCeleste ; Letra blanca, fondo rojo.
	Mov BX, Offset string_celeste
	Call pintar
	Mov ECX, Offset string_jugador_celeste
	Call guardar_nombre
	
	Mov AX, 0503h ; Veo página 3
	Int BIOS
	
esperar_tecla:
	
	Call pintar_turno
	Mov EBX, Offset Matriz ; Parámetro para pintar el tablero.
	Call pintar_estado_actual_tablero
	
	Cmp ganador, 0
	JNE desplegar_resultado ; Si hay un resultado, imprímalo.
	
	Call leer ; La tecla se guarda en AL.
	
	Cmp AL, 27 ; Comparo si es Esc
	JNE no_salir
	
	Mov AX, 0504h ; Veo página 4.
	Int BIOS
	Call leer
	Cmp AL, 27 ; Comparo si es Esc
	JE salir ; Si es Esc, definitivamente salgo.
	; Si no
	Mov AX, 0503h ; Veo página 3.
	Int BIOS
	
	Jmp esperar_tecla
	
salir:
	Call limpiar_paginas

	Mov AX, 0500h ; Antes de salir, pasar a página 0.
	Int BIOS
	
	Mov AH,4CH
	Int DOS ; Terminar programa.
	
no_salir:
	Cmp AL, 020h ; ¿Es espacio?
	Push EAX
	JE poner_ficha
	Pop EAX

	Cmp AL, 00 ; ¿Es una tecla especial?
	JE leer_tecla_especial ; Entonces lea la tecla especial.
	
	Jmp esperar_tecla ; Si no, tecla inválida, espere otra.
	
poner_ficha: ; 1 turno celeste, 2 turno amarillo.
	Mov EBX, Offset Matriz
	Mov ECX, Offset turno_jugador
	Mov EDX, Offset posicion_actual_cursor
	Mov EDX, [EDX]
	
	; Verificar si es correcto poner ficha:
	Mov AX, [EBX]+EDX
	Cmp AX, 20 ; 
	JNE esperar_tecla ; Si en el cursor hay una ficha, no haga nada.
	
	; Sí se puede poner una ficha:
	Cmp [ECX], 1 ; ¿Es turno del celeste?
	JE poner_celete

poner_amarillo: ; Si no es celeste, es amarillo.
	Mov turno_jugador, 1 ; Ahora es turno del celeste
	Mov Matriz + EDX, 22 ; Pongo ficha amarilla en la posición del cursor.
	Call determinar_ganador ; Al poner ficha, se determina si hay ganador.
	Jmp esperar_tecla
	
poner_celete:
	Mov turno_jugador, 2 ; Ahora es turno del amarillo
	Mov Matriz + EDX, 21 ; Pongo ficha celeste en la posición del cursor.
	Call determinar_ganador
	Jmp esperar_tecla
	
leer_tecla_especial:
	Mov AH, 07h
	INT DOS ; En AL, lo que se guarda es el valor de la tecla especial (F1, flechas...)
	
	;Comparaciones:
	
	; F1, valor en AL: 3Bh
	Cmp AL, 3Bh
	JE ayuda
	
	; F10, valor en AL: 44h
	Cmp AL, 44h
	JE reiniciar
	
	; Flecha derecha, valor en AL: 4Dh
	Cmp AL, 4Dh
	JE derecha
	
	; Flecha izquierda, valor en AL: 4Bh
	Cmp AL, 4Bh
	JE izquierda
	
	; Flecha abajo, valor en AL: 50h
	Cmp AL, 50h
	JE abajo
	
	; Flecha arriba, valor en AL: 48h
	Cmp AL, 48h
	JE arriba
	
	Jmp esperar_tecla ; Tecla especial inválida, esperar otra.
		
ayuda:
	Mov AX, 0502H ; Pasar a página 2.
	Int BIOS
	
	Call leer ; Si digita cualquier letra, se cierra la página de ayuda.
	
	Mov AX, 0503h ; Si se digita cualquier tecla, se cierra la página de ayuda.
	Int BIOS;
	
	Jmp esperar_tecla
	
reiniciar:
	
	Mov ECX, 2
	Mov DI, Pagina
	Call limpiar_pag ; Borro página 1, 2 líneas.
	
	
	Mov ECX, 24
	Mov DI, Pagina*3
	Call limpiar_pag ; Borro página 3, 24 líneas.
	
	Mov ECX, 4
	Mov DI, Pagina*5
	Call limpiar_pag ; Borro página 5, 4 líneas.
	
	; Restauro los valores de Matriz.
	Mov Matriz, 20 ; La primera posición es el cursor.
	
	Mov EDX, 2 ; Inicio desde la segunda posición
ciclo_limpiar_matriz:
	Mov Matriz + EDX, 0 ; Empiezo a limpiar Matriz.
	Inc EDX
	Inc EDX ; Me muevo un dato a la derecha (recuerde que es DW, 2 bytes).
	
	Cmp EDX, 19*19*2
	JL ciclo_limpiar_matriz ; Si no me he pasado del límite de Matriz.
	
	; Restauro valores
	Mov posicion_actual_cursor, 0
	Mov turno_jugador, 1
	Mov ganador, 0
	Jmp inicio_juego ; Vuelvo a solicitar datos.
	
; Para saber a qué le tengo que resta 20, necesito la posición 20 actualizada.	
derecha: ; +20 siguiente, -20 actual,  +2 porque Matriz es DW.
	Mov EBX, Offset posicion_actual_cursor
	Mov ECX, [EBX] ; 
	Add ECX, 2 ; ECX = posicion_actual_cursor + 2
	
	Cmp tamano_tablero_bytes, ECX; ¿No me salgo de los límites?
	JBE esperar_tecla ; Si sí, no haga nada.
	
	; Actualizo siguiente
	Add Matriz + ECX, 20 ; Matriz + posicion_actual_cursor+2 += 20

	; Actualizo el actual
	Mov ECX, [EBX] ; Para quitarle el +2, es más barato el Mov que Sub.
	Sub Matriz + ECX, 20
	
	Add posicion_actual_cursor, 2 ; Actulizo la variable
	
	Jmp esperar_tecla
	
izquierda:
	Mov EBX, Offset posicion_actual_cursor
	Mov ECX, [EBX] ; 
	Sub ECX, 2 ; ECX = posicion_actual_cursor - 2
	
	Cmp ECX, 0; ¿No me salgo de los límites?
	JL esperar_tecla ; Si sí, no haga nada.
	
	; Actualizo siguiente
	Add Matriz + ECX, 20 ; Matriz + posicion_actual_cursor+2 += 20
	
	; Actualizo el actual
	Mov ECX, [EBX] ; Para quitarle el -2, es más barato el Mov que Sub.
	Sub Matriz + ECX, 20
	
	Sub posicion_actual_cursor, 2 ; Actulizo la variable


	Jmp esperar_tecla
	
abajo: ; Se suma columnas * 2 + posicion_actual_cursor

	Mov EBX, Offset posicion_actual_cursor
	Imul ECX, columnas, 2 ; 
	Add ECX, [EBX] ; ECX = columnas * 2 + posicion_actual_cursor
	
	
	Cmp tamano_tablero_bytes, ECX; ¿No me salgo de los límites?
	JBE esperar_tecla ; Si sí, no haga nada.
	
	; Actualizo siguiente
	Add Matriz + ECX, 20 ; Matriz + posicion_actual_cursor+columna += 20
	
	; Actualizo el actual
	Mov ECX, [EBX] ; Para quitarle el +columna, es más barato el Mov que Sub.
	Sub Matriz + ECX, 20
	
	Imul ECX, columnas, 2
	Add ECX, [EBX]
	Mov posicion_actual_cursor, ECX ; Actulizo la variable
	
	Jmp esperar_tecla
	
arriba:
	Mov EBX, Offset posicion_actual_cursor
	Imul EDX, columnas, 2
	Mov ECX, [EBX] ; ECX = posición actual
	Sub ECX, EDX ; ECX = columnas * 2 - posicion_actual_cursor
	
	
	Cmp ECX, 0; ¿No me salgo de los límites?
	JL esperar_tecla ; Si sí, no haga nada.
	
	; Actualizo siguiente
	Add Matriz + ECX, 20 ; Matriz - posicion_actual_cursor+columna += 20
	
	; Actualizo el actual
	Mov ECX, [EBX] ; Para quitarle el +columna, es más barato el Mov que Sub.
	Sub Matriz + ECX, 20
	
	; Actualizo variable
	Sub ECX, EDX
	Mov posicion_actual_cursor, ECX ; Actulizo la variable
	
	Jmp esperar_tecla

leer Proc Near
	Mov AH,07h ; Leo tecla, se guarda en AL
	Int DOS
	Ret
leer EndP	
	
pintar Proc Near
; Recibe como parámetros:
; AH el color de la hilera.
; BX el offset de la hilera. NOTA: tiene que tener fin ($).
; DI el offset de la memoria de video donde quiero que inicie la hilera.
	Push DI
pintar_caracter:

	;Note que se recorre caracter por caracter 
	Mov AL, [BX] ; se guarda en AL un caracter de la hilera a pintar.
	Cmp AL, "$"  ; ¿Es el final de la hilera?
	JE terminar_pintar ; Si es el final, termine.
	Mov Word ptr ES:[DI], AX ; Pinte.
	Inc DI
	Inc DI ; Siguiente palabra en la memoria de video.
	Inc BX ; Siguiente caracter de la hilera a pintar.
	Jmp pintar_caracter
	
terminar_pintar:
	Pop DI
	Ret ; Note que en pintar siempre se llama con un Call, por lo que el Ret lo que hace es devolverlo en la línea siguiente
		; a donde se llamó pintar.
pintar EndP

; Recibe como parámetro en ECX el offset del string que se va a modificar.
; Note que se da por un hecho de que el string tiene 6 caracteres disponibles.
guardar_nombre Proc Near
	Xor ESI, ESI ; Contador de caracteres
	Push DI
	
ciclo_guardar_nombre:
	Call leer
	Cmp AL, 0Dh ; ¿Presionó Enter?
	JE caracter_enter
	
	Cmp AL, 024h ; Signo dollar
	JE error_nombre
	
	Cmp ESI, 6
	JE ciclo_guardar_nombre ; Si es 6, no se puede escribir, no haga nada.
	
	; Guardo caracter
	Mov [ECX] + ESI, AL ; Guardo lo capté en leer.
	Inc ESI ; Note que funciona porque el string se asume que es un DB
	Jmp ciclo_guardar_nombre
	
error_nombre:	
	
	Mov AH, FAmarillo
	Mov DI, Pagina*5
	Add DI, 320
	
	; Imprimir dolar
	Mov AL, "$" ; se guarda en AL un caracter de la hilera a pintar.
	Mov Word ptr ES:[DI], AX ; Pinte $
	
	Inc DI
	Inc DI ; Como pinté $, me muevo uno a la derecha.
	
	Mov BX, Offset string_error_nombres
	Call Pintar
	
	Jmp ciclo_guardar_nombre
	
caracter_enter:
	Mov [ECX] + ESI, "$" ; Fin de string.
	Pop DI
	Ret
guardar_nombre EndP


pintar_turno Proc Near

	Mov DI, Pagina*3+144
	Mov AH, LBlanca + FCeleste ; Letra blanca, fondo celeste.
	Mov BX, Offset string_jugador_celeste
	Call pintar
	
	Add DI, 160
	Mov AH, LBlanca + FRojo
	Mov BX, Offset string_jugador_rojo
	Call pintar
	
	Mov DI, Pagina*3+158 ; Posicion para escribir en el lado celeste.
	Cmp turno_jugador, 1 ; ¿Es el turno para el celeste?
	JE turno_celeste_pintar
	
	; TURNO ROJO
	Xor AH, AH ; Para que pinte en negro
	Mov BX, offset string_espacio
	Call Pintar
	
	Add DI, 160 ; cambio de línea
	Mov AH, FRojo + LBlanca
	Mov BX, offset string_asterisco
	Call Pintar
	
	Ret
	
	; TURNO CELESTE
turno_celeste_pintar:
	Mov AH, FCeleste + LBlanca
	Mov BX, offset string_asterisco
	Call Pintar
	
	Add DI, 160 ; cambio de línea
	Xor AH, AH ; Para que pinte en negro
	Mov BX, offset string_espacio
	Call Pintar
	
	Ret
pintar_turno EndP

; Si hay ganador o empate, la variable ganador se modifica a 1 o 2, dependiendo del jugador. 1 celeste, 2 roja y 3 empate.
determinar_ganador Proc Near
	; Llamados de las direcciones, en  busca del patrón ganador.
	
	Call derecha_izquierda
	Cmp ganador, 0
	JNE salir_determinar_ganador
	
	Call arriba_abajo
	Cmp ganador, 0
	JNE salir_determinar_ganador
	
	Call diagonal_creciente
	Cmp ganador, 0
	JNE salir_determinar_ganador
	
	Call diagonal_decreciente
	Cmp ganador, 0
	JNE salir_determinar_ganador
	
	Call empate
	
salir_determinar_ganador:
	Ret
determinar_ganador EndP

limpiar_paginas Proc Near

; Páginas:
; 0: no se usa.
; 1: captar dimensiones.
; 2: ayuda.
; 3: tablero, principal.
; 4: salir.
; 5: solicitar nombres.

	Mov ECX, 2
	Mov DI, Pagina
	Call limpiar_pag
	
	Mov ECX, 25
	Mov DI, Pagina*2
	Call limpiar_pag
	
	
	Mov ECX, 24
	Mov DI, Pagina*3
	Call limpiar_pag
	
	Mov ECX, 1
	Mov DI, Pagina*4
	Call limpiar_pag
	
	Mov ECX, 4
	Mov DI, Pagina*5
	Call limpiar_pag

	
	Ret
limpiar_paginas EndP

Codigo EndS
End main


****Detalles de la implementación****

Leer teclas especiales: cuando se leen teclas especiales, tengo que leerla a la segunda vez para encontrar el verdadero:
la primera vez, lo que se guarda en AL es un 00, la segunda vez que se "invoca" el DOS (para leer en consola) ya hay un valor definido sin que 
el usuario digite, ese valor es el que hay que captar para reconocer cuál tecla especial es.
En resumen, cuando AL es 00, hay tecla especial, vuelva a llamar a DOS.

Información al usuario: todo mediante texto, se va "strings" el cual, informan al usuario de aspectos importantes del juego: solicitar
dimensiones, ayuda, el tablero, etc.  Por lo que se necesitan bastantes "rótulos" para mandar a pintar en pantalla.

Pintar en consola: para llamar a pintar_caracter, se tiene que tener los siguientes datos: el color de la hilera (en AH), tanto el fondo como 
la letra; 
la hilera (en BH) con un fin de caracter ($) y el DI con el offset de la memoria de video (donde quiero que inicie la hilera):
Note que siempre en AX hay que tener guardado SegVideo (técnicamente en AL, ya que en AH se define el color).  Otra cosa importante de 
recalcar es que DI
define dónde empieza a pintarse, por lo que si pongo 0, inicia en la primera línea de la consola, si pongo 160, será en el inicio de la 
segunda línea, y así sucesivamente.  En resumen, multiplico 160 * x, y escribiré en el inicio de x línea.

Pintar el tablero:
(actualizdo después de escribir los detalles) Necesito un tablero pequeño, ya que solo tengo 25 filas, por lo que queda casi justo.  Por lo 
que se va a
pintar de manera simple, una simple matriz mxn sin limite y nada, lo único que se va a diferenciar son los colores.  Para tener un buen orden, 
el tablero se va a pintar en el centro de la consola.

Moverme entre páginas: el que depende es AL, por lo que si quiero estar en la página 0, pongo un 0 en AL y llamo a la DOS. Si quiero estar en 
la 1, pongo 1 en AL y llamo DOS, y así sucesivamente.

Suponga que EBX contiene el Offset de una variable con DW:
Así se accede a cada valor de la posición, de dos en dos por ser DW (2 bytes).  Se tiene que guardar en un registro de 2 bytes.
Si fuera un DB, se mueve de 1 en 1 y se guarda en un registro de 1 byte.
Mov CX, [EBX]
Mov CX, [EBX + 2]
Mov CX, [EBX + 4]


Para cambiar una variable con DW
Mov Variable + 2, 10  ; Cambia la segunda posición con un 10, de 2 en 2 porque es un DW (2 bytes)

Ejemplo para poner una A en un string en la posición 1:

prueba DB "123456"
; prueba = "123456
Mov AL, "A" ; A=41h
Mov prueba + 1, AL
; prueba = "1A3456

Note que es lo mismo que manejarlo con el Offset en un registro:
Mov ECX, Offset prueba
Mov AL, "A" ; A=41h
Mov [ECX] + 1, AL

Para limpiar páginas de la pantalla.
Hay un proceso "universal" que supone que por parámetro se pasa el inicio de una línea, y este proceso limpia la línea, es decir, recorre
desde el inicio hasta el final de la línea (160).  Por lo que al limpiar una página, lo que se hace es hacer n llamados al proceso de limpiar
líneas.  Donde n es la cantidad de líneas quiero limpiar en mi pantalla, pueden ser dos, pueden ser las 25 (esto para hacer el proceso un
poco más eficiente).  Se limpia pintando cada caracter con un espacio y con fondo negro (string_espacio y Xor).

Para determinar ganador:
Desde posicion_actual_cursor, recorro en todas las direcciones, para hacerlo un poco más eficiente, se toma el caso en que la ficha que se 
puso puede estar en el centro del patrón, por lo que se recorre en una dirección y en la opuesta contado las fichas, se busca 4 fichas más 
(porque ya sé que existe una ficha, la de posicion_actual_cursor).  Se hace el mismo procedimiento para todas las direcciones posibles.
Si se encuentra un ganador, la variable se le asigna un valor de 1 o 2, dependiendo de si es celeste o rojo respectivamente.

Para determinar empate: pese a que no es la solución más eficiente, sí es la más sencilla, recorrer todo Matriz hasta el final, si se encuentra
algún 0 en el camino, se detiene el ciclo ya que al menos hay una casilla vacía y cabe la posibilidad de que se pueda ganar 
(no es 100% probable).  Si se llega hasta el final, hay empate y se cambia la variable ganador por un 3.