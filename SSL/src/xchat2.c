
#include "../includes/G-2301-04-P3-redes2.h"
#include <getopt.h>

SSL* ssl_channel = NULL;


void cambioModeChannel(char* channel, char* mode, char* option) {
    char *command = NULL;


    IRCMsg_Mode(&command, NULL, channel, mode, option);

    if (sendData(sck, ssl_channel, TCP, NULL, 0, command, strlen(command)) <= 0) {
        IRCInterface_WriteSystem(NULL, "Error al conectar con el servidor");
        return;
    }
    IRCInterface_PlaneRegisterOutMessage(command);

    IRC_MFree(1, &command);
}

/**
 * Funcion manejada por los hilos que se encarga de recibir y parsear las
 * respuestas del servidor
 * @param sck descriptor de fichero del socket en el que se reciben las respuestas
 * del server
 * @return NULL en caso de error
 * @return en caso de correcto funcionamiento el hilo se cierra
 */
void* recvInfo(void* sck) {
    char buffer[MAX_TCP], *pBuffer = NULL, *command = NULL;
    int retval;
    long fun;

    while (1) {
        bzero(buffer, MAX_TCP);
	retval = recv((*(int*)sck), buffer, MAX_TCP, 0);

        if (retval <= 0) {
            perror("Error recv");
            pthread_exit(NULL);
        }
        pBuffer = buffer;
        do {
            pBuffer = IRC_UnPipelineCommands(pBuffer, &command);

            fun = IRC_CommandQuery(command);
            /*Imprimimos el mensaje recibido en el registro plano*/
            IRCInterface_PlaneRegisterInMessageThread(command);
            if (fun != IRCERR_NOCOMMAND && fun != IRCERR_NOPARAMS && fun != IRCERR_UNKNOWNCOMMAND)
                Messages[fun](command);
            IRC_MFree(1, &command);
        } while (pBuffer);

    }

    pthread_exit(NULL);

}

/**
 * @defgroup IRCInterface Interface
 *
 */

/**
 * @defgroup IRCInterfaceCallbacks Callbaks
 * @ingroup IRCInterface
 *
 */

/**
 * @addtogroup IRCInterfaceCallbacks
 * Funciones que van a ser llamadas desde el interface y que deben ser implementadas por el usuario.
 * Todas estas funciones pertenecen al hilo del interfaz.
 *
 * El programador puede, por supuesto, separar todas estas funciones en múltiples ficheros a
 * efectos de desarrollo y modularización.
 *
 * <hr>
 */

/**
 * @ingroup IRCInterfaceCallbacks
 *
 * @page IRCInterface_ActivateChannelKey IRCInterface_ActivateChannelKey
 *
 * @brief Llamada por el botón de activación de la clave del canal.
 *
 * @synopsis
 * @code
 *	#include <redes2/ircxchat.h>
 *
 * 	void IRCInterface_ActivateChannelKey (char *channel, char * key)
 * @endcode
 *
 * @description
 * Llamada por el botón de activación de la clave del canal. El segundo parámetro es
 * la clave del canal que se desea poner. Si es NULL deberá impedirse la activación
 * con la función implementada a tal efecto. En cualquier caso sólo se puede realizar si el servidor acepta la orden.
 * Las strings recibidas no deben ser manipuladas por el programador, sólo leídas.
 *
 * @param[in] channel canal sobre el que se va a activar la clave.
 * @param[in] key clave para el canal indicado.
 *
 * @warning Esta función debe ser implementada por el alumno.
 *
 * @author
 * Eloy Anguiano (eloy.anguiano@uam.es)
 *
 *<hr>
 */

void IRCInterface_ActivateChannelKey(char *channel, char *key) {
    cambioModeChannel(channel, "+k", key);
}

/**
 * @ingroup IRCInterfaceCallbacks
 *
 * @page IRCInterface_ActivateExternalMessages IRCInterface_ActivateExternalMessages
 *
 * @brief Llamada por el botón de activación de la recepción de mensajes externos.
 *
 * @synopsis
 * @code
 *	#include <redes2/ircxchat.h>
 *
 * 	void IRCInterface_ActivateExternalMessages (char *channel)
 * @endcode
 *
 * @description
 * Llamada por el botón de activación de la recepción de mensajes externos.
 *
 * En cualquier caso sólo se puede realizar si el servidor acepta la orden.
 * La string recibida no debe ser manipulada por el programador, sólo leída.
 *
 * @param[in] channel canal sobre el que se activará la recepción de mensajes externos.
 *
 * @warning Esta función debe ser implementada por el alumno.
 *
 * @author
 * Eloy Anguiano (eloy.anguiano@uam.es)
 *
 *<hr>
 */

void IRCInterface_ActivateExternalMessages(char *channel) {
    cambioModeChannel(channel, "+n", NULL);
}

/**
 * @ingroup IRCInterfaceCallbacks
 *
 * @page IRCInterface_ActivateInvite IRCInterface_ActivateInvite
 *
 * @brief Llamada por el botón de activación de canal de sólo invitación.
 *
 * @synopsis
 * @code
 *	#include <redes2/ircxchat.h>
 *
 * 	void IRCInterface_ActivateInvite (char *channel)
 * @endcode
 *
 * @description
 * Llamada por el botón de activación de canal de sólo invitación.
 *
 * En cualquier caso sólo se puede realizar si el servidor acepta la orden.
 * La string recibida no debe ser manipulada por el programador, sólo leída.
 *
 * @param[in] channel canal sobre el que se activará la invitación.
 *
 * @warning Esta función debe ser implementada por el alumno.
 *
 * @author
 * Eloy Anguiano (eloy.anguiano@uam.es)
 *
 *<hr>
 */

void IRCInterface_ActivateInvite(char *channel) {
    cambioModeChannel(channel, "+i", NULL);
}

/**
 * @ingroup IRCInterfaceCallbacks
 *
 * @page IRCInterface_ActivateModerated IRCInterface_ActivateModerated
 *
 * @brief Llamada por el botón de activación de la moderación del canal.
 *
 * @synopsis
 * @code
 *	#include <redes2/ircxchat.h>
 *
 * 	void IRCInterface_ActivateModerated (char *channel)
 * @endcode
 *
 * @description
 * Llamada por el botón de activación de la moderación del canal.
 *
 * En cualquier caso sólo se puede realizar si el servidor acepta la orden.
 * La string recibida no debe ser manipulada por el programador, sólo leída.
 *
 * @param[in] channel canal sobre el que se activará la moderación.
 *
 * @warning Esta función debe ser implementada por el alumno.
 *
 * @author
 * Eloy Anguiano (eloy.anguiano@uam.es)
 *
 *<hr>
 */

void IRCInterface_ActivateModerated(char *channel) {
    cambioModeChannel(channel, "+m", NULL);
}

/**
 * @ingroup IRCInterfaceCallbacks
 *
 * @page IRCInterface_ActivateNicksLimit IRCInterface_ActivateNicksLimit
 *
 * @brief Llamada por el botón de activación del límite de usuarios en el canal.
 *
 * @synopsis
 * @code
 *	#include <redes2/ircxchat.h>
 *
 * 	void IRCInterface_ActivateNicksLimit (char *channel, int * limit)
 * @endcode
 *
 * @description
 * Llamada por el botón de activación del límite de usuarios en el canal. El segundo es el
 * límite de usuarios que se desea poner. Si el valor es 0 se sobrentiende que se desea eliminar
 * este límite.
 *
 * En cualquier caso sólo se puede realizar si el servidor acepta la orden.
 * La string recibida no debe ser manipulada por el programador, sólo leída.
 *
 * @param[in] channel canal sobre el que se activará el límite de usuarios.
 * @param[in] limit límite de usuarios en el canal indicado.
 *
 * @warning Esta función debe ser implementada por el alumno.
 *
 * @author
 * Eloy Anguiano (eloy.anguiano@uam.es)
 *
 *<hr>
 */

void IRCInterface_ActivateNicksLimit(char *channel, int limit) {
    char cad[50];
    sprintf(cad, "%d", limit);
    cambioModeChannel(channel, "+l", cad);
}

/**
 * @ingroup IRCInterfaceCallbacks
 *
 * @page IRCInterface_ActivatePrivate IRCInterface_ActivatePrivate
 *
 * @brief Llamada por el botón de activación del modo privado.
 *
 * @synopsis
 * @code
 *	#include <redes2/ircxchat.h>
 *
 * 	void IRCInterface_ActivatePrivate (char *channel)
 * @endcode
 *
 * @description
 * Llamada por el botón de activación del modo privado.
 *
 * En cualquier caso sólo se puede realizar si el servidor acepta la orden.
 * La string recibida no debe ser manipulada por el programador, sólo leída.
 *
 * @param[in] channel canal sobre el que se va a activar la privacidad.
 *
 * @warning Esta función debe ser implementada por el alumno.
 *
 * @author
 * Eloy Anguiano (eloy.anguiano@uam.es)
 *
 *<hr>
 */

void IRCInterface_ActivatePrivate(char *channel) {
    cambioModeChannel(channel, "+p", NULL);
}

/**
 * @ingroup IRCInterfaceCallbacks
 *
 * @page IRCInterface_ActivateProtectTopic IRCInterface_ActivateProtectTopic
 *
 * @brief Llamada por el botón de activación de la protección de tópico.
 *
 * @synopsis
 * @code
 *	#include <redes2/ircxchat.h>
 *
 * 	void IRCInterface_ActivateProtectTopic (char *channel)
 * @endcode
 *
 * @description
 * Llamada por el botón de activación de la protección de tópico.
 *
 * En cualquier caso sólo se puede realizar si el servidor acepta la orden.
 * La string recibida no debe ser manipulada por el programador, sólo leída.
 *
 * @param[in] channel canal sobre el que se va a activar la protección de tópico.
 *
 * @warning Esta función debe ser implementada por el alumno.
 *
 * @author
 * Eloy Anguiano (eloy.anguiano@uam.es)
 *
 *<hr>
 */

void IRCInterface_ActivateProtectTopic(char *channel) {
    cambioModeChannel(channel, "+t", NULL);
}

/**
 * @ingroup IRCInterfaceCallbacks
 *
 * @page IRCInterface_ActivateSecret IRCInterface_ActivateSecret
 *
 * @brief Llamada por el botón de activación de canal secreto.
 *
 * @synopsis
 * @code
 *	#include <redes2/ircxchat.h>
 *
 * 	void IRCInterface_ActivateSecret (char *channel)
 * @endcode
 *
 * @description
 * Llamada por el botón de activación de canal secreto.
 *
 * En cualquier caso sólo se puede realizar si el servidor acepta la orden.
 * La string recibida no debe ser manipulada por el programador, sólo leída.
 *
 * @param[in] channel canal sobre el que se va a activar el estado de secreto.
 *
 * @warning Esta función debe ser implementada por el alumno.
 *
 * @author
 * Eloy Anguiano (eloy.anguiano@uam.es)
 *
 *<hr>
 */

void IRCInterface_ActivateSecret(char *channel) {
    cambioModeChannel(channel, "+s", NULL);
}

/**
 * @ingroup IRCInterfaceCallbacks
 *
 * @page IRCInterface_BanNick IRCInterface_BanNick
 *
 * @brief Llamada por el botón "Banear".
 *
 * @synopsis
 * @code
 *	#include <redes2/ircxchat.h>
 *
 * 	void IRCInterface_BanNick (char *channel, char *nick)
 * @endcode
 *
 * @description
 * Llamada por el botón "Banear". Previamente debe seleccionarse un nick del
 * canal para darle voz a dicho usuario.
 *
 * En cualquier caso sólo se puede realizar si el servidor acepta la orden.
 * Las strings recibidas no deben ser manipuladas por el programador, sólo leídas.
 *
 * @param[in] channel canal sobre el que se va a realizar el baneo. En principio es un valor innecesario.
 * @param[in] nick nick del usuario que va a ser baneado
 *
 * @warning Esta función debe ser implementada por el alumno.
 *
 * @author
 * Eloy Anguiano (eloy.anguiano@uam.es)
 *
 *<hr>
 */

void IRCInterface_BanNick(char *channel, char *nick) {
    cambioModeChannel(channel, "+b", nick);
}

/**
 * @ingroup IRCInterfaceCallbacks
 *
 * @page IRCInterface_Connect IRCInterface_Connect
 *
 * @brief Llamada por los distintos botones de conexión.
 *
 * @synopsis
 * @code
 *	#include <redes2/ircxchat.h>
 *
 * 	long IRCInterface_Connect (char *nick, char * user, char * realname, char * password, char * server, int port, boolean ssl)
 * @endcode
 *
 * @description
 * Función a implementar por el programador.
 * Llamada por los distintos botones de conexión. Si implementará la comunicación completa, incluido
 * el registro del usuario en el servidor.
 *
 * En cualquier caso sólo se puede realizar si el servidor acepta la orden.
 * Las strings recibidas no deben ser manipuladas por el programador, sólo leída.
 *
 *
 * @param[in] nick nick con el que se va a realizar la conexíón.
 * @param[in] user usuario con el que se va a realizar la conexión.
 * @param[in] realname nombre real con el que se va a realizar la conexión.
 * @param[in] password password del usuario si es necesaria, puede valer NULL.
 * @param[in] server nombre o ip del servidor con el que se va a realizar la conexión.
 * @param[in] port puerto del servidor con el que se va a realizar la conexión.
 * @param[in] ssl puede ser TRUE si la conexión tiene que ser segura y FALSE si no es así.
 *
 * @retval IRC_OK si todo ha sido correcto (debe devolverlo).
 * @retval IRCERR_NOSSL si el valor de SSL es TRUE y no se puede activar la conexión SSL pero sí una
 * conexión no protegida (debe devolverlo).
 * @retval IRCERR_NOCONNECT en caso de que no se pueda realizar la comunicación (debe devolverlo).
 *
 * @warning Esta función debe ser implementada por el alumno.
 *
 * @author
 * Eloy Anguiano (eloy.anguiano@uam.es)
 *
 *<hr>
 */

long IRCInterface_Connect(char *nick, char *user, char *realname, char *password, char *server, int port, boolean ssl) {
    long retval = 0;
    char *command = NULL;
	SSL_CTX* ctx = NULL;
    pthread_t thread;

	if (ssl == FALSE){
		/*Abrimos el socket con el servidor*/
		ssl_channel = NULL;
		sck = openSocket(TCP);
		if (sck < 0)
		    return IRCERR_NOCONNECT;

		/*Nos conectamos al servidor*/
		if (connectClientSocket(sck, server, port) < 0)
		    return IRCERR_NOCONNECT;
	}
	else{
		ctx = inicializar_nivel_SSL("certs/ca.pem", "certs/cliente.pem");
		if(!ctx){
		    fprintf(stderr, "Los certificados del cliente no son correctos\n");
		    return IRCERR_NOCONNECT;
		}
		sck = openSocket(TCP);
		if (sck < 0){
		    perror("Error abriendo socket");
		    fprintf(stderr, "Error abriendo socket\n");
		    return IRCERR_NOCONNECT;
		}
		
		if(connectClientSocket(sck, server, port) < 0){
		    fprintf(stderr, "Error connect\n");
		    perror("Error connect");
		    close(sck);
		    return IRCERR_NOCONNECT;
		}
		
		ssl_channel = conectar_canal_seguro_SSL(ctx, sck);
		if(!ssl_channel){
		    perror("Error en el handshake inicial");
		    close(sck);
		    return IRCERR_NOCONNECT;
		}  
		if(evaluar_post_connectar_SSL(ssl_channel) < 0){
		    fprintf(stderr, "El servidor no ha enviado ningun certificado o no es verificado por la CA\n");
		    close(sck);
		    return IRCERR_NOCONNECT;
		}
		printf("Inicia ok");
	}

		/*Creamos el hilo que se encarga de recibir las respuestas del servidor*/
		if (pthread_create(&thread, NULL, recvInfo, (void *) &sck) < 0) {
		    fprintf(stderr, "Error creating thread\n");
		    return IRCERR_NOCONNECT;
		}
		if (pthread_detach(thread) < 0) {
		    fprintf(stderr, "Error  detach\n");
		    return IRCERR_NOCONNECT;
		}
	
	

    /*Creamos el mensaje de nick y lo enviamos*/
    retval = IRCMsg_Nick(&command, NULL, nick, NULL);
    if (retval != IRC_OK)
        return IRCERR_NOCONNECT;
    if (sendData(sck, ssl_channel, TCP, NULL, 0, command, strlen(command)) <= 0)
        return IRCERR_NOCONNECT;
    IRC_MFree(1, &command);

    retval = IRCMsg_User(&command, NULL, user, "", realname);
    if (retval != IRC_OK)
        return IRCERR_NOCONNECT;

    /*Creamos el mensaje de user y lo enviamos*/
	sendData(sck, ssl_channel, TCP, NULL, 0, command, strlen(command));

    IRC_MFree(1, &command);

    return IRC_OK;
}

/**
 * @ingroup IRCInterfaceCallbacks
 *
 * @page IRCInterface_DeactivateChannelKey IRCInterface_DeactivateChannelKey
 *
 * @brief Llamada por el botón de desactivación de la clave del canal.
 *
 * @synopsis
 * @code
 *	#include <redes2/ircxchat.h>
 *
 * 	void IRCInterface_DeactivateChannelKey (char *channel)
 * @endcode
 *
 * @description
 * Llamada por el botón de desactivación de la clave del canal.
 *
 * En cualquier caso sólo se puede realizar si el servidor acepta la orden.
 * La string recibida no debe ser manipulada por el programador, sólo leída.
 *
 * @param[in] channel canal sobre el que se va a desactivar la clave.
 *
 * @warning Esta función debe ser implementada por el alumno.
 *
 * @author
 * Eloy Anguiano (eloy.anguiano@uam.es)
 *
 *<hr>
 */

void IRCInterface_DeactivateChannelKey(char *channel) {
    cambioModeChannel(channel, "-k", NULL);
}

/**
 * @ingroup IRCInterfaceCallbacks
 *
 * @page IRCInterface_DeactivateExternalMessages IRCInterface_DeactivateExternalMessages
 *
 * @brief Llamada por el botón de desactivación de la recepción de mensajes externos.
 *
 * @synopsis
 * @code
 *	#include <redes2/ircxchat.h>
 *
 * 	void IRCInterface_DeactivateExternalMessages (char *channel)
 * @endcode
 *
 * @description
 * Llamada por el botón de desactivación de la recepción de mensajes externos.
 *
 * En cualquier caso sólo se puede realizar si el servidor acepta la orden.
 * La string recibida no debe ser manipulada por el programador, sólo leída.
 *
 * @param[in] channel canal sobre el que se va a deactivar la recepción de mensajes externos.
 *
 * @warning Esta función debe ser implementada por el alumno.
 *
 * @author
 * Eloy Anguiano (eloy.anguiano@uam.es)
 *
 *<hr>
 */

void IRCInterface_DeactivateExternalMessages(char *channel) {
    cambioModeChannel(channel, "-n", NULL);
}

/**
 * @ingroup IRCInterfaceCallbacks
 *
 * @page IRCInterface_DeactivateInvite IRCInterface_DeactivateInvite
 *
 * @brief Llamada por el botón de desactivación de canal de sólo invitación.
 *
 * @synopsis
 * @code
 *	#include <redes2/ircxchat.h>
 *
 * 	void IRCInterface_DeactivateInvite (char *channel)
 * @endcode
 *
 * @description
 * Llamada por el botón de desactivación de canal de sólo invitación.
 *
 * En cualquier caso sólo se puede realizar si el servidor acepta la orden.
 * La string recibida no debe ser manipulada por el programador, sólo leída.
 *
 * @param[in] channel canal sobre el que se va a desactivar la invitación.
 *
 * @warning Esta función debe ser implementada por el alumno.
 *
 * @author
 * Eloy Anguiano (eloy.anguiano@uam.es)
 *
 *<hr>
 */

void IRCInterface_DeactivateInvite(char *channel) {
    cambioModeChannel(channel, "-i", NULL);
}

/**
 * @ingroup IRCInterfaceCallbacks
 *
 * @page IRCInterface_DeactivateModerated IRCInterface_DeactivateModerated
 *
 * @brief Llamada por el botón de desactivación  de la moderación del canal.
 *
 * @synopsis
 * @code
 *	#include <redes2/ircxchat.h>
 *
 * 	void IRCInterface_DeactivateModerated (char *channel)
 * @endcode
 *
 * @description
 * Llamada por el botón de desactivación  de la moderación del canal.
 *
 * En cualquier caso sólo se puede realizar si el servidor acepta la orden.
 * La string recibida no debe ser manipulada por el programador, sólo leída.
 *
 * @param[in] channel canal sobre el que se va a desactivar la moderación.
 *
 * @warning Esta función debe ser implementada por el alumno.
 *
 * @author
 * Eloy Anguiano (eloy.anguiano@uam.es)
 *
 *<hr>
 */

void IRCInterface_DeactivateModerated(char *channel) {
    cambioModeChannel(channel, "-m", NULL);
}

/**
 * @ingroup IRCInterfaceCallbacks
 *
 * @page IRCInterface_DeactivateNicksLimit IRCInterface_DeactivateNicksLimit
 *
 * @brief Llamada por el botón de desactivación de la protección de tópico.
 *
 * @synopsis
 * @code
 *	#include <redes2/ircxchat.h>
 *
 * 	void IRCInterface_DeactivateNicksLimit (char *channel)
 * @endcode
 *
 * @description
 * Llamada por el botón de desactivación  del límite de usuarios en el canal.
 *
 * En cualquier caso sólo se puede realizar si el servidor acepta la orden.
 * La string recibida no debe ser manipulada por el programador, sólo leída.
 *
 * @param[in] channel canal sobre el que se va a desactivar el límite de usuarios.
 *
 * @warning Esta función debe ser implementada por el alumno.
 *
 * @author
 * Eloy Anguiano (eloy.anguiano@uam.es)
 *
 *<hr>
 */

void IRCInterface_DeactivateNicksLimit(char *channel) {
    cambioModeChannel(channel, "-l", NULL);
}

/**
 * @ingroup IRCInterfaceCallbacks
 *
 * @page IRCInterface_DeactivatePrivate IRCInterface_DeactivatePrivate
 *
 * @brief Llamada por el botón de desactivación del modo privado.
 *
 * @synopsis
 * @code
 *	#include <redes2/ircxchat.h>
 *
 * 	void IRCInterface_DeactivatePrivate (char *channel)
 * @endcode
 *
 * @description
 * Llamada por el botón de desactivación del modo privado.
 *
 * En cualquier caso sólo se puede realizar si el servidor acepta la orden.
 * La string recibida no debe ser manipulada por el programador, sólo leída.
 *
 * @warning Esta función debe ser implementada por el alumno.
 *
 * @param[in] channel canal sobre el que se va a desactivar la privacidad.
 *
 * @warning Esta función debe ser implementada por el alumno.
 *
 * @author
 * Eloy Anguiano (eloy.anguiano@uam.es)
 *
 *<hr>
 */

void IRCInterface_DeactivatePrivate(char *channel) {
    cambioModeChannel(channel, "-p", NULL);
}

/**
 * @ingroup IRCInterfaceCallbacks
 *
 * @page IRCInterface_DeactivateProtectTopic IRCInterface_DeactivateProtectTopic
 *
 * @brief Llamada por el botón de desactivación de la protección de tópico.
 *
 * @synopsis
 * @code
 *	#include <redes2/ircxchat.h>
 *
 * 	void IRCInterface_DeactivateProtectTopic (char *channel)
 * @endcode
 *
 * @description
 * Llamada por el botón de desactivación de la protección de tópico.
 *
 * En cualquier caso sólo se puede realizar si el servidor acepta la orden.
 * La string recibida no debe ser manipulada por el programador, sólo leída.
 *
 * @param[in] channel canal sobre el que se va a desactivar la protección de tópico.
 *
 * @warning Esta función debe ser implementada por el alumno.
 *
 * @author
 * Eloy Anguiano (eloy.anguiano@uam.es)
 *
 *<hr>
 */

void IRCInterface_DeactivateProtectTopic(char *channel) {
    cambioModeChannel(channel, "-t", NULL);
}

/**
 * @ingroup IRCInterfaceCallbacks
 *
 * @page IRCInterface_DeactivateSecret IRCInterface_DeactivateSecret
 *
 * @brief Llamada por el botón de desactivación de canal secreto.
 *
 * @synopsis
 * @code
 *	#include <redes2/ircxchat.h>
 *
 * 	void IRCInterface_DeactivateSecret (char *channel)
 * @endcode
 *
 * @description
 * Llamada por el botón de desactivación de canal secreto.
 *
 * En cualquier caso sólo se puede realizar si el servidor acepta la orden.
 * La string recibida no debe ser manipulada por el programador, sólo leída.
 *
 * @param[in] channel canal sobre el que se va a desactivar la propiedad de canal secreto.
 *
 * @warning Esta función debe ser implementada por el alumno.
 *
 * @author
 * Eloy Anguiano (eloy.anguiano@uam.es)
 *
 *<hr>
 */

void IRCInterface_DeactivateSecret(char *channel) {
    cambioModeChannel(channel, "-s", NULL);
}

/**
 * @ingroup IRCInterfaceCallbacks
 *
 * @page IRCInterface_DisconnectServer IRCInterface_DisconnectServer
 *
 * @brief Llamada por los distintos botones de desconexión.
 *
 * @synopsis
 * @code
 *	#include <redes2/ircxchat.h>
 *
 * 	void IRCInterface_DisconnectServer (char * server, int port)
 * @endcode
 *
 * @description
 * Llamada por los distintos botones de desconexión. Debe cerrar la conexión con el servidor.
 *
 * En cualquier caso sólo se puede realizar si el servidor acepta la orden.
 * La string recibida no debe ser manipulada por el programador, sólo leída.

 * @param[in] server nombre o ip del servidor del que se va a realizar la desconexión.
 * @param[in] port puerto sobre el que se va a realizar la desconexión.
 *
 * @retval TRUE si se ha cerrado la conexión (debe devolverlo).
 * @retval FALSE en caso contrario (debe devolverlo).
 *
 * @warning Esta función debe ser implementada por el alumno.
 *
 * @author
 * Eloy Anguiano (eloy.anguiano@uam.es)
 *
 *<hr>
 */

boolean IRCInterface_DisconnectServer(char *server, int port) {
	userCommandQuit("/quit", ssl_channel);
    return TRUE;
}


/**
 * @ingroup IRCInterfaceCallbacks
 *
 * @page IRCInterface_ExitAudioChat IRCInterface_ExitAudioChat
 *
 * @brief Llamada por el botón "Cancelar" del diálogo de chat de voz.
 *
 * @synopsis
 * @code
 *	#include <redes2/ircxchat.h>
 *
 * 	void IRCInterface_ExitAudioChat (char *nick)
 * @endcode
 *
 * @description
 * Llamada por el botón "Parar" del diálogo de chat de voz. Previamente debe seleccionarse un nick del
 * canal para darle voz a dicho usuario. Esta función cierrala comunicación. Evidentemente tiene que
 * actuar sobre el hilo de chat de voz.
 *
 * En cualquier caso sólo se puede realizar si el servidor acepta la orden.
 * La string recibida no debe ser manipulada por el programador, sólo leída.
 *
 * @param[in] nick nick del usuario que solicita la parada del chat de audio.
 *
 * @retval TRUE si se ha cerrado la comunicación (debe devolverlo).
 * @retval FALSE en caso contrario (debe devolverlo).
 *
 * @warning Esta función debe ser implementada por el alumno.
 *
 * @author
 * Eloy Anguiano (eloy.anguiano@uam.es)
 *
 *<hr>
 */

boolean IRCInterface_ExitAudioChat(char *nick) {
    return TRUE;
}

/**
 * @ingroup IRCInterfaceCallbacks
 *
 * @page IRCInterface_GiveOp IRCInterface_GiveOp
 *
 * @brief Llamada por el botón "Op".
 *
 * @synopsis
 * @code
 *	#include <redes2/ircxchat.h>
 *
 * 	void IRCInterface_GiveOp (char *channel, char *nick)
 * @endcode
 *
 * @description
 * Llamada por el botón "Op". Previamente debe seleccionarse un nick del
 * canal para darle "op" a dicho usuario.
 *
 * En cualquier caso sólo se puede realizar si el servidor acepta la orden.
 * Las strings recibidas no deben ser manipuladas por el programador, sólo leídas.
 *
 * @param[in] channel canal sobre el que se va dar op al usuario.
 * @param[in] nick nick al que se le va a dar el nivel de op.
 *
 * @warning Esta función debe ser implementada por el alumno.
 *
 * @author
 * Eloy Anguiano (eloy.anguiano@uam.es)
 *
 *<hr>
 */

void IRCInterface_GiveOp(char *channel, char *nick) {
    cambioModeChannel(channel, "+o", nick);
}

/**
 * @ingroup IRCInterfaceCallbacks
 *
 * @page IRCInterface_GiveVoice IRCInterface_GiveVoice
 *
 * @brief Llamada por el botón "Dar voz".
 *
 * @synopsis
 * @code
 *	#include <redes2/ircxchat.h>
 *
 * 	void IRCInterface_GiveVoice (char *channel, char *nick)
 * @endcode
 *
 * @description
 * Llamada por el botón "Dar voz". Previamente debe seleccionarse un nick del
 * canal para darle voz a dicho usuario.
 *
 * En cualquier caso sólo se puede realizar si el servidor acepta la orden.
 * Las strings recibidas no deben ser manipuladas por el programador, sólo leídas.
 *
 * @param[in] channel canal sobre el que se va dar voz al usuario.
 * @param[in] nick nick al que se le va a dar voz.
 *
 * @warning Esta función debe ser implementada por el alumno.
 *
 * @author
 * Eloy Anguiano (eloy.anguiano@uam.es)
 *
 *<hr>
 */

void IRCInterface_GiveVoice(char *channel, char *nick) {
    cambioModeChannel(channel, "+v", nick);
}

/**
 * @ingroup IRCInterfaceCallbacks
 *
 * @page IRCInterface_KickNick IRCInterface_KickNick
 *
 * @brief Llamada por el botón "Echar".
 *
 * @synopsis
 * @code
 *	#include <redes2/ircxchat.h>
 *
 * 	void IRCInterface_KickNick (char *channel, char *nick)
 * @endcode
 *
 * @description
 * Llamada por el botón "Echar". Previamente debe seleccionarse un nick del
 * canal para darle voz a dicho usuario.
 *
 * En cualquier caso sólo se puede realizar si el servidor acepta la orden.
 * Las strings recibidas no deben ser manipuladas por el programador, sólo leídas.
 *
 * @param[in] channel canal sobre el que se va a expulsar al usuario.
 * @param[in] nick nick del usuario que va a ser expulsado.
 *
 * @warning Esta función debe ser implementada por el alumno.
 *
 * @author
 * Eloy Anguiano (eloy.anguiano@uam.es)
 *
 *<hr>
 */

void IRCInterface_KickNick(char *channel, char *nick) {
    char *msgServer;
    msgServer = NULL;

    if (IRCMsg_Kick(&msgServer, NULL, IRCInterface_ActiveChannelName(), nick, NULL) != IRC_OK) {
        IRCInterface_WriteSystem(NULL, "Error al expulsar usuario");
        return;
    }

    if (sendData(sck, ssl_channel, TCP, NULL, 0, msgServer, strlen(msgServer)) <= 0) {
        IRCInterface_WriteSystem(NULL, "Error al conectar con el servidor");
        return;
    }

    IRCInterface_PlaneRegisterOutMessage(msgServer);
    IRC_MFree(1, &msgServer);
    return;

}

/**
 * @ingroup IRCInterfaceCallbacks
 *
 * @page IRCInterface_NewCommandText IRCInterface_NewCommandText
 *
 * @brief Llamada la tecla ENTER en el campo de texto y comandos.
 *
 * @synopsis
 * @code
 *	#include <redes2/ircxchat.h>
 *
 * 	void IRCInterface_NewCommandText (char *command)
 * @endcode
 *
 * @description
 * Llamada de la tecla ENTER en el campo de texto y comandos. El texto deberá ser
 * enviado y el comando procesado por las funciones de "parseo" de comandos de
 * usuario.
 *
 * En cualquier caso sólo se puede realizar si el servidor acepta la orden.
 * La string recibida no debe ser manipulada por el programador, sólo leída.
 *
 * @param[in] comando introducido por el usuario.
 *
 * @warning Esta función debe ser implementada por el alumno.
 *
 * @author
 * Eloy Anguiano (eloy.anguiano@uam.es)
 *
 *<hr>
 */

void IRCInterface_NewCommandText(char *command) {
    long fun;
    char cad[512] = "", *channel;
    channel = NULL;

    fun = IRCUser_CommandQuery(command);

    if (fun != IRCERR_NOSTRING && fun != IRCERR_NOUSERCOMMAND && fun != IRCERR_NOVALIDUSERCOMMAND) {
        UserCommands[fun](command, ssl_channel);
    } else if (fun == IRCERR_NOUSERCOMMAND) {
        /*caso PRIVMSG por interfaz*/
        channel = IRCInterface_ActiveChannelName();
        sprintf(cad, "/privmsg %s %s", channel, command);
        UserCommands[15](cad, ssl_channel);
    }
}

/**
 * @ingroup IRCInterfaceCallbacks
 *
 * @page IRCInterface_NewTopicEnter IRCInterface_NewTopicEnter
 *
 * @brief Llamada cuando se pulsa la tecla ENTER en el campo de tópico.
 *
 * @synopsis
 * @code
 *	#include <redes2/ircxchat.h>
 *
 * 	void IRCInterface_NewTopicEnter (char * topicdata)
 * @endcode
 *
 * @description
 * Llamada cuando se pulsa la tecla ENTER en el campo de tópico.
 * Deberá intentarse cambiar el tópico del canal.
 *
 * En cualquier caso sólo se puede realizar si el servidor acepta la orden.
 * La string recibida no debe ser manipulada por el programador, sólo leída.
 *
 * param[in] topicdata string con el tópico que se desea poner en el canal.
 *
 * @warning Esta función debe ser implementada por el alumno.
 *
 * @author
 * Eloy Anguiano (eloy.anguiano@uam.es)
 *
 *<hr>
 */

void IRCInterface_NewTopicEnter(char *topicdata) {
    char *command = NULL, *channel = NULL;
    channel = IRCInterface_ActiveChannelName();

    /*Creamos el mensaje de topic*/
    if (IRCMsg_Topic(&command, NULL, channel, topicdata) != IRC_OK) {
        IRCInterface_WriteChannel(channel, NULL, "Error al cambiar el topic");
        return;
    }

    /*Enviamos el mensaje al servidor*/
    if (sendData(sck, ssl_channel, TCP, NULL, 0, command, strlen(command)) <= 0) {
        IRC_MFree(1, &command);
        return;
    }
    /*Escribimos en el registro plano*/
    IRCInterface_PlaneRegisterOutMessage(command);
    IRC_MFree(1, &command);
}

/**
 * @ingroup IRCInterfaceCallbacks
 *
 * @page IRCInterface_SendFile IRCInterface_SendFile
 *
 * @brief Llamada por el botón "Enviar Archivo".
 *
 * @synopsis
 * @code
 *	#include <redes2/ircxchat.h>
 *
 * 	void IRCInterface_SendFile (char * filename, char *nick, char *data, long unsigned int length)
 * @endcode
 *
 * @description
 * Llamada por el botón "Enviar Archivo". Previamente debe seleccionarse un nick del
 * canal para darle voz a dicho usuario. Esta función como todos los demás callbacks bloquea el interface
 * y por tanto es el programador el que debe determinar si crea un nuevo hilo para enviar el archivo o
 * no lo hace.
 *
 * En cualquier caso sólo se puede realizar si el servidor acepta la orden.
 * Las strings recibidas no deben ser manipuladas por el programador, sólo leídas.
 *
 * @param[in] filename nombre del fichero a enviar.
 * @param[in] nick nick del usuario que enviará el fichero.
 * @param[in] data datos a ser enviados.
 * @param[in] length longitud de los datos a ser enviados.
 *
 * @retval TRUE si se ha establecido la comunicación (debe devolverlo).
 * @retval FALSE en caso contrario (debe devolverlo).
 *
 * @warning Esta función debe ser implementada por el alumno.
 *
 * @author
 * Eloy Anguiano (eloy.anguiano@uam.es)
 *
 *<hr>
 */

boolean IRCInterface_SendFile(char *filename, char *nick, char *data, long unsigned int length) {
    /*Los ficheros iran codificados dentro de un privmsg a un usuario concreto*/
    /*Comandos posibles (siguiendo el faq de metis):
     * FSEND: solicita al usuario destinatario el envio
     *        parametros: nombre_fichero longitud IP(emisor) puerto
     * FACCEPT: el destinatario acepta la recepcion
     *          parametros: IP(emisor)
     * FCANCEL: cancela o rechaza la recepcion del archivo*/
    char msg[MAXLEN], *command = NULL;
    int port, sckF;
    struct sockaddr_in my_addr;
    socklen_t slen = sizeof (my_addr);
    pthread_t thread;
    struct fileSender_args *fs;

    fs = (struct fileSender_args*) malloc(sizeof (struct fileSender_args));
    if (fs == NULL) {
        IRCInterface_WriteSystem(NULL, "Error al establecer la comunicacion");
        return FALSE;
    }

    /*Abrimos un socket TCP*/
    sckF = openSocket(TCP);
    if (sckF < 0) {
        IRCInterface_WriteSystem(NULL, "Error al establecer la comunicacion");
        return FALSE;
    }
    /*Hacemos directamente el listen para que el servidor asigne un puerto a la conexion*/
    if (listen(sckF, 1) < 0) {
        IRCInterface_WriteSystem(NULL, "Error al establecer la comunicacion");
        close(sckF);
        return FALSE;
    }
    /*Obtenemos el puerto*/
    getsockname(sckF, (struct sockaddr*) &my_addr, &slen);
    port = ntohs(my_addr.sin_port);
    
    fs->data = data;
    fs->length = length;
    fs->sck = sckF;

    /*Creamos un hilo adicional para que atienda al receptor del fichero*/
    if (pthread_create(&thread, NULL, fileSender, (void *) fs) < 0) {
        IRCInterface_WriteSystem(NULL, "Error al establecer la comunicacion");
        close(sckF);
        return FALSE;
    }
    pthread_detach(thread);


    /*Creamos el mensaje PRIVMSG correspondiente*/
    sprintf(msg, "\001FSEND %s %s %d %ld\r\n", filename, hostName, port, length);
    if (IRCMsg_Privmsg(&command, NULL, nick, msg) != IRC_OK) {
        IRCInterface_WriteSystem(NULL, "Error al establecer la comunicacion con el servidor");
        close(sckF);
        return FALSE;
    }

    /*Enviamos el comando*/
    if (sendData(sck, ssl_channel, TCP, NULL, 0, command, strlen(command)) <= 0) {
        IRCInterface_WriteSystem(NULL, "Error al establecer la comunicacion con el servidor");
        close(sckF);
        return FALSE;
    }

    return TRUE;
}

/**
 * @ingroup IRCInterfaceCallbacks
 *
 * @page IRCInterface_StartAudioChat IRCInterface_StartAudioChat
 *
 * @brief Llamada por el botón "Iniciar" del diálogo de chat de voz.
 *
 * @synopsis
 * @code
 *	#include <redes2/ircxchat.h>
 *
 * 	void IRCInterface_StartAudioChat (char *nick)
 * @endcode
 *
 * @description
 * Llamada por el botón "Iniciar" del diálogo de chat de voz. Previamente debe seleccionarse un nick del
 * canal para darle voz a dicho usuario. Esta función como todos los demás callbacks bloquea el interface
 * y por tanto para mantener la funcionalidad del chat de voz es imprescindible crear un hilo a efectos
 * de comunicación de voz.
 *
 * En cualquier caso sólo se puede realizar si el servidor acepta la orden.
 * La string recibida no debe ser manipulada por el programador, sólo leída.
 *
 * @param[in] nick nick del usuario con el que se desea conectar.
 *
 * @retval TRUE si se ha establecido la comunicación (debe devolverlo).
 * @retval FALSE en caso contrario (debe devolverlo).
 *
 * @warning Esta función debe ser implementada por el alumno.
 *
 * @author
 * Eloy Anguiano (eloy.anguiano@uam.es)
 *
 *<hr>
 */

boolean IRCInterface_StartAudioChat(char *nick) {
    return TRUE;
}

/**
 * @ingroup IRCInterfaceCallbacks
 *
 * @page IRCInterface_StopAudioChat IRCInterface_StopAudioChat
 *
 * @brief Llamada por el botón "Parar" del diálogo de chat de voz.
 *
 * @synopsis
 * @code
 *	#include <redes2/ircxchat.h>
 *
 * 	void IRCInterface_StopAudioChat (char *nick)
 * @endcode
 *
 * @description
 * Llamada por el botón "Parar" del diálogo de chat de voz. Previamente debe seleccionarse un nick del
 * canal para darle voz a dicho usuario. Esta función sólo para la comunicación que puede ser reiniciada.
 * Evidentemente tiene que actuar sobre el hilo de chat de voz.
 *
 * En cualquier caso sólo se puede realizar si el servidor acepta la orden.
 * La string recibida no debe ser manipulada por el programador, sólo leída.
 *
 * @param[in] nick nick del usuario con el que se quiere parar el chat de voz.
 *
 * @retval TRUE si se ha parado la comunicación (debe devolverlo).
 * @retval FALSE en caso contrario (debe devolverlo).
 *
 * @warning Esta función debe ser implementada por el alumno.
 *
 * @author
 * Eloy Anguiano (eloy.anguiano@uam.es)
 *
 *<hr>
 */

boolean IRCInterface_StopAudioChat(char *nick) {
    return TRUE;
}

/**
 * @ingroup IRCInterfaceCallbacks
 *
 * @page IRCInterface_TakeOp IRCInterface_TakeOp
 *
 * @brief Llamada por el botón "Quitar Op".
 *
 * @synopsis
 * @code
 *	#include <redes2/ircxchat.h>
 *
 * 	void IRCInterface_TakeOp (char *channel, char *nick)
 * @endcode
 *
 * @description
 * Llamada por el botón "Quitar Op". Previamente debe seleccionarse un nick del
 * canal para quitarle "op" a dicho usuario.
 *
 * En cualquier caso sólo se puede realizar si el servidor acepta la orden.
 * Las strings recibidas no deben ser manipuladas por el programador, sólo leídas.
 *
 * @param[in] channel canal sobre el que se va a quitar op al usuario.
 * @param[in] nick nick del usuario al que se le va a quitar op.
 *
 * @warning Esta función debe ser implementada por el alumno.
 *
 * @author
 * Eloy Anguiano (eloy.anguiano@uam.es)
 *
 *<hr>
 */

void IRCInterface_TakeOp(char *channel, char *nick) {
    cambioModeChannel(channel, "-o", nick);
}

/**
 * @ingroup IRCInterfaceCallbacks
 *
 * @page IRCInterface_TakeVoice IRCInterface_TakeVoice
 *
 * @brief Llamada por el botón "Quitar voz".
 *
 * @synopsis
 * @code
 *	#include <redes2/ircxchat.h>
 *
 * 	void IRCInterface_TakeVoice (char *channel, char *nick)
 * @endcode
 *
 * @description
 * Llamada por el botón "Quitar voz". Previamente debe seleccionarse un nick del
 * canal para darle voz a dicho usuario.
 *
 * En cualquier caso sólo se puede realizar si el servidor acepta la orden.
 * Las strings recibidas no deben ser manipuladas por el programador, sólo leídas.
 *
 * @param[in] channel canal sobre el que se le va a quitar voz al usuario.
 * @param[in] nick nick del usuario al que se va a quitar la voz.
 *
 * @warning Esta función debe ser implementada por el alumno.
 *
 * @author
 * Eloy Anguiano (eloy.anguiano@uam.es)
 *
 *<hr>
 */

void IRCInterface_TakeVoice(char *channel, char *nick) {
    cambioModeChannel(channel, "-v", nick);
}


/***************************************************************************************************/
/***************************************************************************************************/
/**                                                                                               **/
/** MMMMMMMMMM               MMMMM           AAAAAAA           IIIIIII NNNNNNNNNN          NNNNNN **/
/**  MMMMMMMMMM             MMMMM            AAAAAAAA           IIIII   NNNNNNNNNN          NNNN  **/
/**   MMMMM MMMM           MM MM            AAAAA   AA           III     NNNNN NNNN          NN   **/
/**   MMMMM  MMMM         MM  MM            AAAAA   AA           III     NNNNN  NNNN         NN   **/
/**   MMMMM   MMMM       MM   MM           AAAAA     AA          III     NNNNN   NNNN        NN   **/
/**   MMMMM    MMMM     MM    MM           AAAAA     AA          III     NNNNN    NNNN       NN   **/
/**   MMMMM     MMMM   MM     MM          AAAAA       AA         III     NNNNN     NNNN      NN   **/
/**   MMMMM      MMMM MM      MM          AAAAAAAAAAAAAA         III     NNNNN      NNNN     NN   **/
/**   MMMMM       MMMMM       MM         AAAAA         AA        III     NNNNN       NNNN    NN   **/
/**   MMMMM        MMM        MM         AAAAA         AA        III     NNNNN        NNNN   NN   **/
/**   MMMMM                   MM        AAAAA           AA       III     NNNNN         NNNN  NN   **/
/**   MMMMM                   MM        AAAAA           AA       III     NNNNN          NNNN NN   **/
/**  MMMMMMM                 MMMM     AAAAAA            AAAA    IIIII   NNNNNN           NNNNNNN  **/
/** MMMMMMMMM               MMMMMM  AAAAAAAA           AAAAAA  IIIIIII NNNNNNN            NNNNNNN **/
/**                                                                                               **/
/***************************************************************************************************/

/***************************************************************************************************/



int main(int argc, char *argv[]) {
    /* La función IRCInterface_Run debe ser llamada al final      */
    /* del main y es la que activa el interfaz gráfico quedándose */
    /* en esta función hasta que se pulsa alguna salida del       */
    /* interfaz gráfico.                                          */
    int i, port = 6669;
    int long_index;
    SSL_CTX *ctx;
    char opt;

    static struct option options[] = {
        {"port", required_argument, 0, '2'},
	{"ssldata", required_argument, 0,'1'},
        {0, 0, 0, 0}
    };

    /*printf("%ld\n", IRC_CommandQuery(":irc.eps.net 332 ornstein #redes2 :Canal para debatir sobre IRC"));*/


    for (i = 0; i < 64; i++)
        UserCommands[i] = userCommandDefault;
    UserCommands[1] = userCommandNames;
    UserCommands[3] = userCommandList;
    UserCommands[4] = userCommandJoin;
    UserCommands[5] = userCommandPart;
    UserCommands[7] = userCommandQuit;
    UserCommands[8] = userCommandNick;
    UserCommands[9] = userCommandAway;
    UserCommands[10] = userCommandWhois;
    UserCommands[12] = userCommandKick;
    UserCommands[13] = userCommandTopic;
    UserCommands[15] = userCommandPrivmsg;
    UserCommands[33] = userCommandMode;
    UserCommands[49] = userCommandBack;
    for (i = 0; i < 2047; i++)
        Messages[i] = msgDefault;
    Messages[2] = msgNickChanged;
    Messages[5] = msgModeChanged;
    Messages[7] = msgQuit;
    Messages[9] = msgJoin;
    Messages[10] = msgPart;
    Messages[11] = msgTopicChanged;
    Messages[15] = msgKick;
    Messages[16] = msgPrivmsg;
    Messages[17] = msgNotice;
    Messages[34] = msgPing; /*envia la respuesta de pong*/
    Messages[74] = msgYourHost;
    Messages[75] = msgMyInfo;
    Messages[79] = msgWhoisEnd;
    Messages[83] = msgWhoisUser;
    Messages[84] = msgWhoisChannels;
    Messages[86] = msgWhoisServer;
    Messages[87] = msgWhoisIdle;
    Messages[88] = msgWho;
    Messages[99] = msgMode;
    Messages[100] = msgNotOperator;
    Messages[104] = msgEndNames;
    Messages[111] = msgList;
    Messages[113] = msgNames;
    Messages[132] = msgMOTDEnd;
    Messages[140] = msgMOTD;
    Messages[141] = msgMOTDStart;
    Messages[172] = msgAway;
    Messages[176] = msgBack;
    Messages[181] = msgrplAway;
    Messages[182] = msgNoNickOrChannel;
    Messages[183] = msgWelcome;
    Messages[184] = msgCreated;
    Messages[185] = msgIsSupport;

    if(argc == 1)
        IRCInterface_Run(argc, argv);
        

    while ((opt = getopt_long_only(argc, argv, "1:2:?", options, &long_index)) != -1) {
        switch (opt) {
            case '1':
                port = atoi(optarg);
            case '2':
		        /*Inicio capa ssl*/
		        ctx = inicializar_nivel_SSL("certs/ca.pem", "certs/cliente.pem");
		        if(!ctx){
		            fprintf(stderr, "Los certificados del cliente no son correctos\n");
		            return -1;
		        }
		        sck = openSocket(TCP);
		        if (sck < 0){
		            perror("Error abriendo socket");
		            fprintf(stderr, "Error abriendo socket\n");
		            return -1;
		        }
		
		        if(connectClientSocket(sck, "localhost", port) < 0){
		            fprintf(stderr, "Error connect\n");
		            perror("Error connect");
		            close(sck);
		            return -1;
		        }
		
		        ssl_channel = conectar_canal_seguro_SSL(ctx, sck);
		        if(!ssl_channel){
		            perror("Error en el handshake inicial");
		            close(sck);
		            return -1;
		        }  
		        if(evaluar_post_connectar_SSL(ssl_channel) < 0){
		            fprintf(stderr, "El servidor no ha enviado ningun certificado o no es verificado por la CA\n");
		            close(sck);
		            return -1;
		        }
		        enviar_datos_SSL(ssl_channel, optarg, strlen(optarg));
		        cerrar_canal_SSL(ssl_channel,ctx);
		        break;
		
            case '?':
            default:
                printf("Ayuda:\n");
                printf("--port\tEspecifica el puerto para conexiones seguras. Si no se especifica la bandera de ssl, se ignorará\n");
                printf("--ssldata\tEnvía a localhost la cadena especificada cifrada con ssl. Si no se especifica el puerto supone 6669\n");
                break;
        }
    }



    return 0;
}
