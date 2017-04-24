/**
* Modulo de sockets. Contiene funciones de alto nivel para el manejo de
* sockets así como la función que manejan los hilos y la que daemoniza.
* @author Antonio Amor Mourelle &lt;antonio.amor@estudiante.uam.es&gt;
* @author Esther Lopez Ramos &lt;esther.lopezramos@estudiante.uam.es&gt;
* @author Mario Santiago Yepes &lt;mario.santiagoy@estudiante.uam.es&gt;
* @file G-2301-04-P1-socket.h
*/

#include "G-2301-04-P1-socket.h"
#include "G-2301-04-P1-command.h"

/*funcion interna*/
/**
 * @brief Inicia la conexion con el cliente. Espera los comandos
 * NICK, USER en este orden, si no se cierra la conexion con el cliente
 * @param cl_sck identificador de fichero con el socket
 * @param pBuffer buffer para la recepcion de mensajes
 * @param nick cadena de caracteres en la que se guardara el nick del
 * nuevo usuario
 * @return 0 si todo ha ido bien, -1 en caso de error
 */
int beginConnection(int cl_sck, char **pBuffer, char **nick);

/**
* @brief Transforma al proceso que la llama en daemon y abre el log del sistema,
* dicho fichero debe ser cerrado posteriormente
*/
void daemonizar() {

    pid_t pid;
    int numFiles, i;

    pid = fork();

    if (pid < 0)
        exit(EXIT_FAILURE);

    if (pid > 0)
        exit(EXIT_SUCCESS);

    /*Cerramos todos los ficheros*/
    numFiles = getdtablesize();
    for (i = 0; i < numFiles; i++) {
        close(i);
    }

    openlog("Server system messages:", LOG_CONS | LOG_PID | LOG_NDELAY, LOG_LOCAL3);

    syslog(LOG_ERR, "Initiating new server.");


    if (setsid() < 0) {
        syslog(LOG_ERR, "Error creating a new SID for the child process.");
        exit(EXIT_FAILURE);
    }

    if ((chdir("/")) < 0) {
        syslog(LOG_ERR, "Error changing the current working directory = \"/\"");
        exit(EXIT_FAILURE);
    }

    return;
}

/**
 * @brief Abre un socket para comunicarse con el servidor
 * @return devuelve el descriptor de fichero del socket,
                  -1 en caso de error
 */
int openSocket() {
    int sck;

    /*Abrimos el socket, STREAM significa que usaremos el protocolo TCP*/
    sck = socket(AF_INET, SOCK_STREAM, 0);
    if (sck < 0)
        return -1;
    return sck;
}

/**
 * @brief Dado un socket notifica al SO de la apertura del mismo
 * @param sck descriptor del socket
 * @param port puerto con el protocolo de nivel de aplicacion
 * @param max_clients numero maximo de clientes esperando a ser atendidos
 * @return 0 si se realiza todo correctamente,
                  -1 en caso de error
 */
int bindSocket(int sck, uint16_t port, int max_clients) {

    struct sockaddr_in addr;

    /*Ponemos todos los bytes de la estructura a 0*/
    memset(&addr, 0, sizeof (struct sockaddr_in));

    /*Rellenamos los campos de la estructura*/
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    /*Llamamos a bind para notificarle al SO del socket*/
    if (bind(sck, (struct sockaddr *) &addr, sizeof (addr)) < 0)
        return -1;

    /*Preparamos el servidor para recibir peticiones de clientes*/
    if (listen(sck, max_clients) < 0)
        return -1;

    return 0;
}

/**
 * @brief Acepta conexiones de clientes
 * @param sck identificador de fichero del socket
 * @return -1 en caso de error, identificador de fichero del socket del cliente
 */
int acceptSocket(int sck) {
    int cl_sck;
    socklen_t len;
    struct sockaddr_in cl_addr;

    len = sizeof (cl_addr);
    /*Llamamos a la funcion acept para recibir paquetes de clientes*/
    cl_sck = accept(sck, (struct sockaddr *) &cl_addr, &len);
    if (cl_sck < 0)
        return -1;
    return cl_sck;
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
 * @brief Dado un socket, conecta con una IP y un puerto especifico.
 * Específica para clientes.
 * @param sck identificador de fichero con el socket
 * @param host_name url o direccion IP del servidor
 * @param port numero de puerto
 * @return 0 si todo ha ido bien, -1 en caso de error
 */
int connectClientSocket(int sck, char* host_name, int port) {
    struct sockaddr_in addr;
    struct hostent *server;

    /*Ponemos a 0 todos los bytes de la estructura*/
    memset(&addr, 0, sizeof (struct sockaddr_in));
    /*Rellenamos la estructura con la informacion del servidor*/
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    /*Obtenemos la direccion del servidor*/
    server = gethostbyname(host_name);
    if (!server)
        return -1;
    memcpy(&addr.sin_addr.s_addr, server->h_addr_list[0], server->h_length);

    /*Nos conectamos al servidor*/
    if (connect(sck, (struct sockaddr *) &addr, sizeof (addr)))
        return -1;

    return 0;

}

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
