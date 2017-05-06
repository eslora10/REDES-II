#include "../includes/G-2301-04-P3-redes2.h"

/**
 * Accion por defecto para los mensajes
 * @param m_in mensaje que se va a parsear
 * @return 0 si OK
 * @return -1 ERROR
 */
int msgDefault(char *m_in) {
    char extra[MAXLEN], msg[MAXLEN];
    bzero(msg, MAXLEN);
    sscanf(m_in, ":%[^:]:%[^\r\n]", extra, msg);
    IRCInterface_WriteSystemThread(NULL, msg);
    return -1;
}

/**
 * Comprueba si el prefijo de la respuesta del servidor contiene el nick del
 * usuario que usa la interfaz
 * @param prefix prefijo dentro del mensaje de respuesta del serer
 * @return 0 si los usuarios son distintos, distinto de 0 si iguales
 */
int checkNick(char *prefix) {
    char *origin, *nick, *user, *realname, *password, *server;
    int port, ssl, ret;
    origin = nick = user = realname = password = server = NULL;
    port = ssl = 0;

    /*Obtenemos el nick desde el prefijo*/
    origin = strtok(prefix, "!");
    /*Obtenemos la info del usuario de la interfaz*/
    IRCInterface_GetMyUserInfoThread(&nick, &user, &realname, &password, &server, &port, &ssl);
    ret = strcmp(origin, nick);
    IRC_MFree(5, &nick, &user, &realname, &password, &server);

    return ret;
}

/**
 * Parsea el mensaje de NICK
 * @param m_in mensaje que se va a parsear
 * @return 0 si OK
 * @return -1 ERROR
 */
int msgNickChanged(char *m_in) {

    char *prefix, *new, *old, msg[40];
    char *nick, *user, *realname, *password, *server;
    int port, ssl;

    prefix = new = old = NULL;
    nick = user = realname = password = server = NULL;
    port = ssl = 0;

    IRCInterface_GetMyUserInfoThread(&nick, &user, &realname, &password, &server,
            &port, &ssl);
    IRC_MFree(4, &user, &realname, &password, &server);

    IRCParse_Nick(m_in, &prefix, &old, &new);
    old = strtok(prefix, "!");
    IRCInterface_ChangeNickThread(old, new);


    if (strcmp(old, nick)) {
        sprintf(msg, " %s ahora se conoce como %s", old, new);
    } else {
        /*el usuario es el del cambio de nombre*/
        sprintf(msg, "Ahora eres conocido como %s", new);
    }
    IRCInterface_WriteSystemThread(NULL, msg);

    IRC_MFree(7, &prefix, &new,
            &nick, &user, &realname, &password, &server);
    return 0;
}

/**
 * Parsea el mensaje de JOIN
 * @param m_in mensaje que se va a parsear
 * @return 0 si OK
 * @return -1 ERROR
 */
int msgJoin(char *m_in) {
    char *prefix, *channel, *key, *msg, *msgMode, *msgWho, msgPrint[MAXLEN], *origin;
    prefix = channel = key = msg = msgMode = msgWho = NULL;

    if (IRCParse_Join(m_in, &prefix, &msg, &key, &channel) != IRC_OK)
        return -1;

    /*Hacemos una query del modo al servidor*/
    IRCMsg_Mode(&msgMode, NULL, channel, NULL, NULL);
    send(sck, msgMode, strlen(msgMode), 0);
    IRCInterface_PlaneRegisterOutMessageThread(msgMode);

    /*Hacemos una query de los nicks en el canal al servidor*/
    IRCMsg_Who(&msgWho, NULL, channel, NULL);
    send(sck, msgWho, strlen(msgWho), 0);
    IRCInterface_PlaneRegisterOutMessageThread(msgWho);

    /*El servidor envia un mensaje de join cada vez que un usuario nuevo llega al canal*
     * tenemos que comprobar que el usuario que se une no es el mismo que el que usa la *
     * interfaz*/

    /*Comparamos los nicks, si son iguales creamos la nueva ventana*/
    if (!checkNick(prefix)) {
        sprintf(msgPrint, "Hablando con %s", channel);
        IRC_ToLower(channel);
        IRCInterface_AddNewChannelThread(channel, 0);
        IRCInterface_WriteChannelThread(channel, NULL, msgPrint);
    } else {
        origin = strtok(prefix, "!");
        sprintf(msgPrint, "%s se ha unido", origin);
        IRC_ToLower(channel);
        IRCInterface_WriteChannelThread(channel, NULL, msgPrint);
    }

    IRC_MFree(6, &msgWho, &prefix, &channel, &key, &msg, &msgMode);
    return 0;
}

/**
 * Parsea el mensaje de MODE
 * @param m_in mensaje que se va a parsear
 * @return 0 si OK
 * @return -1 ERROR
 */
int msgMode(char *m_in) {
    char *prefix, *channel, *mode, *nick, msg[MAXLEN];
    long modeInt;
    prefix = channel = mode = nick = NULL;



    if (IRCParse_RplChannelModeIs(m_in, &prefix, &nick, &channel, &mode) != IRC_OK)
        return -1;
    /*Imprimimos el mensaje de mode en la ventana del canal*/
    if (!checkNick(nick)) {
        sprintf(msg, "Canal %s modos: %s", channel, mode);
        IRCInterface_WriteChannelThread(channel, NULL, msg);
    }

    /*Cambiamos el modo canal de la ventana*/
    modeInt = IRCInterface_ModeToIntMode(mode);
    IRC_ToLower(channel);
    IRCInterface_AddModeChannelThread(channel, modeInt);

    IRC_MFree(4, &prefix, &channel, &mode, &nick);
    return 0;
}

/**
 * Parsea el mensaje de NAMES
 * @param m_in mensaje que se va a parsear
 * @return 0 si OK
 * @return -1 ERROR
 */
int msgNames(char *m_in) {
    char *prefix, *channel, *nick, *type, *names, msg[MAXLEN];
    prefix = channel = nick = type = names = NULL;

    if (IRCParse_RplNamReply(m_in, &prefix, &nick, &type, &channel, &names) != IRC_OK)
        return -1;

    /*Immprimimos el mensaje de NAMES en la interfaz*/
    sprintf(msg, "Usuarios en %s: %s", channel, names);
    IRC_ToLower(channel);
    if (!IRCInterface_QueryChannelExistThread(channel))
        IRCInterface_WriteSystemThread(NULL, msg);
    else
        IRCInterface_WriteChannelThread(channel, NULL, msg);

    IRC_MFree(5, &prefix, &nick, &type, &channel, &names);
    return 0;

}

/**
 * Parsea el mensaje de NOTICE
 * @param m_in mensaje que se va a parsear
 * @return 0 si OK
 * @return -1 ERROR
 */
int msgNotice(char *m_in) {
    char *prefix, *msgtarget, *msg;
    prefix = msgtarget = msg = NULL;

    if (IRCParse_Notice(m_in, &prefix, &msgtarget, &msg) != IRC_OK)
        return -1;

    IRCInterface_WriteSystemThread(NULL, msg);

    IRC_MFree(3, &prefix, &msgtarget, &msg);

    return 0;
}

/**
 * Parsea el mensaje de WELCOME
 * @param m_in mensaje que se va a parsear
 * @return 0 si OK
 * @return -1 ERROR
 */
int msgWelcome(char *m_in) {
    char *prefix, *nick, *msg, extra[MAXLEN];

    prefix = nick = msg = NULL;

    if (IRCParse_RplWelcome(m_in, &prefix, &nick, &msg) != IRC_OK)
        return -1;
    IRCInterface_WriteSystemThread(NULL, msg);
    /*Obtenemos el hostname*/
    sscanf(m_in, "%[^@]@%[^\r\n]\r\n", extra, hostName);

    IRC_MFree(3, &prefix, &nick, &msg);

    return 0;
}

/**
 * Parsea el mensaje de YOURHOST
 * @param m_in mensaje que se va a parsear
 * @return 0 si OK
 * @return -1 ERROR
 */
int msgYourHost(char *m_in) {
    char *prefix, *nick, *msg, *servername, *versionname;

    prefix = nick = msg = servername = versionname = NULL;

    if (IRCParse_RplYourHost(m_in, &prefix, &nick, &msg, &servername, &versionname) != IRC_OK)
        return -1;

    IRCInterface_WriteSystemThread(NULL, msg);

    IRC_MFree(5, &prefix, &nick, &msg, &servername, &versionname);

    return 0;
}

/**
 * Parsea el mensaje de CREATED
 * @param m_in mensaje que se va a parsear
 * @return 0 si OK
 * @return -1 ERROR
 */
int msgCreated(char *m_in) {
    char *prefix, *nick, *timedate, *msg;
    prefix = nick = timedate = msg = NULL;

    if (IRCParse_RplCreated(m_in, &prefix, &nick, &timedate, &msg) != IRC_OK)
        return -1;

    IRCInterface_WriteSystemThread(NULL, msg);

    IRC_MFree(4, &prefix, &nick, &timedate, &msg);

    return 0;
}

/**
 * Parsea el mensaje de MYINFO
 * @param m_in mensaje que se va a parsear
 * @return 0 si OK
 * @return -1 ERROR
 */
int msgMyInfo(char *m_in) {
    char *prefix, *nick, *servername, *version, *availableusermodes,
            *availablechannelmodes, *addedg, msg[512];

    prefix = nick = servername = version = availableusermodes =
            availablechannelmodes = addedg = NULL;

    if (IRCParse_RplMyInfo(m_in, &prefix, &nick, &servername, &version,
            &availableusermodes, &availablechannelmodes, &addedg) != IRC_OK)
        return -1;
    sprintf(msg, "Nombre del servidor: %s", servername);
    IRCInterface_WriteSystemThread(NULL, msg);

    sprintf(msg, "Version del servidor: %s", version);
    IRCInterface_WriteSystemThread(NULL, msg);

    sprintf(msg, "Modos de usuario disponibles: %s", availableusermodes);
    IRCInterface_WriteSystemThread(NULL, msg);

    sprintf(msg, "Modos de canal disponibles: %s", availablechannelmodes);
    IRCInterface_WriteSystemThread(NULL, msg);


    IRC_MFree(7, &prefix, &nick, &servername, &version,
            &availableusermodes, &availablechannelmodes, &addedg);

    return 0;

}

/**
 * Parsea el mensaje de ISSUPPORT
 * @param m_in mensaje que se va a parsear
 * @return 0 si OK
 * @return -1 ERROR
 */
int msgIsSupport(char *m_in) {
    char *prefix, *nick, *msg;
    prefix = nick = msg = NULL;

    if (IRCParse_RplISupport(m_in, &prefix, &nick, &msg) != IRC_OK)
        return -1;

    IRCInterface_WriteSystemThread(NULL, msg);

    IRC_MFree(3, &prefix, &nick, &msg);

    return 0;
}

/**
 * Parsea el mensaje de MOTD
 * @param m_in mensaje que se va a parsear
 * @return 0 si OK
 * @return -1 ERROR
 */
int msgMOTDStart(char *m_in) {
    char *prefix, *nick, *msg, *server;
    prefix = nick = msg = server = NULL;

    if (IRCParse_RplMotdStart(m_in, &prefix, &nick, &msg, &server) != IRC_OK)
        return -1;

    IRCInterface_WriteSystemThread(NULL, msg);

    IRC_MFree(4, &prefix, &nick, &msg, &server);

    return 0;
}

/**
 * Parsea el mensaje de MOTD
 * @param m_in mensaje que se va a parsear
 * @return 0 si OK
 * @return -1 ERROR
 */
int msgMOTD(char *m_in) {
    char *prefix, *nick, *msg;
    prefix = nick = msg = NULL;

    if (IRCParse_RplMotd(m_in, &prefix, &nick, &msg) != IRC_OK)
        return -1;

    IRCInterface_WriteSystemThread(NULL, msg);

    IRC_MFree(3, &prefix, &nick, &msg);

    return 0;
}

/**
 * Parsea el mensaje de MOTD
 * @param m_in mensaje que se va a parsear
 * @return 0 si OK
 * @return -1 ERROR
 */
int msgMOTDEnd(char *m_in) {
    char *prefix, *nick, *msg;
    prefix = nick = msg = NULL;

    if (IRCParse_RplEndOfMotd(m_in, &prefix, &nick, &msg) != IRC_OK)
        return -1;

    IRCInterface_WriteSystemThread(NULL, msg);

    IRC_MFree(3, &prefix, &nick, &msg);

    return 0;
}

/**
 * Parsea el mensaje de WHO
 * @param m_in mensaje que se va a parsear
 * @return 0 si OK
 * @return -1 ERROR
 */
int msgWho(char *m_in) {
    char *prefix, *nick, *channel, *user, *host, *server, *nick2, *type, *msg,
            *realname;
    int hopcount = 0;
    nickstate ns;

    prefix = nick = channel = user = host = server = nick2 = type = msg = realname = NULL;

    if (IRCParse_RplWhoReply(m_in, &prefix, &nick, &channel, &user, &host, &server,
            &nick2, &type, &msg, &hopcount, &realname) != IRC_OK)
        return -1;
    if (strchr(type, '@'))
        ns = OPERATOR;
    else if (strchr(type, '+'))
        ns = VOICE;
    else
        ns = NONE;

    IRC_ToLower(channel);
    /*Ponemos los nicks en el canal que corresponde*/
    IRCInterface_AddNickChannelThread(channel, nick2, user, realname, host, ns);

    IRC_MFree(10, &prefix, &nick, &channel, &user, &host, &server, &nick2,
            &type, &msg, &realname);
    return 0;
}

/**
 * Parsea el mensaje de LIST
 * @param m_in mensaje que se va a parsear
 * @return 0 si OK
 * @return -1 ERROR
 */
int msgList(char *m_in) {
    char *prefix, *nick, *channel, *visible, *topic, msg[MAXLEN];
    prefix = nick = channel = visible = topic = NULL;

    /*Parseamos la respuesta del servidor*/
    if (IRCParse_RplList(m_in, &prefix, &nick, &channel, &visible, &topic) != IRC_OK)
        return -1;
    if (topic)
        sprintf(msg, "Canal: %s, tema: %s, número de usuarios: %s", channel, topic, visible);
    else
        sprintf(msg, "Canal: %s, tema: , número de usuarios: %s", channel, visible);
    /*Imprimimos el canal en la ventana de System*/
    IRCInterface_WriteSystemThread(NULL, msg);

    IRC_MFree(5, &prefix, &nick, &channel, &visible, &topic);

    return 0;
}

/**
 * Parsea el mensaje de ENDOFNAMES
 * @param m_in mensaje que se va a parsear
 * @return 0 si OK
 * @return -1 ERROR
 */
int msgEndNames(char *m_in) {
    char *prefix, *nick, *channel, *msg;
    prefix = nick = channel = msg = NULL;

    if (IRCParse_RplEndOfNames(m_in, &prefix, &nick, &channel, &msg) != IRC_OK)
        return -1;

    IRC_ToLower(channel);
    if (!IRCInterface_QueryChannelExistThread(channel))
        IRCInterface_WriteSystemThread(NULL, msg);
    else
        IRCInterface_WriteChannelThread(channel, NULL, msg);

    IRC_MFree(4, &prefix, &nick, &channel, &msg);
    return 0;
}

/**
 * Parsea el mensaje de You're not channel operator
 * @param m_in mensaje que se va a parsear
 * @return 0 si OK
 * @return -1 ERROR
 */
int msgNotOperator(char *m_in) {
    char *prefix, *nick, *channel, *msg, msgPrint[MAXLEN];
    long retval;

    prefix = nick = channel = msg = NULL;

    retval = IRCParse_ErrChanOPrivsNeeded(m_in, &prefix, &nick, &channel, &msg);
    if (retval != IRC_OK && retval != IRCERR_NOFIXEDMATCH) {
        return -1;
    }
    sprintf(msgPrint, "[%s] necesitas ser operador del canal para realizar la operación", nick);
    IRC_ToLower(channel);
    IRCInterface_WriteChannelThread(channel, NULL, msgPrint);

    IRC_MFree(4, &prefix, &nick, &channel, &msg);
    return 0;
}

/**
 * Parsea el mensaje de que se recibe se cambia el modo de un canal
 * @param m_in mensaje que se va a parsear
 * @return 0 si OK
 * @return -1 ERROR
 */

/**
 * Parsea el mensaje de que se recibe se cambia el modo de un canal
 * @param m_in mensaje que se va a parsear
 * @return 0 si OK
 * @return -1 ERROR
 */
int msgModeChanged(char *m_in) {
    char *prefix, *channeluser, *mode, *user, *nick, msgPrint[MAXLEN];
    long modeInt;
    prefix = channeluser = mode = user = NULL;

    /*Parseo del comando*/
    if (IRCParse_Mode(m_in, &prefix, &channeluser, &mode, &user) != IRC_OK)
        return -1;

    /*Conseguimos del prefijo el nick del usuario que ha cambiado el modo*/
    nick = strtok(prefix, "!");
    if (user) {
        /*Se cambia el modo del usuario*/
        sprintf(msgPrint, "%s da %s a %s", nick, mode, user);
        if (mode[0] == '-') {
            /*Se quita el modo*/
            IRCInterface_ChangeNickStateChannelThread(channeluser, user, NONE);
        } else {
            /*Se pone el modo*/
            switch (mode[1]) {
                case 'o':
                    IRCInterface_ChangeNickStateChannelThread(channeluser, user, OPERATOR);
                    break;
                case 'v':
                    IRCInterface_ChangeNickStateChannelThread(channeluser, user, VOICE);
                    break;
                case 'b':
                    /*Usuario baneado, se intenta eliminar del canal*/
                    /*No estoy segura*/
                    IRCInterface_DeleteNickChannelThread(channeluser, user);
                    break;
                default:
                    IRCInterface_ChangeNickStateChannelThread(channeluser, user, NONE);
                    break;
            }
        }
    } else {
        /*Se cambbia el modo del canal*/
        sprintf(msgPrint, "%s pone modo %s a %s", nick, mode, channeluser);
    }
    /*Todos los canales se guardan en minúsculas para evitar problemas*/
    IRC_ToLower(channeluser);
    /*Escribimos en la ventana del canal el mensaje*/
    IRCInterface_WriteChannelThread(channeluser, NULL, msgPrint);

    /*Obtenemos el modo del canal*/
    modeInt = IRCInterface_ModeToIntMode(mode);
    /*Comprobamos si debemos poner un modo nuevo*/
    if (strchr(mode, '+'))
        IRCInterface_AddModeChannelThread(channeluser, modeInt);
        /*En caso contrario eliminamos el modo*/
    else
        IRCInterface_DeleteModeChannelThread(channeluser, modeInt);


    IRC_MFree(4, &prefix, &channeluser, &mode, &user);
    return 0;
}

/**
 * Parsea el mensaje de PRIVMSG
 * @param m_in mensaje que se va a parsear
 * @return 0 si OK
 * @return -1 ERROR
 */
int msgPrivmsg(char *m_in) {
    char *prefix, *target, *msg, *origin;
    prefix = target = msg = NULL;

    if (IRCParse_Privmsg(m_in, &prefix, &target, &msg) != IRC_OK)
        return -1;

    /*conseguimos quien mando el mensaje*/
    origin = strtok(prefix, "!");

    /*Vemos si el mensaje es un FSEND*/
    if (msg[0] == '\001') {
        return fileDialog(origin, msg);
    }

    /*mensaje a grupo*/
    if (target[0] == '#') {
        IRC_ToLower(target);
        IRCInterface_WriteChannelThread(target, origin, msg);
    } else {/*mensaje privado*/
        IRC_ToLower(origin);
        if (IRCInterface_QueryChannelExistThread(origin) == 0)
            IRCInterface_AddNewChannelThread(origin, 0);
        IRCInterface_WriteChannelThread(origin, origin, msg);
    }
    IRC_MFree(3, &prefix, &target, &msg);

    return 0;
}

/**
 * Parsea el mensaje de WHOISUSER
 * @param m_in mensaje que se va a parsear
 * @return 0 si OK
 * @return -1 ERROR
 */
int msgWhoisUser(char *m_in) {
    char *prefix, *nick, *nick2, *name, *host, *realname;
    char msg[512];
    prefix = NULL;

    if (IRCParse_RplWhoIsUser(m_in, &prefix, &nick, &nick2, &name, &host, &realname) != IRC_OK)
        return -1;

    sprintf(msg, "%s(%s@%s) :%s", nick2, name, host, realname);

    IRCInterface_WriteChannelThread(IRCInterface_ActiveChannelName(),
            NULL, msg);

    IRC_MFree(6, &prefix, &nick, &nick2, &name, &host, &realname);

    return 0;
}

/**
 * Parsea el mensaje de WHOISSERVER
 * @param m_in mensaje que se va a parsear
 * @return 0 si OK
 * @return -1 ERROR
 */
int msgWhoisServer(char *m_in) {
    char *prefix, *nick, *nick2, *host, *serverinfo;
    char msg[512];
    prefix = NULL;

    if (IRCParse_RplWhoIsServer(m_in, &prefix, &nick, &nick2, &host, &serverinfo) != IRC_OK)
        return -1;

    sprintf(msg, "[%s] %s:%s", nick2, host, serverinfo);

    IRCInterface_WriteChannelThread(IRCInterface_ActiveChannelName(), NULL, msg);

    IRC_MFree(5, &prefix, &nick, &nick2, &host, &serverinfo);

    return 0;
}

/**
 * Parsea el mensaje de WHOISCHANNELS
 * @param m_in mensaje que se va a parsear
 * @return 0 si OK
 * @return -1 ERROR
 */
int msgWhoisChannels(char *m_in) {
    char *prefix, *nick, *nick2, *channellist;
    char msg[512];
    prefix = NULL;

    if (IRCParse_RplWhoIsChannels(m_in, &prefix, &nick, &nick2, &channellist) != IRC_OK)
        return -1;

    sprintf(msg, "[%s] %s", nick2, channellist);

    IRCInterface_WriteChannelThread(IRCInterface_ActiveChannelName(),
            NULL, msg);

    IRC_MFree(4, &prefix, &nick, &nick2, &channellist);

    return 0;
}

/**
 * Parsea el mensaje de WHOISIDLE
 * @param m_in mensaje que se va a parsear
 * @return 0 si OK
 * @return -1 ERROR
 */
int msgWhoisMode(char *m_in) {
    char *prefix, *nick, *nick2, *msgRec;
    char msg[512];
    int sec_idle, signon;
    prefix = NULL;

    if (IRCParse_RplWhoIsIdle(m_in, &prefix, &nick, &nick2, &sec_idle, &signon, &msgRec) != IRC_OK)
        /* IRCParse_RplUModeIs (char *strin, char **prefix, char **nick, char **usermodestring)*/
        return -1;

    sprintf(msg, "inactividad %s", IRCTAD_TimestampToGMTDate((long) sec_idle));

    IRCInterface_WriteChannelThread(IRCInterface_ActiveChannelName(),
            NULL, msg);

    IRC_MFree(4, &prefix, &nick, &nick2, &msgRec);

    return 0;
}

/**
 * Parsea el mensaje de WHOISIDLE
 * @param m_in mensaje que se va a parsear
 * @return 0 si OK
 * @return -1 ERROR
 */
int msgWhoisIdle(char *m_in) {
    char *prefix, *nick, *nick2, *msgRec;
    char msg[512];
    int sec_idle, signon;
    prefix = NULL;

    if (IRCParse_RplWhoIsIdle(m_in, &prefix, &nick, &nick2, &sec_idle, &signon, &msgRec) != IRC_OK)
        return -1;

    sprintf(msg, "[%s] inactividad %s", nick2, IRCTAD_TimestampToGMTDate((long) sec_idle));

    IRCInterface_WriteChannelThread(IRCInterface_ActiveChannelName(),
            NULL, msg);

    IRC_MFree(4, &prefix, &nick, &nick2, &msgRec);

    return 0;
}

/**
 * Parsea el mensaje de WHOISEND
 * @param m_in mensaje que se va a parsear
 * @return 0 si OK
 * @return -1 ERROR
 */
int msgWhoisEnd(char *m_in) {
    char *prefix, *nick, *name, *msgRec;
    char msg[512];
    prefix = NULL;

    if (IRCParse_RplEndOfWhoIs(m_in, &prefix, &nick, &name, &msgRec) != IRC_OK)
        return -1;

    sprintf(msg, "%s", msgRec);

    IRCInterface_WriteChannelThread(IRCInterface_ActiveChannelName(),
            NULL, msg);

    IRC_MFree(4, &prefix, &nick, &name, &msgRec);

    return 0;
}

/**
 * Parsea el mensaje de Ping enviado por el servidor Y ENVIA EL PONG
 * @param m_in mensaje que se va a parsear
 * @return 0 si OK
 * @return -1 ERROR
 */
int msgPing(char *m_in) {
    char *prefix, *server, *server2, *msgRec, *msgServer;
    prefix = server = server2 = msgRec = msgServer = NULL;

    if (IRCParse_Ping(m_in, &prefix, &server, &server2, &msgRec) != IRC_OK)
        return -1;
    /*enviamos el pong*/
    if (IRCMsg_Pong(&msgServer, NULL, server, NULL, "") != IRC_OK) {
        IRCInterface_WriteSystemThread(NULL, "PONG");
        return -1;
    }

    if (send(sck, msgServer, strlen(msgServer), 0) <= 0) {
        IRCInterface_WriteSystemThread(NULL, "Error al conectar con el servidor");
        return -1;
    }

    IRCInterface_PlaneRegisterOutMessageThread(msgServer);

    IRC_MFree(4, &prefix, &server, &server2, &msgRec, &msgServer);

    return 0;
}

/**
 * Parsea el mensaje de Ping enviado por el servidor Y ENVIA EL PONG
 * @param m_in mensaje que se va a parsear
 * @return 0 si OK
 * @return -1 ERROR
 */
int msgTopicChanged(char *m_in) {
    char *prefix, *channel, *topic, *nick, msgPrint[MAXLEN];
    prefix = channel = topic = nick = NULL;

    /*Parseo del mensaje*/
    if (IRCParse_Topic(m_in, &prefix, &channel, &topic) != IRC_OK)
        return -1;

    /*Obtenemos el nick del usuario que cambia el topic*/
    nick = strtok(prefix, "!");
    sprintf(msgPrint, "%s ha cambiado el tema a: %s", nick, topic);

    /*Cambiamos el topic*/
    IRCInterface_SetTopicThread(topic);

    /*Escribimos en la ventana el mensaje*/
    IRC_ToLower(channel);
    IRCInterface_WriteChannelThread(channel, NULL, msgPrint);

    IRC_MFree(3, &prefix, &channel, &topic);
    return 0;
}

/**
 * Parsea el mensaje de NO NICK OR CHANNEL
 * @param m_in mensaje que se va a parsear
 * @return 0 si OK
 * @return -1 ERROR
 */
int msgNoNickOrChannel(char *m_in) {
    /*num 128*/
    char *prefix, *nick, *channelName, *msg;
    long retval;

    prefix = nick = channelName = msg = NULL;


    /*Parseo del mensaje*/
    retval = IRCParse_ErrNoSuchNick(m_in, &prefix, &nick, &channelName, &msg);
    if (retval != IRCERR_NOFIXEDMATCH && retval != IRC_OK) {
        /*IRC_perror("error", retval);*/

        return -1;
    }

    /*cerramos el canal se ha abierto privmsg, esto se hace por defecto asi que
      debe cerrarse ahora, al darnos el fallo de que no existe ese nick o canal*/

    IRC_ToLower(channelName); /*supongo que el nickname es el chanel name*/
    IRCInterface_RemoveChannelThread(channelName);

    IRC_MFree(4, &prefix, &nick, &channelName, &msg);
    return 0;

}
/**
 * Parsea el mensaje de Kick
 * @param m_in mensaje que se va a parsear
 * @return 0 si OK
 * @return -1 ERROR
 */
int msgKick(char *m_in) {
    char *nick, msg[512], *prefix, *channel, *comment, *origin;
    char *kicked = NULL, *user = NULL, *realname = NULL, *password = NULL, *server = NULL;
    int port = 0, ssl = 0;
    nick = prefix = channel = comment = origin = NULL;

    if (IRCParse_Kick(m_in, &prefix, &channel, &nick, &comment) != IRC_OK)
        return -1;

    origin = strtok(prefix, "!");
    IRCInterface_GetMyUserInfoThread(&kicked, &user, &realname, &password, &server, &port, &ssl);

    if (strcmp(kicked, nick) == 0) {
	if (comment == NULL)
        	sprintf(msg, "You have been kicked from %s by %s (%s)", channel, origin, nick);
	else
		sprintf(msg, "You have been kicked from %s by %s (%s)", channel, origin, comment);
        //IRCInterface_RemoveAllNicksChannelThread(channel);
    } else {
	if (comment == NULL)
        	sprintf(msg, "%s has kicked %s from %s (%s)", kicked, nick, channel, kicked);
	else
		sprintf(msg, "%s has kicked %s from %s (%s)", kicked, nick, channel, comment);
	IRCInterface_DeleteNickChannelThread(channel, nick);
	}

    IRCInterface_WriteChannelThread(IRCInterface_ActiveChannelName(), NULL, msg);
    IRC_MFree(9, &prefix, &channel, &nick, &comment, &kicked, &user, &realname, &password, &server);
    return 0;
}

/**
 * Parsea el mensaje de Part
 * @param m_in mensaje que se va a parsear
 * @return 0 si OK
 * @return -1 ERROR
 */
int msgPart(char *m_in) {
    char msg[512], *prefix, *channel, *comment, *origin;
    char *nick = NULL, *user = NULL, *realname = NULL, *password = NULL, *server = NULL;
    int port = 0, ssl = 0;
    prefix = channel = comment = NULL;

    if (IRCParse_Part(m_in, &prefix, &channel, &comment) != IRC_OK)
        return -1;

    origin = strtok(prefix, "!");
    IRCInterface_GetMyUserInfoThread(&nick, &user, &realname, &password, &server, &port, &ssl);

    if (strcmp(origin, nick) == 0) {
        sprintf(msg, "You have left channel %s (%s)", channel, comment);
        IRCInterface_RemoveChannelThread(channel);
    } else {
        sprintf(msg, "%s (%s) has left %s (%s)", origin, strtok(NULL, "!"), channel, comment);
        IRCInterface_DeleteNickChannelThread(channel, origin);
    }

    IRCInterface_WriteChannelThread(channel, NULL, msg);

    IRC_MFree(8, &prefix, &channel, &comment, &nick, &user, &realname, &password, &server);
    return 0;

}

/**
 * Parsea el mensaje de Quit
 * @param m_in mensaje que se va a parsear
 * @return 0 si OK
 * @return -1 ERROR
 */
int msgQuit(char *m_in) {
    char msg[512], *prefix, *comment, *origin, **channels;
    int num = 0, i;
    prefix = comment = NULL;

    if (IRCParse_Quit(m_in, &prefix, &comment) != IRC_OK)
        return -1;

    origin = strtok(prefix, "!");
    sprintf(msg, "%s has quit (%s)", origin, comment);
    IRCInterface_WriteChannelThread(IRCInterface_ActiveChannelName(), NULL, msg);

    IRCInterface_ListAllChannels(&channels, &num);
    for (i = 0; i < num; i++)
        IRCInterface_DeleteNickChannelThread(channels[i], origin);

    IRCInterface_FreeListAllChannelsThread(channels, num);

    IRC_MFree(2, &prefix, &comment);
    return 0;

}


/**
 * Parsea el mensaje de Away
 * @param m_in mensaje que se va a parsear
 * @return 0 si OK
 * @return -1 ERROR
 */
int msgAway(char *m_in) {
    char msg[512];

    sprintf(msg, "You have been marked as being away");
    IRCInterface_WriteSystemThread(NULL, msg);


    return 0;
}

/**
 * Parsea el mensaje de Back (UNAWAY)
 * @param m_in mensaje que se va a parsear
 * @return 0 si OK
 * @return -1 ERROR
 */
int msgBack(char *m_in) {
    char msg[512];

    sprintf(msg, "You are no longer marked as being away");
    IRCInterface_WriteSystemThread(NULL, msg);


    return 0;
}

/**
 * Parsea el mensaje de Away despues de mandar un privmsg (NOWAWAY)
 * @param m_in mensaje que se va a parsear
 * @return 0 si OK
 * @return -1 ERROR
 */
int msgrplAway(char *m_in) {
    char *prefix, *message, *nick, *nick2, msg[512];
    prefix = message = nick = nick2 = NULL;

    if (IRCParse_RplAway(m_in, &prefix, &nick, &nick2, &message) != IRC_OK)
        return -1;

    sprintf(msg, "[%s] is away (%s)", nick2, message);
    IRCInterface_WriteChannelThread(IRCInterface_ActiveChannelName(), NULL, msg);

    return 0;
}
