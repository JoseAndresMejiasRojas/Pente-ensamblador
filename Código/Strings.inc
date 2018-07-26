; Las variables tienen espacios y un $ al final, para demarcar el fin de string.
string_jugador_rojo 	DB "       $"
string_jugador_celeste 	DB "       $"

string_filas 		DB "Digite la cantidad de filas en dos digitos: ", "$"
string_columnas		DB "Digite la cantidad de columnas en dos digitos", "$"
string_salir		DB "Seguro que quiere salir? Presione Esc para confirmar.", "$"
string_reiniciar	DB "Seguro que quiere reiniciar? Presione Enter para confirmar.", "$"
string_celeste		DB "Nombre del jugador celeste, maximo 6 caracteres, luego presione Enter.", "$"
string_rojo			DB "Nombre del jugador rojo, maximo 6 caracteres, luego presione Enter.", "$"
string_asterisco	DB "*", "$"
string_espacio		DB " ", "$"

; Texto que se va a escribir en el menú de ayuda:
string_ayuda1 	DB "Ayuda para el juego Pente: ", "$"
string_ayuda2 	DB "*Al iniciar el juego, se le solicita las dimensiones del tablero, con un minimo", "$"
string_ayuda3	DB "de 5x5 y un maximo de 19x19, primero se le solicita la cantidad de filas,", "$"
string_ayuda4	DB "luego, la cantidad de columnas. Tiene que digitarlas en numeros (5, 10...).", "$"
string_ayuda5	DB "*Presione las teclas de las flechas para mover el cursor en el tablero.", "$"
string_ayuda6	DB "*Presione la tecla espacio para colocar la ficha en la posicion del cursor.", "$"
string_ayuda7	DB "*Digite F10 para reiniciar el juego, se solicitaran nuevamente las dimensiones.", "$"
string_ayuda8	DB "Reglas del juego", "$"
string_ayuda9	DB "*Hay dos jugadores: fichas celestes y rojas. Note que el fondo es verde", "$"
string_ayuda10	DB "*Al colocar una ficha, no se puede a poner otra ficha en la misma posicion.", "$"
string_ayuda11	DB "*Gana el primer jugador que forme una linea recta del mismo color.", "$"
string_ayuda12	DB "*En caso de que se llene el tablero, reinice el juego.", "$"
string_ayuda13	DB "Presione cualquier tecla para salir de la ayuda.", "$"
string_ayuda14	DB "*** Hecho por Jose Andres Mejias Rojas :D ***", "$"

string_resultado			DB "Partida finalizada", "$"
string_resultado_ganador	DB "El ganador es ", "$"
string_resultado_empate 	DB "Empate!", "$"
string_resultado2			DB "Digite Esc para salir, F10 para reiniciar partida.", "$"

string_error_dimensiones	DB "Error D:", "$"

string_error_nombres			DB	" no permitido.", "$"