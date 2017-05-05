#ifndef USER_COMMANDS_H
#define USER_COMMANDS_H

#include "../includes/G-2301-04-P2-messages.h"


/**
 * Accion por defecto para los mensajes
 * @param m_in mensaje que se va a parsear
 * @return 0 si OK
 * @return -1 ERROR
 */
int userCommandDefault(char *m_in, SSL *ssl);

/**
 * Parsea el comando de usuario NAMES crea el mensaje para el servidor y lo envia
 * @param m_in mensaje que se va a parsear
 * @return 0 si OK
 * @return -1 ERROR
 */
int userCommandNames(char *m_in, SSL *ssl);

/**
 * Parsea el comando de usuario LIST crea el mensaje para el servidor y lo envia
 * @param m_in mensaje que se va a parsear
 * @return 0 si OK
 * @return -1 ERROR
 */
int userCommandList(char *command, SSL *ssl);

/**
 * Parsea el comando de usuario JOIN crea el mensaje para el servidor y lo envia
 * @param m_in mensaje que se va a parsear
 * @return 0 si OK
 * @return -1 ERROR
 */
int userCommandJoin(char *command, SSL *ssl);

/**
 * Parsea el comando de usuario nick crea el mensaje para el servidor y lo envia
 * @param m_in mensaje que se va a parsear
 * @return 0 si OK
 * @return -1 ERROR
 */
int userCommandNick(char *command, SSL *ssl);


/**
 * Parsea el comando de usuario PRIVMSG crea el mensaje para el servidor y lo envia
 * @param m_in mensaje que se va a parsear
 * @return 0 si OK
 * @return -1 ERROR
 */
/*NUM 10*/
int userCommandWhois(char *command, SSL *ssl);


/**
 * Parsea el comando de usuario PRIVMSG crea el mensaje para el servidor y lo envia
 * @param m_in mensaje que se va a parsear
 * @return 0 si OK
 * @return -1 ERROR
 */
/*NUM 15*/
int userCommandPrivmsg(char *command, SSL *ssl);


/**
 * Parsea el comando de usuario MODE crea el mensaje para el servidor y lo envia
 * @param command comando a parsear
 * @return 0 si OK
 * @return -1 ERROR
 */
int userCommandMode(char *command, SSL *ssl);



/**
 * Parsea el comando de usuario KICK crea el mensaje para el servidor y lo envia
 * @param m_in mensaje que se va a parsear
 * @return 0 si OK
 * @return -1 ERROR
 */
int userCommandKick(char *command, SSL *ssl);

/**
 * Parsea el comando de usuario PART crea el mensaje para el servidor y lo envia
 * @param m_in mensaje que se va a parsear
 * @return 0 si OK
 * @return -1 ERROR
 */
int userCommandPart(char *command, SSL *ssl);

/**
 * Parsea el comando de usuario QUIT crea el mensaje para el servidor y lo envia
 * @param m_in mensaje que se va a parsear
 * @return 0 si OK
 * @return -1 ERROR
 */
int userCommandQuit(char *command, SSL *ssl);

/**
 * Parsea el comando de usuario AWAY crea el mensaje para el servidor y lo envia
 * @param m_in mensaje que se va a parsear
 * @return 0 si OK
 * @return -1 ERROR
 */
int userCommandAway(char *command, SSL *ssl);

/**
 * Parsea el comando de usuario BACK
 * @param m_in mensaje que se va a parsear
 * @return 0 si OK
 * @return -1 ERROR
 */
int userCommandBack(char *command, SSL *ssl);

/**
 * Parsea el comando de usuario TOPIC
 * @param m_in mensaje que se va a parsear
 * @return 0 si OK
 * @return -1 ERROR
 */
int userCommandTopic(char *command, SSL *ssl);



typedef int (*pUCommand)(char *m_in, SSL *ssl);
pUCommand UserCommands[64];

#endif /*USER_COMMANDS_H*/
