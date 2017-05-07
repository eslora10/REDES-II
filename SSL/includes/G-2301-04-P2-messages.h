#ifndef MESSAGES_H
#define MESSAGES_H

/**
 * Modulo de mensajes. Contiene funciones de alto nivel para la gestion
 * de mensajes que llegan al servidor.
 * @author Antonio Amor Mourelle &lt;antonio.amor@estudiante.uam.es&gt;
 * @author Esther Lopez Ramos &lt;esther.lopezramos@estudiante.uam.es&gt;
 * @author Mario Santiago Yepes &lt;mario.santiagoy@estudiante.uam.es&gt;
 * @file G-2301-04-P2-messages.h
 */

#include <redes2/irc.h>
#include <redes2/ircxchat.h>
#include "G-2301-04-P1-socket.h"

#define MAX_TCP 65535


SSL* ssl_channel;

int sck; /*Socket en el que se conecta el cliente*/
char hostName[MAXLEN]; /*Nombre de host que nos asigna el servidor al entrar en el.
                 * Usado para el envío de ficheros*/
int stopAudio;

/**
 * Accion por defecto para los mensajes
 * @param m_in mensaje que se va a parsear
 * @return 0 si OK
 * @return -1 ERROR
 */
int msgDefault(char *m_in);


/**
 * Parsea el mensaje de NICK
 * @param m_in mensaje que se va a parsear
 * @return 0 si OK
 * @return -1 ERROR
 */
int msgNickChanged(char *m_in);


/**
 * Parsea el mensaje de JOIN
 * @param m_in mensaje que se va a parsear
 * @return 0 si OK
 * @return -1 ERROR
 */
int msgJoin(char *m_in);

/**
 * Parsea el mensaje de MODE
 * @param m_in mensaje que se va a parsear
 * @return 0 si OK
 * @return -1 ERROR
 */
int msgMode(char *m_in);

/**
 * Parsea el mensaje de NAMES
 * @param m_in mensaje que se va a parsear
 * @return 0 si OK
 * @return -1 ERROR
 */
int msgNames(char *m_in);

/**
 * Parsea el mensaje de WHO
 * @param m_in mensaje que se va a parsear
 * @return 0 si OK
 * @return -1 ERROR
 */
int msgWho(char *m_in);

/**
 * Parsea el mensaje de NOTICE
 * @param m_in mensaje que se va a parsear
 * @return 0 si OK
 * @return -1 ERROR
 */
int msgNotice(char *m_in);

/**
 * Parsea el mensaje de WELCOME
 * @param m_in mensaje que se va a parsear
 * @return 0 si OK
 * @return -1 ERROR
 */
int msgWelcome(char *m_in);

/**
 * Parsea el mensaje de YOURHOST
 * @param m_in mensaje que se va a parsear
 * @return 0 si OK
 * @return -1 ERROR
 */
int msgYourHost(char *m_in);

/**
 * Parsea el mensaje de CREATED
 * @param m_in mensaje que se va a parsear
 * @return 0 si OK
 * @return -1 ERROR
 */
int msgCreated(char *m_in);

/**
 * Parsea el mensaje de MYINFO
 * @param m_in mensaje que se va a parsear
 * @return 0 si OK
 * @return -1 ERROR
 */
int msgMyInfo(char *m_in);

/**
 * Parsea el mensaje de ISSUPPORT
 * @param m_in mensaje que se va a parsear
 * @return 0 si OK
 * @return -1 ERROR
 */
int msgIsSupport(char *m_in);

/**
 * Parsea el mensaje de MOTD
 * @param m_in mensaje que se va a parsear
 * @return 0 si OK
 * @return -1 ERROR
 */
int msgMOTDStart(char *m_in);

/**
 * Parsea el mensaje de MOTD
 * @param m_in mensaje que se va a parsear
 * @return 0 si OK
 * @return -1 ERROR
 */
int msgMOTD(char *m_in);

/**
 * Parsea el mensaje de MOTD
 * @param m_in mensaje que se va a parsear
 * @return 0 si OK
 * @return -1 ERROR
 */
int msgMOTDEnd(char *m_in);

/**
 * Parsea el mensaje de LIST
 * @param m_in mensaje que se va a parsear
 * @return 0 si OK
 * @return -1 ERROR
 */
int msgList(char *m_in);

/**
 * Parsea el mensaje de ENDOFNAMES
 * @param m_in mensaje que se va a parsear
 * @return 0 si OK
 * @return -1 ERROR
 */
int msgEndNames(char *m_in);

/**
 * Parsea el mensaje de You're not channel operator
 * @param m_in mensaje que se va a parsear
 * @return 0 si OK
 * @return -1 ERROR
 */
int msgNotOperator(char *m_in);

/**
 * Parsea el mensaje de que se recibe se cambia el modo de un canal
 * @param m_in mensaje que se va a parsear
 * @return 0 si OK
 * @return -1 ERROR
 */
int msgModeChanged(char *m_in);

/**
 * Parsea el mensaje de WHOISUSER
 * @param m_in mensaje que se va a parsear
 * @return 0 si OK
 * @return -1 ERROR
 */
int msgWhoisUser(char *m_in);

/**
 * Parsea el mensaje de WHOISSERVER
 * @param m_in mensaje que se va a parsear
 * @return 0 si OK
 * @return -1 ERROR
 */
int msgWhoisServer(char *m_in);

/**
 * Parsea el mensaje de WHOISCHANNELS
 * @param m_in mensaje que se va a parsear
 * @return 0 si OK
 * @return -1 ERROR
 */
int msgWhoisChannels(char *m_in);

/**
 * Parsea el mensaje de WHOISIDLE
 * @param m_in mensaje que se va a parsear
 * @return 0 si OK
 * @return -1 ERROR
 */
int msgWhoisMode(char *m_in);




/**
 * Parsea el mensaje de WHOISIDLE
 * @param m_in mensaje que se va a parsear
 * @return 0 si OK
 * @return -1 ERROR
 */
int msgWhoisIdle(char *m_in);



/**
 * Parsea el mensaje de WHOISEND
 * @param m_in mensaje que se va a parsear
 * @return 0 si OK
 * @return -1 ERROR
 */
int msgWhoisEnd(char *m_in);

/**
 * Parsea el mensaje de PRIVMSG
 * @param m_in mensaje que se va a parsear
 * @return 0 si OK
 * @return -1 ERROR
 */
int msgPrivmsg(char *m_in);

/**
 * Parsea el mensaje de Ping enviado por el servidor Y ENVIA EL PONG
 * @param m_in mensaje que se va a parsear
 * @return 0 si OK
 * @return -1 ERROR
 */
int msgPing(char *m_in);

/**
 * Parsea el mensaje de Ping enviado por el servidor Y ENVIA EL PONG
 * @param m_in mensaje que se va a parsear
 * @return 0 si OK
 * @return -1 ERROR
 */
int msgTopicChanged(char *m_in);

/**
 * Parsea el mensaje de NO NICK OR CHANNEL
 * @param m_in mensaje que se va a parsear
 * @return 0 si OK
 * @return -1 ERROR
 */
int msgNoNickOrChannel(char *m_in);

/**
 * Parsea el mensaje de Part
 * @param m_in mensaje que se va a parsear
 * @return 0 si OK
 * @return -1 ERROR
 */
int msgPart(char *m_in);

/**
 * Parsea el mensaje de Quit
 * @param m_in mensaje que se va a parsear
 * @return 0 si OK
 * @return -1 ERROR
 */
int msgQuit(char *m_in);

/**
 * Parsea el mensaje de Kick
 * @param m_in mensaje que se va a parsear
 * @return 0 si OK
 * @return -1 ERROR
 */
int msgKick(char *m_in);


/**
 * Parsea el mensaje de Away
 * @param m_in mensaje que se va a parsear
 * @return 0 si OK
 * @return -1 ERROR
 */
int msgAway(char *m_in);

/**
 * Parsea el mensaje de Back (UNAWAY)
 * @param m_in mensaje que se va a parsear
 * @return 0 si OK
 * @return -1 ERROR
 */
int msgBack(char *m_in);

/**
 * Parsea el mensaje de Away despues de mandar un privmsg
 * @param m_in mensaje que se va a parsear
 * @return 0 si OK
 * @return -1 ERROR
 */
int msgrplAway(char *m_in);

/**
 * @typedef pFuncs
 * @brief Definimos el tipo funcion comando
 */
typedef int (*pMsg)(char *m_in);

/**
 * @var Commands
 * @brief Array de punteros a las funciones de comandos
 */
pMsg Messages[2048];

#endif /*MESSAGES_H*/
