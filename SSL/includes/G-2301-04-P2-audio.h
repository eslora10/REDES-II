#ifndef AUDIO_H
#define AUDIO_H

/**
 * Modulo de audio. Contiene funciones de alto nivel para el envio de audio
 * desde la interfaz de usuario.
 * @author Antonio Amor Mourelle &lt;antonio.amor@estudiante.uam.es&gt;
 * @author Esther Lopez Ramos &lt;esther.lopezramos@estudiante.uam.es&gt;
 * @author Mario Santiago Yepes &lt;mario.santiagoy@estudiante.uam.es&gt;
 * @file G-2301-04-P2-audio.h
 */

#include "G-2301-04-P2-messages.h"

/**
 * @brief Gestiona el envio y recibo de archivos de audio
 * @param sender nick del cliente que envia audio
 * @param msg comando que contiene la informacion necesaria para gestionar el envio
 * @return -1 en caso de error, identificador de fichero del socket del cliente
 */
int audioChat(char *sender, char *msg);

/**
 * @brief Gestiona el envio de ficheros de audio
 * @param args estructura que contiene informacion sobre el envio
 * @return -1 en caso de error, identificador de fichero del socket del cliente
 */
void *audioSend(void *args);

/**
 * @brief Gestiona el recibo de ficheros de audio
 * @param args estructura que contiene informacion sobre el recibo
 * @return -1 en caso de error, identificador de fichero del socket del cliente
 */
void *audioRecv(void *args);

struct audioArgs{
    char *ip;
    int port;
};



#endif /*AUDIO_H*/
