/**
 * Modulo de audio. Contiene funciones de alto nivel para el envio de audio
 * desde la interfaz de usuario.
 * @author Antonio Amor Mourelle &lt;antonio.amor@estudiante.uam.es&gt;
 * @author Esther Lopez Ramos &lt;esther.lopezramos@estudiante.uam.es&gt;
 * @author Mario Santiago Yepes &lt;mario.santiagoy@estudiante.uam.es&gt;
 * @file G-2301-04-P2-audio.c
 */

#include "../includes/G-2301-04-P3-redes2.h"

/**
 * @brief Gestiona el envio y recibo de archivos de audio
 * @param sender nick del cliente que envia audio
 * @param msg comando que contiene la informacion necesaria para gestionar el envio
 * @return -1 en caso de error, identificador de fichero del socket del cliente
 */
int audioChat(char *sender, char *msg) {
    int port;
    char send_recv[16], ip[64], audiochat[16], *command = NULL, privmsg[512];
    pthread_t thread;
    struct audioArgs *args;

    args = (struct audioArgs*) malloc(sizeof (struct audioArgs));
    if (!args) {
        printf("Error de memoria");
        return -1;
    }

    /*/001AUDIOCHAT sender ip port*/
    /*Parseamos la cadena*/
    sscanf(msg, "%s %s %s %d", audiochat, send_recv, ip, &port);

    args->port = port;
    args->ip = ip;

    if (!strcmp(send_recv, "sender")) {
        /*El privmsg solicita la recepcion de un mensaje de voz*/
        /*Creamos un nuevo privmsg con nuestros datos para el emisor*/
        sprintf(privmsg, "\001AUDIOCHAT receiver %s %d\r\n", "192.168.0.66", port);
        if (IRCMsg_Privmsg(&command, NULL, sender, privmsg) != IRC_OK) {
            IRCInterface_WriteSystem(NULL, "Error al establecer la comunicacion con el servidor");
            return -1;
        }
        sendData(sck, NULL, TCP, NULL, 0, command, strlen(command));
        if (pthread_create(&thread, NULL, audioRecv, (void*) args) < 0) {
            IRCInterface_WriteSystemThread(NULL, "Error de memoria\n");
            return -1;
        }

        if (pthread_detach(thread) < 0) {
            IRCInterface_WriteSystemThread(NULL, "Error\n");
            return -1;
        }


    } else {
        if (pthread_create(&thread, NULL, audioSend, (void*) args) < 0) {
            IRCInterface_WriteSystemThread(NULL, "Error de memoria\n");
            return -1;
        }

        if (pthread_detach(thread) < 0) {
            IRCInterface_WriteSystemThread(NULL, "Error\n");
            return -1;
        }

    }
    return 0;

}

/**
 * @brief Gestiona el recibo de ficheros de audio
 * @param args estructura que contiene informacion sobre el recibo
 * @return -1 en caso de error, identificador de fichero del socket del cliente
 */
void *audioRecv(void *args) {
    int sckA;
    char buffer[256];

    /*Escuchamos en el puerto que nos ha indicado*/
    /*Abrimos un socket UDP*/
    sckA = openSocket(UDP);
    if (sckA < 0) {
        perror("Error socket");
        pthread_exit(NULL);
    }

    if (bindSocket(sckA, (*(struct audioArgs*) args).port, 0, UDP) < 0) {
        perror("Error bind");
        pthread_exit(NULL);
    }


    /*formato del sonido que se va a reproducir*/
    IRCSound_PlayFormat(PA_SAMPLE_S16BE, 2);

    /*Abrimos la reproduccion*/
    if (IRCSound_OpenPlay())
        pthread_exit(NULL);

    do {

        receiveData(sckA, NULL, UDP, (*(struct audioArgs*) args).ip, (*(struct audioArgs*) args).port,
	            buffer, 256);
        IRCSound_PlaySound(buffer, 160);
    } while (strcmp(buffer, "STOP"));

    close(sckA);

    pthread_exit(NULL);

}

/**
 * @brief Gestiona el envio de ficheros de audio
 * @param args estructura que contiene informacion sobre el envio
 * @return -1 en caso de error, identificador de fichero del socket del cliente
 */
void *audioSend(void *args) {
    int sckA;
    char buffer[256];

    /*Escuchamos en el puerto que nos ha indicado*/
    /*Abrimos un socket UDP*/
    sckA = openSocket(UDP);
    if (sckA < 0) {
        perror("Error socket");
        pthread_exit(NULL);
    }

    if (bindSocket(sckA, (*(struct audioArgs*) args).port, 0, UDP) < 0) {
        perror("Error bind");
        pthread_exit(NULL);
    }

    IRCSound_RecordFormat(PA_SAMPLE_S16BE, 2);

    /*Abrimos la grabacion*/
    if (IRCSound_OpenRecord())
        pthread_exit(NULL);
    do {
        IRCSound_RecordSound(buffer, 160);
        sendData(sckA, NULL, UDP, (*(struct audioArgs*) args).ip, (*(struct audioArgs*) args).port, buffer, 160);
    } while(!stopAudio);

    stopAudio = 0;
    bzero(buffer, 216);
    strcpy(buffer, "STOP");

    sendData(sckA, NULL, UDP, (*(struct audioArgs*) args).ip, (*(struct audioArgs*) args).port, buffer, strlen(buffer));

    close(sckA);

    pthread_exit(NULL);

}
