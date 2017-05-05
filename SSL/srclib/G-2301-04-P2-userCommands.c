#include "../includes/G-2301-04-P2-userCommands.h"

/**
 * Accion por defecto para los mensajes
 * @param m_in mensaje que se va a parsear
 * @return 0 si OK
 * @return -1 ERROR
 */
int userCommandDefault(char *m_in, SSL *ssl) {
    IRCInterface_WriteSystem(NULL, "Comando no reconocido");
    return -1;
}

/**
 * Parsea el comando de usuario NAMES crea el mensaje para el servidor y lo envia
 * @param m_in mensaje que se va a parsear
 * @return 0 si OK
 * @return -1 ERROR
 */
int userCommandNames(char *m_in, SSL *ssl) {
    char *channel, *targetserver, *msgServer, *currentChannel;
    currentChannel = msgServer = channel = targetserver = NULL;

    /*Parseamos el comando*/
    if (IRCUserParse_Names(m_in, &channel, &targetserver) != IRC_OK) {
        /*Si no se especifica canal rellenamos con el canal abierto*/
        currentChannel = IRCInterface_ActiveChannelName();
        channel = (char *) malloc((strlen(currentChannel) + 1) * sizeof (char));
        strcpy(channel, currentChannel);
    }

    /*Creamos el mensaje de names*/
    if (IRCMsg_Names(&msgServer, NULL, channel, NULL) != IRC_OK) {
        IRC_MFree(2, &channel, &targetserver);
        return -1;
    }
    /*Enviamos el mensaje*/
    if (sendData(sck, ssl, TCP, NULL, 0, msgServer, strlen(msgServer)) <= 0) {
        IRC_MFree(3, &msgServer, &channel, &targetserver);
        return -1;
    }
    /*Escribimos en el registro plano*/
    IRCInterface_PlaneRegisterOutMessage(msgServer);

    IRC_MFree(3, &msgServer, &channel, &targetserver);
    return 0;
}

/**
 * Parsea el comando de usuario LIST crea el mensaje para el servidor y lo envia
 * @param m_in mensaje que se va a parsear
 * @return 0 si OK
 * @return -1 ERROR
 */
int userCommandList(char *command, SSL *ssl) {
    char *channel, *searchstring, *msg;
    msg = channel = searchstring = NULL;

    /*Parseamos el comando de usuario*/
    if (IRCUserParse_List(command, &channel, &searchstring) != IRC_OK)
        return -1;

    /*Creamos el mensaje para mandarlo al servidor*/
    if (IRCMsg_List(&msg, NULL, channel, NULL) != IRC_OK)
        return -1;

    if (sendData(sck, ssl, TCP, NULL, 0, msg, strlen(msg))<= 0)
        return -1;
    /*Escribimos el mensaje en el registro plano*/
    IRCInterface_PlaneRegisterOutMessage(msg);

    IRC_MFree(3, &msg, &channel, &searchstring);

    return 0;
}

/**
 * Parsea el comando de usuario JOIN crea el mensaje para el servidor y lo envia
 * @param m_in mensaje que se va a parsear
 * @return 0 si OK
 * @return -1 ERROR
 */
int userCommandJoin(char *command, SSL *ssl) {
    char *channel, *password, *msgServer;
    channel = password = msgServer = NULL;

    /*Parseamos el comando de usuario*/
    if (IRCUserParse_Join(command, &channel, &password) != IRC_OK) {
        IRCInterface_WriteSystem(NULL, "JOIN <canal> [<password>], entra al canal");
        return -1;
    }

    /*Creamos el mensaje para mandarlo al servidor*/
    if (IRCMsg_Join(&msgServer, NULL, channel, password, NULL) != IRC_OK) {
        IRCInterface_WriteSystem(NULL, "JOIN <canal> [<password>], entra al canal");
        return -1;
    }

    if (sendData(sck, ssl, TCP, NULL, 0, msgServer, strlen(msgServer)) <= 0) {
        IRCInterface_WriteSystemThread(NULL, "Error al conectar con el servidor");
        return -1;
    }
    IRCInterface_PlaneRegisterOutMessage(msgServer);

    IRC_MFree(3, &channel, &password, &msgServer);

    return 0;
}

/***
 * Parsea el comando de usuario nick crea el mensaje para el servidor y lo envia
 * @param m_in mensaje que se va a parsear
 * @return 0 si OK
 * @return -1 ERROR
 */
int userCommandNick(char *command, SSL *ssl) {
    char *newnick, *msgServer;
    newnick = msgServer = NULL;

    /*Parseamos el comando de usuario*/
    if (IRCUserParse_Nick(command, &newnick) != IRC_OK) {
        IRCInterface_WriteSystem(NULL, "/nick <newnick>, cambia el nick");
        return -1;
    }

    /*Creamos el mensaje para mandarlo al servidor*/
    if (IRCMsg_Nick(&msgServer, NULL, newnick, NULL) != IRC_OK) {
        IRCInterface_WriteSystem(NULL, "/nick <newnick>, cambia el nick");
        return -1;
    }

    if (sendData(sck, ssl, TCP, NULL, 0, msgServer, strlen(msgServer)) <= 0) {
        IRCInterface_WriteSystemThread(NULL, "Error al conectar con el servidor");
        return -1;
    }
    IRCInterface_PlaneRegisterOutMessage(msgServer);

    IRC_MFree(2, &newnick, &msgServer);

    return 0;
}

/**
 * Parsea el comando de usuario PRIVMSG crea el mensaje para el servidor y lo envia
 * @param m_in mensaje que se va a parsear
 * @return 0 si OK
 * @return -1 ERROR
 */
int userCommandPrivmsg(char *command, SSL *ssl) {
    char *msg, *msgServer, *nickorchannel;
    int port, ssl_info;
    char *nick, *user, *realname, *password, *server;
    long retval;

    msg = nickorchannel = msgServer = NULL;

    nick = user = realname = password = server = NULL;




    retval = IRCUserParse_Priv(command, &nickorchannel, &msg);
    if (retval != IRC_OK) {
        IRCInterface_WriteSystem(NULL, "PRIVMSG <target> <msg>");
        return -1;
    }

    /*preparamos el mensaje*/
    if (IRCMsg_Privmsg(&msgServer, NULL, nickorchannel, msg) != IRC_OK) {
        IRCInterface_WriteSystem(NULL, "PRIVMSG <target> <msg>");
        return -1;
    }
    if (sendData(sck, ssl, TCP, NULL, 0, msgServer, strlen(msgServer)) <= 0) {
        IRCInterface_WriteSystem(NULL, "Error al conectar con el servidor");
        return -1;
    }
    /*si se ha enviado ok tbn lo sacamos por pantalla*/
    /*comprobamos que exista la pestaña del canal*/
    IRC_ToLower(nickorchannel);
    if (IRCInterface_QueryChannelExist(nickorchannel) == 0)
        IRCInterface_AddNewChannel(nickorchannel, 0);

    IRCInterface_GetMyUserInfo(&nick, &user, &realname, &password,
            &server, &port, &ssl_info);

    IRCInterface_WriteChannel(nickorchannel, nick, msg);
    IRCInterface_PlaneRegisterOutMessage(msgServer);

    IRC_MFree(5, &nick, &user, &realname, &password, &server);
    IRC_MFree(3, &nickorchannel, &msg, &msgServer);
    return 0;
}

/**
 * Parsea el comando de usuario WHOIS crea el mensaje para el servidor y lo envia
 * @param command comando a parsear
 * @return 0 si OK
 * @return -1 ERROR
 */
int userCommandWhois(char *command, SSL *ssl) {
    char *msg, *msgServer;

    long retval;

    msg = msgServer = NULL;

    if (IRCUserParse_Whois(command, &msg) != IRC_OK) {
        IRCInterface_WriteSystem(NULL, "WHOIS <target>");
        return -1;
    }

    /*preparamos el mensaje*/

    if ((retval = IRCMsg_Whois(&msgServer, NULL, NULL, msg)) != IRC_OK) {
        IRCInterface_WriteSystem(NULL, "WHOIS <target>");
        return -1;
    }
    if (sendData(sck, ssl, TCP, NULL, 0, msgServer, strlen(msgServer)) <= 0) {
        IRCInterface_WriteSystem(NULL, "Error al conectar con el servidor");
        return -1;
    }

    IRCInterface_PlaneRegisterOutMessage(msgServer);
    IRC_MFree(2, &msg, &msgServer);
    return 0;
}

/**
 * Parsea el comando de usuario MODE crea el mensaje para el servidor y lo envia
 * @param command comando a parsear
 * @return 0 si OK
 * @return -1 ERROR
 */
int userCommandMode(char *command, SSL *ssl) {
    char *msgServer, *filter, *modeOrChannel, *temp, *channel;

    msgServer = filter = modeOrChannel = temp = channel = NULL;
    
    if (IRCUserParse_Mode(command, &modeOrChannel, &filter) != IRC_OK) {
        IRCInterface_WriteSystem(NULL, "/mode <mode> [<target>]");
        return -1;
    }
    if (modeOrChannel == NULL) {
        /*caso /mode
         en este caso se completa con el canal activo*/
        channel = IRCInterface_ActiveChannelName();
        IRCMsg_Mode(&msgServer, NULL, channel, "", "");
    } else if (modeOrChannel[0] == '#' && !filter) {
        /*caso /mode #otroCanal
         se pregunta por el otro canal*/
        IRCMsg_Mode(&msgServer, NULL, modeOrChannel, "", "");
    } else {
        /*cambiamos el modo a un usuario*/
        channel = IRCInterface_ActiveChannelName();
        IRCMsg_Mode(&msgServer, NULL, channel, modeOrChannel, filter);
    }

    if (sendData(sck, ssl, TCP, NULL, 0, msgServer, strlen(msgServer)) <= 0) {
        IRCInterface_WriteSystem(NULL, "Error al conectar con el servidor");
        return -1;
    }
    IRCInterface_PlaneRegisterOutMessage(msgServer);

    IRC_MFree(3, &modeOrChannel, &filter, &msgServer);
    return 0;
}

/**
 * Parsea el comando de usuario KICK crea el mensaje para el servidor y lo envia
 * @param m_in mensaje que se va a parsear
 * @return 0 si OK
 * @return -1 ERROR
 */
int userCommandKick(char *command, SSL *ssl) {
    char *nick, *msg, *msgServer;
    nick = msg = msgServer = NULL;


    if (IRCUserParse_Kick(command, &nick, &msg) != IRC_OK) {
        IRCInterface_WriteSystem(NULL, "KICK <user>, kickea a un usuario");
        return -1;
    }

    if (IRCMsg_Kick(&msgServer, NULL, IRCInterface_ActiveChannelName(), nick, msg) != IRC_OK) {
        IRCInterface_WriteSystem(NULL, "KICK <user>, kickea a un usuario");
        return -1;
    }

    if (sendData(sck, ssl, TCP, NULL, 0, msgServer, strlen(msgServer)) <= 0) {
        IRCInterface_WriteSystemThread(NULL, "Error al conectar con el servidor");
        return -1;
    }

    IRCInterface_PlaneRegisterOutMessage(msgServer);
    IRC_MFree(3, &nick, &msg, &msgServer);
    return 0;

}

/**
 * Parsea el comando de usuario PART crea el mensaje para el servidor y lo envia
 * @param m_in mensaje que se va a parsear
 * @return 0 si OK
 * @return -1 ERROR
 */
int userCommandPart(char *command, SSL *ssl) {
    char *msg, *msgServer;
    msg = msgServer = NULL;

    if (IRCUserParse_Part(command, &msg) != IRC_OK) {
        IRCInterface_WriteSystem(NULL, "PART <mensaje>, sale del canal actual");
        return -1;
    }

    if (IRCMsg_Part(&msgServer, NULL, IRCInterface_ActiveChannelName(), msg) != IRC_OK) {
        IRCInterface_WriteSystem(NULL, "PART <mensaje>, sale del canal actual");
        return -1;
    }

    if (sendData(sck, ssl, TCP, NULL, 0, msgServer, strlen(msgServer)) <= 0) {
        IRCInterface_WriteSystemThread(NULL, "Error al conectar con el servidor");
        return -1;
    }
    IRCInterface_PlaneRegisterOutMessage(msgServer);
    IRC_MFree(2, &msg, &msgServer);
    return 0;

}

/**
 * Parsea el comando de usuario QUIT crea el mensaje para el servidor y lo envia
 * @param m_in mensaje que se va a parsear
 * @return 0 si OK
 * @return -1 ERROR
 */
int userCommandQuit(char *command, SSL *ssl) {
    char *msg, *msgServer, **channels, message[512];
    int num = 0, i;
    char *nick = NULL, *user = NULL, *realname = NULL, *password = NULL, *server = NULL;
    int port = 0, ssl_info = 0;
    msg = msgServer = NULL;

    if (IRCUserParse_Quit(command, &msg) != IRC_OK) {
        IRCInterface_WriteSystem(NULL, "QUIT [mensaje], deja el servidor");
        return -1;
    }

    if (msg == NULL) {
        if (IRCMsg_Quit(&msgServer, NULL, "Leaving") != IRC_OK) {
            IRCInterface_WriteSystem(NULL, "QUIT [mensaje], deja el servidor");
            return -1;
        }
    } else {
        if (IRCMsg_Quit(&msgServer, NULL, msg) != IRC_OK) {
            IRCInterface_WriteSystem(NULL, "QUIT [mensaje], deja el servidor");
            return -1;
        }
    }

    IRCInterface_GetMyUserInfo(&nick, &user, &realname, &password, &server, &port, &ssl_info);
    IRCInterface_ListAllChannels(&channels, &num);
    sprintf(message, "Disconnected ()");
    IRCInterface_WriteSystem(NULL, message);
    for (i = 0; i < num; i++) {
        IRCInterface_WriteChannel(channels[i], NULL, message);
         IRCInterface_RemoveAllNicksChannel(channels[i]);
    }
    IRCInterface_FreeListAllChannels(channels, num);

    if (sendData(sck, ssl, TCP, NULL, 0, msgServer, strlen(msgServer)) <= 0) {
        IRCInterface_WriteSystemThread(NULL, "Error al conectar con el servidor");
        return -1;
    }

    IRC_MFree(7, &msg, &msgServer, &nick, &user, &realname, &password, &server);
    return 0;

}

/**
 * Parsea el comando de usuario AWAY crea el mensaje para el servidor y lo envia
 * @param m_in mensaje que se va a parsear
 * @return 0 si OK
 * @return -1 ERROR
 */
int userCommandAway(char *command, SSL *ssl) {
    char *msg, *msgServer;
    msg = msgServer = NULL;


    if (IRCUserParse_Away(command, &msg) != IRC_OK) {
        IRCInterface_WriteSystem(NULL, "AWAY <mensaje>, entra en modo away");
        return -1;
    }

    if (IRCMsg_Away(&msgServer, NULL, msg) != IRC_OK) {
        IRCInterface_WriteSystem(NULL, "AWAY <mensaje>, entra en modo away");
        return -1;
    }

    if (sendData(sck, ssl, TCP, NULL, 0, msgServer, strlen(msgServer)) <= 0) {
        IRCInterface_WriteSystemThread(NULL, "Error al conectar con el servidor");
        return -1;
    }

    IRCInterface_PlaneRegisterOutMessage(msgServer);
    IRC_MFree(1, &msgServer);
    return 0;

}

/**
 * Parsea el comando de usuario BACK
 * @param m_in mensaje que se va a parsear
 * @return 0 si OK
 * @return -1 ERROR
 */
int userCommandBack(char *command, SSL *ssl) {
    char *msgServer;
    msgServer = NULL;

    if (IRCMsg_Away(&msgServer, NULL, NULL) != IRC_OK) {
        IRCInterface_WriteSystem(NULL, "AWAY <mensaje>, entra en modo away");
        return -1;
    }

    if (sendData(sck, ssl, TCP, NULL, 0, msgServer, strlen(msgServer)) <= 0) {
        IRCInterface_WriteSystemThread(NULL, "Error al conectar con el servidor");
        return -1;
    }

    IRCInterface_PlaneRegisterOutMessage(msgServer);
    IRC_MFree(1, &msgServer);
    return 0;

}

/**
 * Parsea el comando de usuario TOPIC
 * @param m_in mensaje que se va a parsear
 * @return 0 si OK
 * @return -1 ERROR
 */
int userCommandTopic(char *command, SSL *ssl) {
    char *msgServer, *topic;
    topic = msgServer = NULL;

    if (IRCUserParse_Topic(command, &topic) != IRC_OK) {
        IRCInterface_WriteSystem(NULL, "TOPIC [mensaje], entra en modo away");
        return -1;
    }

    if (IRCMsg_Topic(&msgServer, NULL, IRCInterface_ActiveChannelName(), topic) != IRC_OK) {
        IRCInterface_WriteSystem(NULL, "TOPIC [mensaje], entra en modo away");
        return -1;
    }

    if (sendData(sck, ssl, TCP, NULL, 0, msgServer, strlen(msgServer)) <= 0) {
        IRCInterface_WriteSystemThread(NULL, "Error al conectar con el servidor");
        return -1;
    }

    IRCInterface_PlaneRegisterOutMessage(msgServer);
    return 0;
}

int userCommandPong(char *command, SSL *ssl) {
    char *msg, *msgServer;
    long retval;
    msg = msgServer = NULL;

    if (IRCUserParse_Whois(command, &msg) != IRC_OK) {
        IRCInterface_WriteSystem(NULL, "WHOIS <target>");
        return -1;
    }

    /*preparamos el mensaje*/

    if ((retval = IRCMsg_Whois(&msgServer, NULL, NULL, msg)) != IRC_OK) {
        IRCInterface_WriteSystem(NULL, "WHOIS <target>");
        return -1;
    }
    if (sendData(sck, ssl, TCP, NULL, 0, msgServer, strlen(msgServer)) <= 0) {
        IRCInterface_WriteSystem(NULL, "Error al conectar con el servidor");
        return -1;
    }

    IRCInterface_PlaneRegisterOutMessage(msgServer);
    IRC_MFree(2, &msg, &msgServer);
    return 0;
}
