/**
* Modulo de comandos. Implementa todos los comandos IRC requeridos por
* el corrector
* @author Antonio Amor Mourelle &lt;antonio.amor@estudiante.uam.es&gt;
* @author Esther Lopez Ramos &lt;esther.lopezramos@estudiante.uam.es&gt;
* @author Mario Santiago Yepes &lt;mario.santiagoy@estudiante.uam.es&gt;
* @file G-2301-04-P1-command.c
*/

#include "../includes/G-2301-04-P3-redes2.h"


/**
 * @brief Inicia la conexion con el cliente. Espera los comandos
 * NICK, USER en este orden, si no se cierra la conexion con el cliente
 * @param cl_sck identificador de fichero con el socket
 * @param pBuffer buffer para la recepcion de mensajes
 * @param nick cadena de caracteres en la que se guardara el nick del
 * nuevo usuario
 * @return 0 si todo ha ido bien, -1 en caso de error
 */
int beginConnection(int cl_sck, char** pBuffer, char** nick) {

    char *prefix = NULL, *command = NULL;
    char *nickname = NULL, *username = NULL, *hostname = NULL, *servername = NULL, *realname = NULL, *pass = NULL;
    char *msg = NULL;
    char buffer[MAXLEN];
    long retval, numCommand;
    struct sockaddr_in cl_addr;
    socklen_t len = sizeof(cl_addr);
    char *cl_addName;
    /*Flags control*/
    int nickDone = 0, welcome = 0;

    do {
        /*Esperamos a la llegada de un mensaje*/
        bzero(buffer, MAXLEN);
        recv(cl_sck, buffer, MAXLEN, 0);
        /*se lee el maximo posible, tener cuidado con el envio*/
        *pBuffer = buffer;

        do {
            *pBuffer = IRC_UnPipelineCommands(*pBuffer, &command);
            numCommand = IRC_CommandQuery(command);
            switch (numCommand) {
                /*PASS*/
                case 1:
                    /*La password se devuelve por pass*/
                    retval = IRCParse_Pass(command, &prefix, &pass);

                    /*control errores*/
                    break;

                /*NICK*/
                case 2:
                    /*El nick se devuelve por nick*/
                    retval = IRCParse_Nick(command, &prefix, &nickname, &msg);
                    nickDone = 1;
                    /*control errores*/
                    if (retval!= IRC_OK) {
                        sendErrMsg(retval, cl_sck, "*", "*");
                        nickDone = 0;
                    }
                    if(strlen(nickname)>MAX_NICK){
                        sendErrMsg(IRCERR_INVALIDNICK,cl_sck,"*","*");
                        nickDone= 0;
                        free(nickname);nickname=NULL;
                    }
                    break;

                    /*USER*/
                case 3:
                    if (!nickDone) break;
                    /*Parseamos los datos del comando User*/
                    retval = IRCParse_User(command, &prefix, &username, &hostname, &servername, &realname);
                    /*control errores*/
                    if (retval!=IRC_OK) {
                        sendErrMsg(retval, cl_sck, "*", command);
                        IRC_MFree(1,&nickname);
                        return ERROR;
                    }

                    /*Obtenemos el nombre de servidor del cliente*/
                    getpeername(cl_sck, (struct sockaddr *)&cl_addr, &len);
                    cl_addName = inet_ntoa(cl_addr.sin_addr);
                    /*Creamos el usuario*/
                    retval = IRCTADUser_New(username, nickname, realname, pass, cl_addName, MY_ADDR, cl_sck);
                    welcome = 1;
                    /*control errores*/
                    if (retval == IRCERR_INVALIDNICK) {
                        sendErrMsg(retval, cl_sck, "*", nickname);
                    } else if (retval == IRCERR_INVALIDIP) {
                        sendErrMsg(retval, cl_sck, "*", nickname);
                    } else if (retval != IRC_OK) {
                        sendErrMsg(retval,cl_sck,"*",command);
                    }
                    break;

                    /*OTHERWISE*/
                default:
                    //return numCommand;
                    break;
            }
            /*liberamos command*/
            free(command);
        } while (*pBuffer);

    } while (!welcome);
    IRCMsg_RplWelcome(&command, MY_ADDR, nickname, nickname, username, cl_addName);
    if (send(cl_sck, command, strlen(command) , 0) < 0)
        perror("Error");
    *nick = (char*)malloc((MAX_NICK+1)*sizeof(char));
    if(*nick == NULL)
        return -1;
    strcpy(*nick, nickname);
    IRC_MFree(7, &prefix, &command, &nickname, &username, &hostname, &servername,
            &realname);
    return 0;

}

/**
 * @brief Dado un socket acepta la peticion de un cliente
 * @param sck descriptor del socket
 */
void* attendClientSocket(void *sck) {
    int ret = 0, cl_sck;
    long fun = 0;
    char *buffer = NULL;
    char * pBuffer, *command = NULL;
    char  *nick = NULL;

    cl_sck = *((int*)sck);

    beginConnection(cl_sck, &buffer, &nick);

    if (!buffer) {
        buffer = (char*) malloc(MAXLEN * sizeof (char));
    }

    do {
        bzero(buffer, MAXLEN);
        if (recv(cl_sck, buffer, MAXLEN, 0) <= 0) {
            /*Se ha producido un error o el cliente ha cerrado la conexion*/
            IRCTAD_Quit (nick);
            free(nick);
            free(buffer);
            close(cl_sck);
            pthread_exit(NULL);
        }
        pBuffer = buffer;

        do {
            pBuffer = IRC_UnPipelineCommands(pBuffer, &command);

            fun = IRC_CommandQuery(command);
            if (fun < 0) {
                FuncDefault(command, nick, cl_sck);
            } else {
                ret = Commands[fun](command, nick, cl_sck);
            }
            /*Actualizamos el timestamp de la ultima accion del usuario*/
            IRCTADUser_SetActionTS (0, NULL, nick, NULL);
            free(command);

        } while (pBuffer);
    } while (ret != END_CONNECTION);

    free(buffer);
    free(nick);
    close(cl_sck);

    pthread_exit(NULL);
}

/**
* @brief Funcion de parseo de errores IRC. Coge el codigo de error y envia su
* mensaje correspondiente
* @param errval Codigo de error IRC
* @param sck descriptor de fichero del socket en el que se envia el error
* @param nick nickname del usuario
* @param param parametros adicionales para el mensaje de error
* @return 0 OK, -1 ERROR
*/
int sendErrMsg(long errval, int sck, char *nick, char *param) {
    char *errmsg = NULL;
    switch (errval) {
        case IRCERR_ERRONEUSCOMMAND:
            IRCMsg_ErrNeedMoreParams(&errmsg, MY_ADDR, nick, param);
            break;
        case IRCERR_NICKUSED:
            IRCMsg_ErrNickNameInUse(&errmsg, MY_ADDR, nick, param);
            break;
        case IRCERR_NOSTRING:
            IRCMsg_ErrNeedMoreParams(&errmsg, MY_ADDR, nick, param);
            break;
        case IRCERR_INVALIDCHANNELNAME:
            IRCMsg_ErrNoSuchChannel(&errmsg, MY_ADDR, nick, param);
            break;
        case IRCERR_NOINVITEDUSER:
            IRCMsg_ErrInviteOnlyChan(&errmsg, MY_ADDR, nick, param);
            break;
        case IRCERR_USERSLIMITEXCEEDED:
            IRCMsg_ErrChannelIsFull(&errmsg, MY_ADDR, nick, param);
            break;
        case IRCERR_NOVALIDCHANNEL:
            IRCMsg_ErrNoSuchChannel(&errmsg, MY_ADDR, nick, param);
            break;
        case IRCERR_NOVALIDUSER:
            /*401*/IRCMsg_ErrNoSuchNick(&errmsg, MY_ADDR, nick, param);
            break;
        case IRCERR_BANEDUSERONCHANNEL:
            IRCMsg_ErrBannedFromChan(&errmsg, MY_ADDR, nick, param);
            break;
        case IRCERR_NOENOUGHMEMORY:
            IRCMsg_ErrTooManyChannels(&errmsg, MY_ADDR, nick, param);

        case IRCERR_NONICK:
        case IRCERR_INVALIDNICK:
            /*401*/IRCMsg_ErrNoSuchNick(&errmsg, MY_ADDR, nick, param);
            break;
        case IRCERR_NONAME:
            IRCMsg_ErrNeedMoreParams(&errmsg, MY_ADDR, nick, param);
            break;
        case IRCERR_NOHOST:
            IRCMsg_ErrNeedMoreParams(&errmsg, MY_ADDR, nick, param);
            break;
        case IRCERR_NOREALNAME:
        case IRCERR_INVALIDREALNAME:
            IRCMsg_ErrNeedMoreParams(&errmsg, MY_ADDR, nick, param);
            break;
        case IRCERR_NOSERVER:
            IRCMsg_ErrNeedMoreParams(&errmsg, MY_ADDR, nick, param);
            break;
        case IRCERR_NOSERVERINFO:
            IRCMsg_ErrNeedMoreParams(&errmsg, MY_ADDR, nick, param);
            break;
        case IRCERR_YETINCHANNEL:
            IRCMsg_ErrUserOnChannel(&errmsg, MY_ADDR, nick, nick, param);
            break;
        case IRCERR_NOMODE:
            IRCMsg_ErrNoChanModes(&errmsg, MY_ADDR, nick, param);
            break;
        case IRCERR_ISNOTOPERATOR:
            IRCMsg_ErrChanOPrivsNeeded(&errmsg, MY_ADDR, nick, param);
            break;
        case IRCERR_NOUSERONCHANNEL:
            IRCMsg_ErrUserNotInChannel(&errmsg, MY_ADDR, nick, nick, "");
            break;
        case IRCERR_INVALIDUSER:
            /*401*/IRCMsg_ErrNoSuchNick(&errmsg, MY_ADDR, nick, param);
            break;
        case IRCERR_FAIL:
            IRCMsg_ErrBadChannelKey(&errmsg, MY_ADDR, nick, param);
            break;
        case IRCERR_INVALIDIP:
            IRCMsg_ErrAlreadyRegistred (&errmsg, MY_ADDR, param);
        default:
            IRCMsg_ErrNeedMoreParams(&errmsg, MY_ADDR, nick, param);
            break;


    }
    send(sck, errmsg, strlen(errmsg), 0);

    free(errmsg);
    return 0;
}

/**
 * @brief Esta funcion se utiliza cuando un comando es desconocido por el
 * servidor para notificar al cliente
 * @param command comando que se va a parsear y ejecutar
 * @param nick nickname del usuario que ejecuta el comando
 * @param sck socket en el que se recibio el comando
 * @return -1 en caso de fallo, 0 OK
 */
int FuncDefault(char* command, char* nick, int sck) {
    char *rpl;

    if (IRCMsg_ErrUnKnownCommand(&rpl, MY_ADDR, nick, command) != IRC_OK) {
        return -1;
    }

    send(sck, rpl, strlen(rpl), 0);

    free(rpl);

    return 0;
}

/**
 * @brief Ejecuta el comando USER
 * @param command comando que se va a parsear y ejecutar
 * @param nick nickname del usuario que ejecuta el comando
 * @param sck socket en el que se recibio el comando
 * @return -1 en caso de fallo, 0 OK
 */
int userCommand(char *command, char *nick, int sck) {
    char *errmsg = NULL;

    IRCMsg_ErrAlreadyRegistred(&errmsg, MY_ADDR, nick);
    send(sck, errmsg, strlen(errmsg), 0);

    free(errmsg);

    return 0;
}

/**
 * @brief Ejecuta el comando NICK
 * @param command comando que se va a parsear y ejecutar
 * @param nick nickname del usuario que ejecuta el comando
 * @param sck socket en el que se recibio el comando
 * @return -1 en caso de fallo, 0 OK
 */
int nickCommand(char* command, char* nick, int sck) {
    char *newnick, *msg, *prefix, *errmsg, *rply, *away, *user, *real, *host, *IP;
    long retval;
    long id, creationTS, actionTS;
    id = actionTS = creationTS = 0;
    retval = 0;
    newnick = msg = prefix = errmsg = rply = away = user = real = host = IP = NULL;

    /*Parseamos el commando de usuario para obtener el nuevo nick*/
    if (IRCParse_Nick(command, &prefix, &newnick, &msg) != IRC_OK) {
        IRCMsg_ErrNoNickNameGiven(&errmsg, MY_ADDR, nick);
        send(sck, errmsg, strlen(errmsg), 0);
        free(errmsg);
        return -1;
    }

    /*Cambiamos el nick, buscamos al usuario por el nick que nos pasan como argumento*/
    retval = IRCTADUser_Set(0, NULL, nick, NULL, NULL, newnick, NULL);
    if (retval == IRCERR_NICKUSED) {
        /*Nick en uso*/
        /*Creamos el mensaje de error*/
        IRCMsg_ErrNickNameInUse(&errmsg, MY_ADDR, nick, newnick);
        send(sck, errmsg, strlen(errmsg), 0);
        IRC_MFree(4, &errmsg, &newnick, &prefix, &msg);
        return -1;

    }
    /*Obtenemos lo datos del usuario para crear el complex user*/
    IRCTADUser_GetData(&id, &user, &newnick, &real, &host, &IP, &sck, &creationTS, &actionTS, &away);
    /*Si todo ha ido bien mandamos el mensaje de respuesta de nick*/
    if (prefix) {
        free(prefix);
        prefix = NULL;
    }
    retval = IRC_ComplexUser(&prefix, nick, user, host, MY_ADDR);
    strcpy(nick, newnick);
    IRCMsg_Nick(&rply, MY_ADDR, NULL, newnick);
    send(sck, rply, strlen(rply), 0);
    IRC_MFree(8, &rply, &user, &real, &host, &IP, &away, &prefix, &newnick);

    return 0;
}

/**
 * @brief Ejecuta el comando MODE
 * @param command comando que se va a parsear y ejecutar
 * @param nick nickname del usuario que ejecuta el comando
 * @param sck socket en el que se recibio el comando
 * @return -1 en caso de fallo, 0 OK
 */
int modeCommand(char* command, char* nick, int sck) {
    long retval = 0;
    char *mode, *channel, *prefix, *pass, *rplM;
    char *user, *host, *server, *away, *real, *IP;
    long id, actionTS, creationTS;
    id = actionTS = creationTS = 0;

    pass = host = server = away = real = rplM = channel = prefix = user = mode = IP = NULL;

    /*Parseo del comando*/
    retval = IRCParse_Mode(command, &prefix, &channel, &mode, &pass);
    if (retval != IRC_OK) {
        sendErrMsg(retval, sck, nick, command);
        IRC_MFree(4, &mode, &channel, &prefix, &pass);
        return -1;
    }

    /*Si no aparece el parametro mode el el comando, mostramos el modo del canal
     solicitado*/
    if (!mode) {
        mode = IRCTADChan_GetModeChar(channel);
        if (!mode) {
            sendErrMsg(IRCERR_NOVALIDCHANNEL, sck, nick, channel);
            IRC_MFree(3, &channel, &prefix, &pass);
            return -1;
        }
        retval = IRCMsg_RplChannelModeIs(&rplM, MY_ADDR, nick, channel, mode);
        if (retval != IRC_OK) {
            sendErrMsg(retval, sck, nick, channel);
            IRC_MFree(5, &rplM, &channel, &prefix, &pass, &mode);
            return -1;
        }
        send(sck, rplM, strlen(rplM), 0);
        IRC_MFree(5, &rplM, &channel, &prefix, &pass, &mode);
        return 0;
    }
    /*Cambiamos el modo del canal*/
    retval = IRCTAD_Mode(channel, nick, mode);
    if (retval != IRC_OK) {
        sendErrMsg(retval, sck, nick, channel);
        IRC_MFree(5, &rplM, &channel, &prefix, &pass, &mode);
        return -1;
    }
    /*si el usuario pide cambiar la contraseña del canal lo hacemos*/
    if (strchr(mode, 'k')) {
        retval = IRCTADChan_SetPassword(channel, pass);
        if (retval != IRC_OK) {
            sendErrMsg(retval, sck, nick, channel);
            IRC_MFree(5, &rplM, &channel, &prefix, &pass, &mode);
            return -1;
        }
    }

    /*Obtenemos los datos del usuario para crear el complexuser*/
    retval = IRCTADUser_GetData(&id, &user, &nick, &real, &host,
            &IP, &sck, &creationTS, &actionTS, &away);
    if (retval != IRC_OK) {
        sendErrMsg(retval, sck, nick, "");
        IRC_MFree(9, &user, &real, &host, &IP, &away, &channel, &prefix,
                &pass, &mode);
        return -1;

    }
    free(prefix);
    IRC_ComplexUser(&prefix, nick, user, host, MY_ADDR);

    retval = IRCMsg_Mode(&rplM, prefix, channel, mode, nick);
    if (retval != IRC_OK) {
        sendErrMsg(retval, sck, nick, channel);
        IRC_MFree(10, &rplM, &user, &real, &host, &IP, &away, &channel, &prefix,
                &pass, &mode);
        return -1;
    }
    send(sck, rplM, strlen(rplM), 0);
    IRC_MFree(10, &rplM, &user, &real, &host, &IP, &away, &channel, &prefix,
            &pass, &mode);
    return 0;
}

/**
 * @brief Ejecuta el comando QUIT
 * @param command comando que se va a parsear y ejecutar
 * @param nick nickname del usuario que ejecuta el comando
 * @param sck socket en el que se recibio el comando
 * @return -1 en caso de fallo, 0 OK
 */
int quitCommand(char* command, char* nick, int sck) {
    long retval;
    char *msg = NULL, *errmsg = NULL, *prefix = NULL, *notice = NULL;

    /*Parseo del comando*/
    retval = IRCParse_Quit(command, &prefix, &msg);
    if (retval != IRC_OK) {
        IRCMsg_ErrNeedMoreParams(&errmsg, MY_ADDR, nick, command);
        send(sck, errmsg, strlen(errmsg), 0);
        free(errmsg);
        return -1;
    }

    /*Eliminamos al usuario*/
    IRCTAD_Quit(nick);
    IRCMsg_Notice(&notice, MY_ADDR, nick, "Leaving server...");
    send(sck, notice, strlen(notice), 0);


    IRC_MFree(3, &notice, &prefix, &msg);

    return END_CONNECTION;
}

/**
 * @brief Funcion interna para el comando NAMES. Se encarga de de obtener la
 * lista de nicks en un canal y generar los mensajes de NAMES y END NAMES LIST
 * @param rplN cadena donde se guardara el mensaje de respuesta con los nicks
 * @param rplNE cadeba con el mensaje END NAMES LIST
 * @param chan canal del que se obtiene la lista de nicks
 * @param nick nick del usuario que ejecuta el comando
 * @return 0 OK, -1 ERR
 */
int getNamesMsg(char **rplN, char **rplNE, char *chan, char *nick) {

    long retval = 0, nNicks = 0;
    char *lNicks = NULL, *type = NULL;


    retval = IRCTAD_ListNicksOnChannel(chan, &lNicks, &nNicks);
    if (retval != IRC_OK) {
        /*No hay canal devolvemos el mensaje de END*/
        IRCMsg_RplEndOfNames(rplNE, MY_ADDR, nick, chan);
    } else {
        type = IRCTADChan_GetModeChar(chan);
        IRCMsg_RplNamReply(rplN, MY_ADDR, nick,
                type, chan, lNicks);
        IRCMsg_RplEndOfNames(rplNE, MY_ADDR, nick, chan);
    }

    IRC_MFree(2, &type, &lNicks);

    return 0;
}

/**
 * @brief Ejecuta el comando NAMES
 * @param command comando que se va a parsear y ejecutar
 * @param nick nickname del usuario que ejecuta el comando
 * @param sck socket en el que se recibio el comando
 * @return -1 en caso de fallo, 0 OK
 */
int namesCommand(char* command, char* nick, int sck) {
    long retval, nChan, i;
    char *prefix, *channel, *target, *errmsg, *lNicks, *pChan, *rplN, *rplNE,
            **lChan = NULL;
    rplN = rplNE = lNicks = errmsg = target = channel = prefix = NULL;
    nChan = retval = 0;

    /*Parseo del comando*/
    retval = IRCParse_Names(command, &prefix, &channel, &target);
    if (retval != IRC_OK) {
        IRCMsg_ErrNeedMoreParams(&errmsg, prefix, nick, command);
        send(sck, errmsg, strlen(errmsg), 0);
        free(errmsg);
        return -1;
    }

    /*Si el parametro channel esta presente mandamos la info de los canales indicados*/
    if (channel) {
        /*Se especifican uno o varios canales y tenemos que devolver la lista
         de nombres de todos ellos*/
        pChan = strtok(channel, ",");
        while (pChan) {
            getNamesMsg(&rplN, &rplNE, pChan, nick);
            if (rplN != NULL)
                send(sck, rplN, strlen(rplN), 0);
            send(sck, rplNE, strlen(rplNE), 0);
            IRC_MFree(2, &rplN, &rplNE);
            pChan = strtok(NULL, ",");
        }
    } else {
        /*No se especifica ningun canal y por tanto debemos mandar la lista de
         nombres de todos los canales en el servidor*/
        retval = IRCTADChan_GetList(&lChan, &nChan, NULL);
        if (retval != IRC_OK) {
            IRC_MFree(3, &channel, &target, &errmsg);
            return -1;
        }
        for (i = 0; i < nChan; i++) {
            getNamesMsg(&rplN, &rplNE, lChan[i], nick);
            send(sck, rplN, strlen(rplN), 0);
            send(sck, rplNE, strlen(rplNE), 0);
            IRC_MFree(3, &lChan[i], &rplN, &rplNE);
        }
    }

    IRC_MFree(5, &prefix, &channel, &target, &errmsg, &lChan);

    return 0;

}

/**
 * @brief Funcion interna para el comando NAMES. Se encarga de de obtener la
 * lista de nicks en un canal y generar los mensajes de NAMES y END NAMES LIST
 * @param msgL cadena donde se guarda el mensaje de list
 * @param errmsg en caso de que haya algun error se generara el correspondiente 
 * mensaje
 * @param chan canal del que se obtiene el mensaje
 * @param nick nick del usuario que ejecuta el comando
 * @return 0 OK, -1 ERR
 */
int getListMsg(char **msgL, char **errmsg, char *chan, char *nick){
    long retval, mode;
    char *topic = NULL, numUsers[sizeof (long)];
    retval = mode = 0;
    /*Cogemos el topic del canal*/
    retval = IRCTAD_GetTopic(chan, &topic);
    if (retval != IRC_OK) {
        IRCMsg_ErrNoSuchChannel(errmsg, MY_ADDR, nick, chan);
        return -1;
    }
    /*Cogemos el modo del canal para ver que no es secreto*/
    mode = IRCTADChan_GetModeInt(chan);
    if (((mode & IRCMODE_SECRET) == IRCMODE_SECRET) && 
        IRCTAD_TestUserOnChannel(chan, nick) != IRC_OK) {
        /*El canal es secretoy el usuario no esta en el canal*/
        IRC_MFree(1, &topic);
        return -1;
    }
    sprintf(numUsers, "%ld", IRCTADChan_GetNumberOfUsers(chan));
    if(!topic)
        IRCMsg_RplList(msgL, MY_ADDR, nick, chan, numUsers, "");
    else 
        IRCMsg_RplList(msgL, MY_ADDR, nick, chan, numUsers, topic);

    free(topic);

    return 0;
}

/**
 * @brief Ejecuta el comando LIST
 * @param command comando que se va a parsear y ejecutar
 * @param nick nickname del usuario que ejecuta el comando
 * @param sck socket en el que se recibio el comando
 * @return -1 en caso de fallo, 0 OK
 */
int listCommand(char* command, char* nick, int sck) {
    long retval, numChan, i;
    char *prefix, *channel, *target, *errmsg, *pChan;
    char *msgLS, *msgL, *msgLE, **listChan = NULL;
    pChan = msgLS = msgLE = msgL = errmsg = prefix = channel = target = NULL;
    numChan = retval = 0;

    /*Parseo del comando*/
    retval = IRCParse_List(command, &prefix, &channel, &target);
    if (retval == IRCERR_NOSTRING || retval == IRCERR_ERRONEUSCOMMAND) {
        IRCMsg_ErrNeedMoreParams(&errmsg, prefix, nick, command);
        send(sck, errmsg, strlen(errmsg), 0);
        free(errmsg);
        return -1;
    }

    /*Creacion de los mensajes de inicio y fin de la lista de canales*/
    IRCMsg_RplListStart(&msgLS, MY_ADDR, nick);
    IRCMsg_RplListEnd(&msgLE, MY_ADDR, nick);

    /*Comprobamos si se ha introducido el parametro channel, en cuyo caso solo*
     *damos informacion sobre esos canales*/
    /*El parametro target se usa para comunicacion entre servidores, supondremos*
     * que no se manda nunca*/
    /*TAMBIEN PUEDE RECIBIR UNA LISTA*/
    send(sck, msgLS, strlen(msgLS), 0);
    if (channel) {
        /*Separamos los canales*/
        pChan = strtok(channel, ",");
        while (pChan) {
            getListMsg(&msgL, &errmsg, pChan, nick);
            if(errmsg)
                send(sck, errmsg, strlen(errmsg), 0);
            else if (msgL)
                send(sck, msgL, strlen(msgL), 0);
            IRC_MFree(2, &errmsg, &msgL);
            pChan = strtok(NULL, ",");
        }
    }else {
        IRCTADChan_GetList(&listChan, &numChan, NULL);
        for (i = 0; i < numChan; i++) {
            getListMsg(&msgL, &errmsg, listChan[i], nick);
            if(errmsg)
                send(sck, errmsg, strlen(errmsg), 0);
            else if (msgL)
                send(sck, msgL, strlen(msgL), 0);
            IRC_MFree(3, &errmsg, &msgL, &listChan[i]);        
        }
    }
    send(sck, msgLE, strlen(msgLE), 0);
    IRC_MFree(8, &prefix, &channel, &target, &errmsg, &msgLS, &msgLE,
            &msgL, &listChan);
    return 0;
}

/**
 * @brief realiza el comando PRIVMSG para un solo usuarios
 * @param nickDest nick del usuario al que se le envía el comando
 * @param dest cadena necesaria para saber a que canal se manda el mensaje, NULL si usuario
 * @param prefix prefijo para crear el mensaje
 * @param msg mensaje que se le quiere al nick
 * @return devuelve un codido de error, 0 si ok
 */
long privToUser(char *nickDest, char *dest, char *prefix, char *msg) {
    /*no se liberan los argumentos de la funcion*/
    long retval = 0, id = 0, creationTS = 0, actionTS = 0;
    char *user, *real, *host, *IP, *away;
    user = real = host = IP = away = NULL;
    int sckDest = 0;
    char *rplPrivmsg = NULL;

    /*caso usuario*/
    if (!dest)
        dest = nickDest;

    /*conseguimos el sck del usuario destino*/
    retval = IRCTADUser_GetData(&id, &user, &nickDest, &real, &host, &IP, &sckDest,
            &creationTS, &actionTS, &away);
    if (retval != IRC_OK) {
        IRC_MFree(6, &rplPrivmsg,
                &user, &real, &host, &IP, &away);
        return IRCERR_NONICK;
    }
    /*creamos el mensaje*/
    retval = IRCMsg_Privmsg(&rplPrivmsg, prefix, dest, msg);
    if (retval != IRC_OK) {
        IRC_MFree(6, &rplPrivmsg,
                &user, &real, &host, &IP, &away);
        return retval;
    }
    /*enviamos el mensaje al user destino*/
    send(sckDest, rplPrivmsg, strlen(rplPrivmsg), 0);

    /*variable que controla si el usiario destino esta away*/
    int r = 0;
    if (away) r = 1;
    IRC_MFree(6, &rplPrivmsg,
            &user, &real, &host, &IP, &away);
    return r;
}

/**
 * @brief Ejecuta el comando PRIVMSG
 * @param command comando que se va a parsear y ejecutar
 * @param nick nickname del usuario que ejecuta el comando
 * @param sck socket en el que se recibio el comando
 * @return -1 en caso de fallo, 0 OK
 */
int privmsgCommand(char* command, char * nick, int sck) {
    long retval = 0, num = 0, i = 0;
    char *prefix, *msgtarget, *msg, **users;
    prefix = msgtarget = msg = NULL;
    users = NULL;
    char *rplAway = NULL;

    long id = 0, creationTS = 0, actionTS = 0;
    char *user, *real, *host, *IP, *away;
    user = real = host = IP = away = NULL;

    retval = IRCParse_Privmsg(command, &prefix, &msgtarget, &msg);
    if (retval != IRC_OK) {
        sendErrMsg(retval, sck, nick, command);
        IRC_MFree(1, &prefix);
        return -1;
    }

    if (!prefix) {
        free(prefix);
        prefix = NULL;
    }

    retval = IRCTADUser_GetData(&id, &user, &nick, &real, &host, &IP, &sck,
            &creationTS, &actionTS, &away);
    if (retval != IRC_OK) {
        sendErrMsg(retval, sck, nick, command);
        IRC_MFree(10, &prefix, &msgtarget, &msg, &user, &nick, &real, &host, &IP, &sck, &away);
        return -1;
    }

    IRC_ComplexUser(&prefix, nick, user, host, MY_ADDR);
    IRC_MFree(5, &user, &real, &host, &IP, &away);
    id = creationTS = actionTS = 0;


    /*miramos si es un canal o un user*/
    if (msgtarget[0] == '#') {/*canal*/
        retval = IRCTAD_ListNicksOnChannelArray(msgtarget, &users, &num);
        if (retval == IRCERR_NOVALIDCHANNEL) {
            sendErrMsg(retval, sck, nick, command);
            IRC_MFree(3, &prefix, &msg, &msgtarget);
            return -1;
        }

        if (retval != IRC_OK) {
            sendErrMsg(retval, sck, nick, command);
            IRC_MFree(3, &prefix, &msg, &msgtarget);
            return -1;
        }
        for (i = 0; i < num; i++) {
            /*ponemos como destino el canal*/
            retval = privToUser(users[i], msgtarget, prefix, msg);
            if (retval == 1) {
                int sck2 = 0;
                retval = IRCTADUser_GetData(&id, &user, &users[i], &real, &host, &IP, &sck2,
                        &creationTS, &actionTS, &away);
                retval = IRCMsg_RplAway(&rplAway, MY_ADDR, nick, users[i], away);
                if (retval != IRC_OK) {
                    sendErrMsg(retval, sck, nick, command);
                    IRC_MFree(5, &user, &real, &host, &IP, &away);
                    return -1;
                }
                send(sck, rplAway, strlen(rplAway), 0);
                IRC_MFree(5, &real, &host, &IP, &away, &rplAway);
            }
            free(users[i]);
            users[i] = NULL;
        }
        free(users);


    } else {/*solo un user*/
        /*ponemos como destino el usuario*/
        retval = privToUser(msgtarget, NULL, prefix, msg);
        if (retval == 1) {
            int sck2 = 0;
            retval = IRCTADUser_GetData(&id, &user, &msgtarget, &real, &host, &IP, &sck2,
                    &creationTS, &actionTS, &away);
            retval = IRCMsg_RplAway(&rplAway, MY_ADDR, nick, msgtarget, away);
            if (retval != IRC_OK) {
                sendErrMsg(retval, sck, nick, command);
                IRC_MFree(5, &user, &real, &host, &IP, &away);
                return -1;
            }
            send(sck, rplAway, strlen(rplAway), 0);
            IRC_MFree(5, &real, &host, &IP, &away, &rplAway);
        }
        if (retval == IRCERR_NONICK) {
            sendErrMsg(retval, sck, nick, msgtarget);
            IRC_MFree(3, &msg, &msgtarget, &prefix);
            return -1;
        }
        if (retval != IRC_OK) {
            sendErrMsg(retval, sck, nick, command);
            IRC_MFree(3, &msg, &msgtarget, &prefix);
            return -1;
        }
        IRC_MFree(6, &rplAway, &user, &real, &host, &IP, &away);
    }
    IRC_MFree(3, &prefix, &msg, &msgtarget);

    return 0;

}

/**
 * @brief Ejecuta el comando MOTD
 * @param command comando que se va a parsear y ejecutar
 * @param nick nickname del usuario que ejecuta el comando
 * @param sck socket en el que se recibio el comando
 * @return -1 en caso de fallo, 0 OK
 */
int motdCommand(char* command, char* nick, int sck) {
    char *prefix, *target, *rplS, *rpl, *rplE;
    long retval = 0;
    rplE = rpl = prefix = target = rplS = NULL;

    /*Parseo del comando*/
    retval = IRCParse_Motd(command, &prefix, &target);
    if (retval != IRC_OK) {
        sendErrMsg(retval, sck, nick, command);
        IRC_MFree(2, &prefix, &target);
        return -1;
    }

    retval = IRCMsg_RplMotdStart(&rplS, MY_ADDR, nick, MY_ADDR);
    if (retval != IRC_OK) {
        sendErrMsg(retval, sck, nick, command);
        IRC_MFree(3, &rplS, &prefix, &target);
        return -1;
    }
    send(sck, rplS, strlen(rplS), 0);

    retval = IRCMsg_RplMotd(&rpl, MY_ADDR, nick, "Cual fue la web que creo "
            "Chewbacca por la que le arrestaron?");
    if (retval != IRC_OK) {
        sendErrMsg(retval, sck, nick, command);
        IRC_MFree(4, &rplS, &rpl, &prefix, &target);
        return -1;
    }
    send(sck, rpl, strlen(rpl), 0);
    free(rpl);
    retval = IRCMsg_RplMotd(&rpl, MY_ADDR, nick, "Wookieleaks");
    if (retval != IRC_OK) {
        sendErrMsg(retval, sck, nick, command);
        IRC_MFree(4, &rplS, &rpl, &prefix, &target);
        return -1;
    }
    send(sck, rpl, strlen(rpl), 0);

    retval = IRCMsg_RplEndOfMotd(&rplE, MY_ADDR, nick);
    if (retval != IRC_OK) {
        sendErrMsg(retval, sck, nick, command);
        IRC_MFree(5, &rplE, &rplS, &rpl, &prefix, &target);
        return -1;
    }
    send(sck, rplE, strlen(rplE), 0);

    IRC_MFree(5, &rplE, &rplS, &rpl, &prefix, &target);
    return 0;

}

/**
 * @brief Ejecuta el comando JOIN
 * @param command comando que se va a parsear y ejecutar
 * @param nick nickname del usuario que ejecuta el comando
 * @param sck socket en el que se recibio el comando
 * @return -1 en caso de fallo, 0 OK
 */
int joinCommand(char* command, char* nick, int sck) {
    long retval = 0, actionTS = 0, creationTS = 0, id = 0, nNicks = 0;
    char *prefix, *channel, *key, *msg, *user, *real, *host, *IP, *away, *pChan,
            *rplJ, *rplT, *rplN, *topic, *type, *lNicks;
    lNicks = type = topic = rplJ = rplT = rplN = pChan = user = real = host =
            IP = away = prefix = channel = key = msg = NULL;

    /*Parseo del comando*/
    retval = IRCParse_Join(command, &prefix, &channel, &key, &msg);
    if (retval != IRC_OK) {
        sendErrMsg(retval, sck, nick, command);
        IRC_MFree(4, &prefix, &channel, &key, &msg);
        return -1;
    }

    /*Obtenemos los datos del usuario para crear el complex user*/
    retval = IRCTADUser_GetData(&id, &user, &nick, &real, &host, &IP, &sck, &creationTS,
            &actionTS, &away);
    if (retval != IRC_OK) {
        sendErrMsg(retval, sck, nick, nick);
        IRC_MFree(9, &prefix, &channel, &key, &msg, &user, &real, &host, &IP, &away);
        return -1;
    }
    /*Creamos el complexUser*/
    if (prefix) {
        free(prefix);
        prefix = NULL;
    }
    retval = IRC_ComplexUser(&prefix, nick, user, host, MY_ADDR);

    /*Join puede aceptar una lista de canales, por tanto seperamos los canales*/
    pChan = strtok(channel, ",");
    while (pChan != NULL) {
        if (pChan[0] != '#') {
            /*El canal debe comenzar por #*/
            sendErrMsg(IRCERR_INVALIDCHANNELNAME, sck, nick, pChan);
            pChan = strtok(NULL, ",");
            continue;
        }
        /*Unimos el usuario al canal*/
        retval = IRCTAD_Join(pChan, nick, NULL, key);
        if (retval != IRC_OK) {
            sendErrMsg(retval, sck, nick, pChan);
            pChan = strtok(NULL, ",");
            continue;
        }
        /*Creamos el mensaje de join*/
        IRCMsg_Join(&rplJ, prefix, NULL, NULL, pChan);
        send(sck, rplJ, strlen(rplJ), 0);

        /*Creamos el mensaje de topic*/
        IRCTAD_GetTopic(pChan, &topic);
        if (topic)
            IRCMsg_RplTopic(&rplT, MY_ADDR, nick, pChan, topic);
        else
            IRCMsg_RplTopic(&rplT, MY_ADDR, nick, pChan, "");
        send(sck, rplT, strlen(rplT), 0);

        /*Creamos el mensaje de names*/
        /*Obtenemos el tipo del canal*/
        type = IRCTADChan_GetModeChar(pChan);
        /*Obtenemos la lista de usuarios en el canal*/
        IRCTAD_ListNicksOnChannel(pChan, &lNicks, &nNicks);
        if (type)
            IRCMsg_RplNamReply(&rplN, MY_ADDR, nick, type, pChan, lNicks);
        else
            IRCMsg_RplNamReply(&rplN, MY_ADDR, nick, "", pChan, lNicks);
        send(sck, rplN, strlen(rplN), 0);


        IRC_MFree(6, &rplJ, &rplN, &rplT, &lNicks, &type, &topic);
        pChan = strtok(NULL, ",");
    }


    IRC_MFree(9, &prefix, &channel, &key, &msg, &user, &real, &host, &IP, &away);
    return 0;
}

/**
 * @brief funcion auxiliar que devuelve una cadena con la lista de canales y el modo del usuario en ellos
 * @param chanlist  RETURN lista con los canales y los modos
 * @param user username del usuario del que queremos saber los canales y sus modos
 * @param nick nickname del usuario que ejecuta el comando
 * @return -1 en caso de fallo, 0 OK
 */
long channelsMode(char **chanlist, char *user, char *nick) {
    long i = 0, retval = 0, numChan = 0;
    char *channelslist, *aux, **channelsArray;
    channelslist = aux = NULL;
    channelsArray = NULL;

    retval = IRCTAD_ListChannelsOfUserArray(user, nick, &channelsArray, &numChan);
    if (retval != IRC_OK) {
        return retval;
    }
    if (numChan == 0) {
        channelslist = NULL;
        return 0;
    }

    channelslist = (char*) malloc(sizeof (""));
    strcpy(channelslist, "");

    for (i = 0; i < numChan; i++) {
        retval = IRCTAD_GetUserModeOnChannel(channelsArray[i], nick);
        /*comprobamos caso error*/
        if (retval < 0) {
            free(channelslist);
            return retval;
        }
        /*comprobamos si nick es operador*/
        if ((retval & IRCUMODE_OPERATOR) == IRCUMODE_OPERATOR) {
            asprintf(&aux, "%s @%s", channelslist, channelsArray[i]);
        } else {
            asprintf(&aux, "%s %s", channelslist, channelsArray[i]);
        }
        free(channelslist);
        free(channelsArray[i]);
        asprintf(&channelslist, "%s", aux);
        free(aux);
    }
    free(channelsArray);
    *chanlist = channelslist;
    return 0;
}

/**
 * @brief Ejecuta el comando WHOIS
 * @param command comando que se va a parsear y ejecutar
 * @param nick nickname del usuario que ejecuta el comando
 * @param sck socket en el que se recibio el comando
 * @return -1 en caso de fallo, 0 OK
 */
int whoisCommand(char* command, char* nick, int sck) {
    /*no se tiene en cuenta la comunicacion entre servidores*/

    char *prefix = NULL, *server = NULL, *nicksarray = NULL, *errmsg = NULL;
    char *user = NULL, *real = NULL, *host = NULL, *IP = NULL;
    char *away = NULL;
    char *rplChan = NULL, *rplUser = NULL, *rplServ = NULL, *rplIdle = NULL, *rplAway = NULL, *rplEnd = NULL;
    char *nick2 = NULL, *channelslist = NULL;
    int sck2 = 0;
    long id = 0, retval = 0, actionTS = 0, creationTS = 0;

    /*NO CAMBIAR, LO NECESITA R2*/
    retval = IRCParse_Whois(command, &prefix, &server, &nicksarray);
    if (retval == IRCERR_NOSTRING || retval == IRCERR_ERRONEUSCOMMAND) {
        IRCMsg_ErrNoNickNameGiven(&errmsg, MY_ADDR, nick);
        send(sck, errmsg, strlen(errmsg), 0);
        IRC_MFree(4, &prefix, &errmsg, &server, &nicksarray);
        return -1;
    }

    /*iteramos sobre la lista de nicks que viniese con el command*/
    nick2 = strtok(nicksarray, ",");
    while (nick2) {

        /*Conseguimos los datos de nick2*/
        /*se busca por nick2 para conseguir el user*/
        sck2 = 0;
        id = creationTS = actionTS = 0;
        IRC_MFree(5, &user, &real, &host, &IP, &away);

        /*comprobamos que el usuario exista*/
        retval = IRCTADUser_GetData(&id, &user, &nick2, &real, &host,
                &IP, &sck2, &creationTS, &actionTS, &away);
        if (retval != IRC_OK) {
            sendErrMsg(retval, sck, nick, nick2);
            IRC_MFree(7, &prefix, &server, &user, &real,
                    &host, &IP, &away);
            /*seguimos buscando en los siguientes usuarios*/
            nick2 = strtok(NULL, ",");
            continue;

        }

        /*RPL USER*/
        retval = IRCMsg_RplWhoIsUser(&rplUser, MY_ADDR, nick, nick2, user, host, real);
        if (retval != IRC_OK) {
            sendErrMsg(retval, sck, nick, command);
            IRC_MFree(7, &prefix, &server, &user, &real,
                    &host, &IP, &away);
            /*seguimos buscando en los siguientes usuarios*/
            nick2 = strtok(NULL, ",");
            continue;
        }

        /*RLP SERVER*/
        retval = IRCMsg_RplWhoIsServer(&rplServ, MY_ADDR, nick, nick2, host, "servidor EAM");
        if (retval != IRC_OK) {
            sendErrMsg(retval, sck, nick, command);
            IRC_MFree(7, &prefix, &server, &user, &real,
                    &host, &IP, &away);
            /*seguimos buscando en los siguientes usuarios*/
            nick2 = strtok(NULL, ",");
            continue;
        }
        /*RLP CHANNELS*/

        retval = channelsMode(&channelslist, user, nick2);
        if (retval != IRC_OK) {
            sendErrMsg(retval, sck, nick, command);
            IRC_MFree(7, &prefix, &server, &user, &real,
                    &host, &IP, &away);
            /*seguimos buscando en los siguientes usuarios*/
            nick2 = strtok(NULL, ",");
            continue;
        }
        if (channelslist) {
            retval = IRCMsg_RplWhoIsChannels(&rplChan, MY_ADDR, nick, nick2, channelslist);
            if (retval != IRC_OK) {
                sendErrMsg(retval, sck, nick, command);
                IRC_MFree(8, &prefix, &server, &user, &real,
                        &host, &IP, &away, &channelslist);
                /*seguimos buscando en los siguientes usuarios*/
                nick2 = strtok(NULL, ",");
                continue;
            }
        }

        retval = IRCMsg_RplWhoIsIdle(&rplIdle, MY_ADDR, nick, nick2, actionTS, NULL);
        if (retval != IRC_OK) {
            sendErrMsg(retval, sck, nick, command);
            IRC_MFree(8, &prefix, &server, &user, &real,
                    &host, &IP, &away, &channelslist);
            /*seguimos buscando en los siguientes usuarios*/
            nick2 = strtok(NULL, ",");
            continue;
        }
        /*RPL AWAY*/
        if (away) {
            retval = IRCMsg_RplAway(&rplAway, MY_ADDR, nick, nick2, away);
            if (retval != IRC_OK) {
                sendErrMsg(retval, sck, nick, command);
                IRC_MFree(8, &prefix, &server, &user, &real,
                        &host, &IP, &away, &channelslist);
                /*seguimos buscando en los siguientes usuarios*/
                nick2 = strtok(NULL, ",");
                continue;
            }
        }
        send(sck, rplUser, strlen(rplUser), 0);
        send(sck, rplServ, strlen(rplServ), 0);
        if (rplChan)
            send(sck, rplChan, strlen(rplChan), 0);
        send(sck, rplIdle, strlen(rplIdle), 0);
        if (rplAway)
            send(sck, rplAway, strlen(rplAway), 0);
        IRC_MFree(5, &rplUser, &rplServ, &rplChan, &rplIdle, &channelslist);

        /*seguimos iterando en los usuarios introducidos en el comando*/
        nick2 = strtok(NULL, ",");
    }


    retval = IRCMsg_RplEndOfWhoIs(&rplEnd, MY_ADDR, nick, nicksarray);
    if (retval == IRCERR_NONICK) {
        sendErrMsg(retval, sck, nick, command);
        IRC_MFree(8, &prefix, &server, &user, &real,
                &host, &IP, &away, &nicksarray);
        return -1;
    }
    send(sck, rplEnd, strlen(rplEnd), 0);


    IRC_MFree(10, &prefix, &user, &channelslist, &server,
            &real, &host, &IP, &away, &nicksarray, &rplEnd);
    return 0;
}

/**
 * @brief Ejecuta el comando PING
 * @param command comando que se va a parsear y ejecutar
 * @param nick nickname del usuario que ejecuta el comando
 * @param sck socket en el que se recibio el comando
 * @return -1 en caso de fallo, 0 OK
 */
int pingCommand(char* command, char* nick, int sck) {

    long retval = 0;
    char *prefix, *server, *server2, *msg, *errmsg, *rply;
    prefix = server = server2 = msg = errmsg = rply = NULL;

    retval = IRCParse_Ping(command, &prefix, &server, &server2, &msg);

    if (retval == IRCERR_NOSTRING || retval == IRCERR_ERRONEUSCOMMAND) {
        IRCMsg_ErrNoOrigin(&errmsg, MY_ADDR, nick, command);
        send(sck, errmsg, strlen(errmsg), 0);
        IRC_MFree(5, &prefix, &server, &server2, &msg, &errmsg);
        return -1;
    }
    retval = IRCMsg_Pong(&rply, MY_ADDR, MY_ADDR, server2, server);
    if (retval == IRCERR_NOSERVER) {
        IRCMsg_ErrNoSuchServer(&errmsg, MY_ADDR, nick, command);
        send(sck, errmsg, strlen(errmsg), 0);
        IRC_MFree(5, &prefix, &server, &server2, &msg, &errmsg);
        return -1;
    }

    send(sck, rply, strlen(rply), 0);
    IRC_MFree(5, &prefix, &server, &server2, &msg, &rply);
    return 0;
}

/**
 * @brief Ejecuta el comando TOPIC
 * @param command comando que se va a parsear y ejecutar
 * @param nick nickname del usuario que ejecuta el comando
 * @param sck socket en el que se recibio el comando
 * @return -1 en caso de fallo, 0 OK
 */
int topicCommand(char* command, char* nick, int sck) {
    long retval = 0;
    char *prefix = NULL, *channel = NULL, *errmsg = NULL, *topic = NULL, *rplyT = NULL, *rplyNT = NULL;

    retval = IRCParse_Topic(command, &prefix, &channel, &topic);

    if (retval != IRC_OK) {
        sendErrMsg(retval, sck, nick, command);
        return -1;
    }

    if (topic == NULL) {
        retval = IRCTAD_GetTopic(channel, &topic);
        if (retval != IRC_OK) {
            sendErrMsg(retval, sck, nick, command);
            IRC_MFree(3, &prefix, &channel, &topic);
            return -1;
        } else {
            if (topic)
                IRCMsg_RplTopic(&rplyT, MY_ADDR, nick, channel, topic);
            else
                IRCMsg_RplNoTopic(&rplyNT, MY_ADDR, nick, channel);
        }
    } else if (strcmp(topic, "") == 0) {
        if ((IRCTAD_GetUserModeOnChannel(channel, nick) & IRCUMODE_OPERATOR) != IRCUMODE_OPERATOR) {
            IRCMsg_ErrChanOPrivsNeeded(&errmsg, MY_ADDR, nick, channel);
            send(sck, errmsg, strlen(errmsg), 0);
            IRC_MFree(4, &prefix, &channel, &topic, &errmsg);
            return -1;
        } else {
            retval = IRCTAD_DeleteTopic(channel, nick);
            if (retval != IRC_OK) {
                sendErrMsg(retval, sck, nick, command);
                IRC_MFree(3, &prefix, &channel, &topic);
                return -1;
            } else
                IRCMsg_RplNoTopic(&rplyNT, MY_ADDR, nick, channel);
        }
    } else {
        if ((IRCTAD_GetUserModeOnChannel(channel, nick) & IRCUMODE_OPERATOR) != IRCUMODE_OPERATOR) {
            IRCMsg_ErrChanOPrivsNeeded(&errmsg, MY_ADDR, nick, channel);
            send(sck, errmsg, strlen(errmsg), 0);
            IRC_MFree(4, &prefix, &channel, &topic, &errmsg);
            return -1;
        } else {
            retval = IRCTAD_SetTopic(channel, nick, topic);
            if (retval != IRC_OK) {
                sendErrMsg(retval, sck, nick, command);
                IRC_MFree(3, &prefix, &channel, &topic);
                return -1;
            } else
                IRCMsg_Topic(&rplyT, MY_ADDR, channel, topic);
        }

    }

    if (rplyT)
        send(sck, rplyT, strlen(rplyT), 0);
    if (rplyNT)
        send(sck, rplyNT, strlen(rplyNT), 0);
    IRC_MFree(5, &prefix, &channel, &topic, &rplyNT, &rplyT);
    return 0;

}

/**
 * @brief Ejecuta el comando PART
 * @param command comando que se va a parsear y ejecutar
 * @param nick nickname del usuario que ejecuta el comando
 * @param sck socket en el que se recibio el comando
 * @return -1 en caso de fallo, 0 OK
 */
int partCommand(char* command, char* nick, int sck) {
    long retval = 0;
    char *prefix = NULL, *channel = NULL, *msg = NULL, *pChan = NULL, *rply = NULL;

    retval = IRCParse_Part(command, &prefix, &channel, &msg);
    if (retval != IRC_OK) {
        sendErrMsg(retval, sck, nick, command);
        IRC_MFree(3, &prefix, &channel, &msg);
        return -1;
    } else {
        pChan = strtok(channel, ",");
        while (pChan != NULL) {
            retval = IRCTAD_Part(pChan, nick);
            if (retval != IRC_OK) {
                sendErrMsg(retval, sck, nick, pChan);
                pChan = strtok(NULL, ",");
                continue;
            } else
                IRCMsg_Part(&rply, MY_ADDR, pChan, msg);
            send(sck, rply, strlen(rply), 0);
            free(rply);
            pChan = strtok(NULL, ",");
        }
    }

    IRC_MFree(3, &prefix, &channel, &msg);
    return 0;
}

/**
 * @brief Ejecuta el comando KICK
 * @param command comando que se va a parsear y ejecutar
 * @param nick nickname del usuario que ejecuta el comando
 * @param sck socket en el que se recibio el comando
 * @return -1 en caso de fallo, 0 OK
 */
int kickCommand(char* command, char* nick, int sck) {
    long retval = 0, id = 0, creationTS = 0, actionTS = 0;
    int sck_kicked = 0;
    char *prefix = NULL, *channel = NULL, *comment = NULL, *errmsg = NULL, *user = NULL, *rply = NULL;
    char *nick_kicked = NULL, *away = NULL, *real = NULL, *host = NULL, *IP = NULL;


    retval = IRCParse_Kick(command, &prefix, &channel, &user, &comment);
    if (retval != IRC_OK) {
        sendErrMsg(retval, sck, nick, command);
        IRC_MFree(4, &prefix, &channel, &user, &comment);
        return -1;
    } else {
        if ((IRCTAD_GetUserModeOnChannel(channel, nick) & IRCUMODE_OPERATOR) != IRCUMODE_OPERATOR) {
            IRCMsg_ErrChanOPrivsNeeded(&errmsg, MY_ADDR, nick, channel);
            send(sck, errmsg, strlen(errmsg), 0);
            IRC_MFree(5, &prefix, &channel, &user, &comment, &errmsg);
            return -1;
        } else {
            IRCTADUser_GetData(&id, &user, &nick_kicked, &real, &host, &IP, &sck_kicked, &creationTS, &actionTS, &away);
            retval = IRCTAD_KickUserFromChannel(channel, nick_kicked);
            if (retval != IRC_OK) {
                sendErrMsg(retval, sck_kicked, nick_kicked, command);
                IRC_MFree(9, &prefix, &channel, &user, &comment, &nick_kicked, &real, &host, &IP, &away);
                return -1;
            }
            IRCMsg_Kick(&rply, MY_ADDR, channel, user, comment);
            send(sck_kicked, rply, strlen(rply), 0);
            IRC_MFree(10, &prefix, &channel, &user, &comment, &nick_kicked, &real, &host, &IP, &away, &rply);
            return 0;
        }
    }
}

/**
 * @brief Ejecuta el comando AWAY
 * @param command comando que se va a parsear y ejecutar
 * @param nick nickname del usuario que ejecuta el comando
 * @param sck socket en el que se recibio el comando
 * @return -1 en caso de fallo, 0 OK
 */
int awayCommand(char* command, char* nick, int sck) {
    long retval = 0, id = 0, creationTS = 0, actionTS = 0;
    char *prefix = NULL, *msg = NULL, *rply = NULL, *user = NULL, *real = NULL;
    char *away = NULL, *host = NULL, *IP = NULL;

    retval = IRCParse_Away(command, &prefix, &msg);
    if (retval != IRC_OK) {
        sendErrMsg(retval, sck, nick, command);
        IRC_MFree(2, &prefix, &msg);
        return -1;
    } else {
        IRCTADUser_GetData(&id, &user, &nick, &real, &host, &IP, &sck, &creationTS, &actionTS, &away);
        if (msg != NULL) {
            retval = IRCTADUser_SetAway(id, user, nick, real, msg);
            if (retval != IRC_OK) {
                sendErrMsg(retval, sck, nick, command);
                IRC_MFree(7, &prefix, &msg, &user, &real, &host, &IP, &away);
                return -1;
            }
            IRCMsg_RplNowAway(&rply, MY_ADDR, nick);
        } else {
            retval = IRCTADUser_SetAway(id, user, nick, real, NULL);
            if (retval != IRC_OK) {
                sendErrMsg(retval, sck, nick, command);
                IRC_MFree(7, &prefix, &msg, &user, &real, &host, &IP, &away);
                return -1;
            }
            IRCMsg_RplUnaway(&rply, MY_ADDR, nick);
        }
        send(sck, rply, strlen(rply), 0);
        IRC_MFree(8, &prefix, &msg, &user, &real, &host, &IP, &away, &rply);
        return 0;
    }
}
