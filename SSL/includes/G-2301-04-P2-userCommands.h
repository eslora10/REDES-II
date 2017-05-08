#ifndef USER_COMMANDS_H
#define USER_COMMANDS_H

/**
 * Modulo de comandos de usuario. Contiene funciones de alto nivel para la gestion
 * de comandos que ejecuta el cliente.
 * @author Antonio Amor Mourelle &lt;antonio.amor@estudiante.uam.es&gt;
 * @author Esther Lopez Ramos &lt;esther.lopezramos@estudiante.uam.es&gt;
 * @author Mario Santiago Yepes &lt;mario.santiagoy@estudiante.uam.es&gt;
 * @file G-2301-04-P2-userCommands.h
 */

#include "../includes/G-2301-04-P2-messages.h"


/**
 * Accion por defecto para los mensajes
 * @param command commando de usuario que se va a enviar
 * @param ssl estructura de ssl. En caso de no usar la capa segura debe valer NULL
 * @return 0 si OK
 * @return -1 ERROR
 */
int userCommandDefault(char *command, SSL *ssl);

/**
 * Parsea el comando de usuario NAMES crea el mensaje para el servidor y lo envia
 * @param command commando de usuario que se va a enviar
 * @param ssl estructura de ssl. En caso de no usar la capa segura debe valer NULL
 * @return 0 si OK
 * @return -1 ERROR
 */
int userCommandNames(char *command, SSL *ssl);

/**
 * Parsea el comando de usuario LIST crea el mensaje para el servidor y lo envia
 * @param command commando de usuario que se va a enviar
 * @param ssl estructura de ssl. En caso de no usar la capa segura debe valer NULL
 * @return 0 si OK
 * @return -1 ERROR
 */
int userCommandList(char *command, SSL *ssl);

/**
 * Parsea el comando de usuario JOIN crea el mensaje para el servidor y lo envia
 * @param command commando de usuario que se va a enviar
 * @param ssl estructura de ssl. En caso de no usar la capa segura debe valer NULL
 * @return 0 si OK
 * @return -1 ERROR
 */
int userCommandJoin(char *command, SSL *ssl);

/**
 * Parsea el comando de usuario nick crea el mensaje para el servidor y lo envia
 * @param command commando de usuario que se va a enviar
 * @param ssl estructura de ssl. En caso de no usar la capa segura debe valer NULL
 * @return 0 si OK
 * @return -1 ERROR
 */
int userCommandNick(char *command, SSL *ssl);


/**
 * Parsea el comando de usuario PRIVMSG crea el mensaje para el servidor y lo envia
 * @param command commando de usuario que se va a enviar
 * @param ssl estructura de ssl. En caso de no usar la capa segura debe valer NULL
 * @return 0 si OK
 * @return -1 ERROR
 */
/*NUM 10*/
int userCommandWhois(char *command, SSL *ssl);


/**
 * Parsea el comando de usuario PRIVMSG crea el mensaje para el servidor y lo envia
 * @param command commando de usuario que se va a enviar
 * @param ssl estructura de ssl. En caso de no usar la capa segura debe valer NULL
 * @return 0 si OK
 * @return -1 ERROR
 */
/*NUM 15*/
int userCommandPrivmsg(char *command, SSL *ssl);


/**
 * Parsea el comando de usuario MODE crea el mensaje para el servidor y lo envia
 * @param command commando de usuario que se va a enviar
 * @param ssl estructura de ssl. En caso de no usar la capa segura debe valer NULL
 * @return 0 si OK
 * @return -1 ERROR
 */
int userCommandMode(char *command, SSL *ssl);



/**
 * Parsea el comando de usuario KICK crea el mensaje para el servidor y lo envia
 * @param command commando de usuario que se va a enviar
 * @param ssl estructura de ssl. En caso de no usar la capa segura debe valer NULL
 * @return 0 si OK
 * @return -1 ERROR
 */
int userCommandKick(char *command, SSL *ssl);

/**
 * Parsea el comando de usuario PART crea el mensaje para el servidor y lo envia
 * @param command commando de usuario que se va a enviar
 * @param ssl estructura de ssl. En caso de no usar la capa segura debe valer NULL
 * @return 0 si OK
 * @return -1 ERROR
 */
int userCommandPart(char *command, SSL *ssl);

/**
 * Parsea el comando de usuario QUIT crea el mensaje para el servidor y lo envia
 * @param command commando de usuario que se va a enviar
 * @param ssl estructura de ssl. En caso de no usar la capa segura debe valer NULL
 * @return 0 si OK
 * @return -1 ERROR
 */
int userCommandQuit(char *command, SSL *ssl);

/**
 * Parsea el comando de usuario AWAY crea el mensaje para el servidor y lo envia
 * @param command commando de usuario que se va a enviar
 * @param ssl estructura de ssl. En caso de no usar la capa segura debe valer NULL
 * @return 0 si OK
 * @return -1 ERROR
 */
int userCommandAway(char *command, SSL *ssl);

/**
 * Parsea el comando de usuario BACK
 * @param command commando de usuario que se va a enviar
 * @param ssl estructura de ssl. En caso de no usar la capa segura debe valer NULL
 * @return 0 si OK
 * @return -1 ERROR
 */
int userCommandBack(char *command, SSL *ssl);

/**
 * Parsea el comando de usuario TOPIC
 * @param command commando de usuario que se va a enviar
 * @param ssl estructura de ssl. En caso de no usar la capa segura debe valer NULL
 * @return 0 si OK
 * @return -1 ERROR
 */
int userCommandTopic(char *command, SSL *ssl);

/**
 * @typedef pUCommand
 * @brief Definimos el tipo funcion de comando de usuario
 */
typedef int (*pUCommand)(char *m_in, SSL *ssl);

/**
 * @var UserCommands
 * @brief Array de punteros a las funciones de comandos de usuario
 */
pUCommand UserCommands[64];

#endif /*USER_COMMANDS_H*/
