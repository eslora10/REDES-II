#ifndef COMMAND_H
#define COMMAND_H

/**
 * @def _GNU_SOURCE
 * @brief Define necesario para evitar implicit declaration en asprintf
 */
#define _GNU_SOURCE

/**
* Modulo de comandos. Implementa todos los comandos IRC requeridos por
* el corrector
* @author Antonio Amor Mourelle &lt;antonio.amor@estudiante.uam.es&gt;
* @author Esther Lopez Ramos &lt;esther.lopezramos@estudiante.uam.es&gt;
* @author Mario Santiago Yepes &lt;mario.santiagoy@estudiante.uam.es&gt;
* @file G-2301-04-P1-command.h
*/

#include <stdio.h>
#include <string.h>
#include <redes2/irc.h>

/**
 * @def MY_ADDR
 * @brief Direccion IP del servidor
 */
#define MY_ADDR "127.0.0.1"

/**
 * @def END_CONNECTION
 * @brief Codigo para indicar al hilo que el usuario va a cerrar la conexion
 */
#define END_CONNECTION 27
/**
 * @def MAX_NICK
 * @brief LTamanno maximo que puede tener el nick del usuario segun
 * el estandar IRC
 */
#define MAX_NICK 9

/**
 * @brief Envia PING a todos los usuarios, parte del protocolo ping-pong
 * @return 0 si todo ha ido bien, -1 en caso de error
 */
int ping();

/**
 * @brief Ejecuta el comando PONG
 * @param command comando que se va a parsear y ejecutar
 * @param nick nickname del usuario que ejecuta el comando
 * @param sck socket en el que se recibio el comando
 * @param ssl estructura para la conexion segura de un determinado cliente
 * Si no se está usando ssl debe valer NULL
 * @return -1 en caso de fallo, 0 OK
 */
int pongCommand(char* command, char* nick, int sck, SSL *ssl);

/**
 * @brief Funcion que comprueba que los usuarios han realizado el pong para el
 * protocolo pingpong. En caso de que no, se les cierra el socket
 * @return 0 si todo ha ido bien, -1 en caso de error
 */
int checkConnection();

/**
 * @brief Funcion que ejecuta el hilo de pingpong
 * @return 0 si todo ha ido bien, -1 en caso de error
 */
void* pingpong();

/**
 * @brief Dado un socket acepta la peticion de un cliente
 * @param sck_ssl descriptor del socket
 */
void * attendClientSocket(void *sck_ssl);

/**
 * @brief Funcion de parseo de errores IRC. Coge el codigo de error y envia su
 * mensaje correspondiente
 * @param errval Codigo de error IRC
 * @param sck descriptor de fichero del socket en el que se envia el error
 * @param ssl estructura para la conexion segura de un determinado cliente
 * Si no se está usando ssl debe valer NULL
 * @param nick nickname del usuario
 * @param param parametros adicionales para el mensaje de error
 * @return 0 OK, -1 ERR
*/
int sendErrMsg(long errval, int sck, SSL *ssl, char *nick, char *param);

/**
 * @brief Esta funcion se utiliza cuando un comando es desconocido por el
 * servidor para notificar al cliente
 * @param command comando que se va a parsear y ejecutar
 * @param nick nickname del usuario que ejecuta el comando
 * @param sck socket en el que se recibio el comando
 * @param ssl estructura para la conexion segura de un determinado cliente
 * Si no se está usando ssl debe valer NULL
 * @return -1 en caso de fallo, 0 OK
 */
int FuncDefault(char* command, char* nick, int sck, SSL *ssl);

/**
 * @brief Ejecuta el comando USER
 * @param command comando que se va a parsear y ejecutar
 * @param nick nickname del usuario que ejecuta el comando
 * @param sck socket en el que se recibio el comando
 * @param ssl estructura para la conexion segura de un determinado cliente
 * Si no se está usando ssl debe valer NULL
 * @return -1 en caso de fallo, 0 OK
 */
int userCommand(char* command, char* nick, int sck, SSL *ssl);

/**
 * @brief Ejecuta el comando NICK
 * @param command comando que se va a parsear y ejecutar
 * @param nick nickname del usuario que ejecuta el comando
 * @param sck socket en el que se recibio el comando
 * @param ssl estructura para la conexion segura de un determinado cliente
 * Si no se está usando ssl debe valer NULL
 * @return -1 en caso de fallo, 0 OK
 */
int nickCommand(char* command, char* nick, int sck, SSL *ssl);

/**
 * @brief Ejecuta el comando MODE
 * @param command comando que se va a parsear y ejecutar
 * @param nick nickname del usuario que ejecuta el comando
 * @param sck socket en el que se recibio el comando
 * @param ssl estructura para la conexion segura de un determinado cliente
 * Si no se está usando ssl debe valer NULL
 * @return -1 en caso de fallo, 0 OK
 */
int modeCommand(char* command, char* nick, int sck, SSL *ssl);

/**
 * @brief Ejecuta el comando QUIT
 * @param command comando que se va a parsear y ejecutar
 * @param nick nickname del usuario que ejecuta el comando
 * @param sck socket en el que se recibio el comando
 * @param ssl estructura para la conexion segura de un determinado cliente
 * Si no se está usando ssl debe valer NULL
 * @return -1 en caso de fallo, END_CONNECTION si todo
 * el usuario es eliminado correctamente
 */
int quitCommand(char* command, char* nick, int sck, SSL *ssl);

/**
 * @brief Ejecuta el comando JOIN
 * @param command comando que se va a parsear y ejecutar
 * @param nick nickname del usuario que ejecuta el comando
 * @param sck socket en el que se recibio el comando
 * @param ssl estructura para la conexion segura de un determinado cliente
 * Si no se está usando ssl debe valer NULL
 * @return -1 en caso de fallo, 0 OK
 */
int joinCommand(char* command, char* nick, int sck, SSL *ssl);

/**
 * @brief Ejecuta el comando PART
 * @param command comando que se va a parsear y ejecutar
 * @param nick nickname del usuario que ejecuta el comando
 * @param sck socket en el que se recibio el comando
 * @param ssl estructura para la conexion segura de un determinado cliente
 * Si no se está usando ssl debe valer NULL
 * @return -1 en caso de fallo, 0 OK
 */
int partCommand(char* command, char* nick, int sck, SSL *ssl);

/**
 * @brief Ejecuta el comando TOPIC
 * @param command comando que se va a parsear y ejecutar
 * @param nick nickname del usuario que ejecuta el comando
 * @param sck socket en el que se recibio el comando
 * @param ssl estructura para la conexion segura de un determinado cliente
 * Si no se está usando ssl debe valer NULL
 * @return -1 en caso de fallo, 0 OK
 */
int topicCommand(char* command, char* nick, int sck, SSL *ssl);


/**
 * @brief Ejecuta el comando NAMES
 * @param command comando que se va a parsear y ejecutar
 * @param nick nickname del usuario que ejecuta el comando
 * @param sck socket en el que se recibio el comando
 * @param ssl estructura para la conexion segura de un determinado cliente
 * Si no se está usando ssl debe valer NULL
 * @return -1 en caso de fallo, 0 OK
 */
int namesCommand(char* command, char* nick, int sck, SSL *ssl);

/**
 * @brief Ejecuta el comando LIST
 * @param command comando que se va a parsear y ejecutar
 * @param nick nickname del usuario que ejecuta el comando
 * @param sck socket en el que se recibio el comando
 * @param ssl estructura para la conexion segura de un determinado cliente
 * Si no se está usando ssl debe valer NULL
 * @return -1 en caso de fallo, 0 OK
 */
int listCommand(char* command, char* nick, int sck, SSL *ssl);

/**
 * @brief Ejecuta el comando KICK
 * @param command comando que se va a parsear y ejecutar
 * @param nick nickname del usuario que ejecuta el comando
 * @param sck socket en el que se recibio el comando
 * @param ssl estructura para la conexion segura de un determinado cliente
 * Si no se está usando ssl debe valer NULL
 * @return -1 en caso de fallo, 0 OK
 */
int kickCommand(char* command, char* nick, int sck, SSL *ssl);


/**
 * @brief Ejecuta el comando PRIVMSG
 * @param command comando que se va a parsear y ejecutar
 * @param nick nickname del usuario que ejecuta el comando
 * @param sck socket en el que se recibio el comando
 * @param ssl estructura para la conexion segura de un determinado cliente
 * Si no se está usando ssl debe valer NULL
 * @return -1 en caso de fallo, 0 OK
 */
int privmsgCommand(char* command, char* nick, int sck, SSL *ssl);

/**
 * @brief Ejecuta el comando MOTD
 * @param command comando que se va a parsear y ejecutar
 * @param nick nickname del usuario que ejecuta el comando
 * @param sck socket en el que se recibio el comando
 * @param ssl estructura para la conexion segura de un determinado cliente
 * Si no se está usando ssl debe valer NULL
 * @return -1 en caso de fallo, 0 OK
 */
int motdCommand(char* command, char* nick, int sck, SSL *ssl);

/**
 * @brief Ejecuta el comando WHO
 * @param command comando que se va a parsear y ejecutar
 * @param nick nickname del usuario que ejecuta el comando
 * @param sck socket en el que se recibio el comando
 * @param ssl estructura para la conexion segura de un determinado cliente
 * Si no se está usando ssl debe valer NULL
 * @return -1 en caso de fallo, 0 OK
 */
int whoCommand(char* command, char* nick, int sck, SSL *ssl);

/**
 * @brief Ejecuta el comando WHOIS
 * @param command comando que se va a parsear y ejecutar
 * @param nick nickname del usuario que ejecuta el comando
 * @param sck socket en el que se recibio el comando
 * @param ssl estructura para la conexion segura de un determinado cliente
 * Si no se está usando ssl debe valer NULL
 * @return -1 en caso de fallo, 0 OK
 */
int whoisCommand(char* command, char* nick, int sck, SSL *ssl);

/**
 * @brief Ejecuta el comando PING
 * @param command comando que se va a parsear y ejecutar
 * @param nick nickname del usuario que ejecuta el comando
 * @param sck socket en el que se recibio el comando
 * @param ssl estructura para la conexion segura de un determinado cliente
 * Si no se está usando ssl debe valer NULL
 * @return -1 en caso de fallo, 0 OK
 */
int pingCommand(char* command, char* nick, int sck, SSL *ssl);

/**
 * @brief Ejecuta el comando AWAY
 * @param command comando que se va a parsear y ejecutar
 * @param nick nickname del usuario que ejecuta el comando
 * @param sck socket en el que se recibio el comando
 * @param ssl estructura para la conexion segura de un determinado cliente
 * Si no se está usando ssl debe valer NULL
 * @return -1 en caso de fallo, 0 OK
 */
int awayCommand(char* command, char* nick, int sck, SSL *ssl);

/**
 * @typedef pFuncs
 * @brief Definimos el tipo funcion comando
 */
typedef int (*pFuncs)(char *command, char* nick, int sck, SSL *ssl);

/**
 * @typedef Sck_SSL
 * @brief Estructura argumento de la funcion attendClient
 */
/**
 * @struct _
 * estructura para guardar los argumentos de attendClient
 */
/** 
 * @var _::sck
 * socket en caso del cliente. Parametro obligtorio
 */
/** 
 * @var _::ssl
 * Estructura ssl necesaria si se ha iniciado una conexion segura
 */
typedef struct _{
    int sck;
    SSL* ssl;

} Sck_SSL;

/**
 * @var Commands
 * @brief Array de punteros a las funciones de comandos
 */
pFuncs Commands[38];


#endif /*COMMAND_H*/
