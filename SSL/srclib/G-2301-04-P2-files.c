#include "../includes/G-2301-04-P3-redes2.h"

void* fileReceiver(void *args) {
    FILE *fd;
    char *buffer;
    int len, remain_data;
    /*Creamos el fichero de recepción*/
    printf("Path: %s\n", ((*(struct fileReceiver_args *) args).path));
    fd = fopen((*(struct fileReceiver_args *) args).path, "w");
    if (!fd) {
        close((*(struct fileReceiver_args *) args).sckF);
        IRCInterface_WriteSystemThread(NULL, "Error en la recepcion del fichero.1");
        pthread_exit(NULL);
    }
    buffer = (char*) malloc(MAX_TCP * sizeof (char));
    if (!buffer) {
        fclose(fd);
        close((*(struct fileReceiver_args *) args).sckF);
        IRCInterface_WriteSystemThread(NULL, "Error en la recepcion del fichero.2");
        pthread_exit(NULL);
    }
    /*recibimos la informacion del socket*/
    bzero(buffer, MAX_TCP);
    len = 0;
    remain_data = (*(struct fileReceiver_args *) args).length;
    while (((len = recv((*(struct fileReceiver_args *) args).sckF,
            buffer, MAX_TCP, 0)) > 0) && (remain_data > 0)) {
        fwrite(buffer, sizeof (char), len, fd);
        remain_data -= len;
        bzero(buffer, MAX_TCP);
    }

    /*Cerramos todo y liberamos memoria*/
    close((*(struct fileReceiver_args *) args).sckF);
    fclose(fd);
    free(buffer);
    pthread_exit(NULL);
}

int fileDialog(char *nick, char *msg) {
    char ip[64], filename[64], fsend[6], path[66];
    long unsigned int len;
    int port, sckF;
    struct fileReceiver_args *args;
    pthread_t thread;

    args = (struct fileReceiver_args *) malloc(sizeof (struct fileReceiver_args));
    if (!args) {
        IRCInterface_WriteSystemThread(NULL, "Error en la recepcion del fichero.4");
        return -1;
    }
    /*Parseamos la cadena*/
    sscanf(msg, "%s %s %s %d %lu", fsend, filename, ip, &port, &len);
    /*Sacamos la ventana de recepción*/
    if (!IRCInterface_ReceiveDialogThread(nick, filename))
        return 0;
    /*Se ha aceptado la recepción*/
    /*Abrimos un socket para la recepcion*/
    sckF = openSocket(TCP);
    if (sckF < 0) {
        IRCInterface_WriteSystemThread(NULL, "Error en la recepcion del fichero.5");
        return -1;
    }
    printf("Puerto: %d\n", port);
    printf("Direccion: %s\n", ip);
    /*Nos conectamos al emisor*/
    if (connectClientSocket(sckF, "192.168.0.66", port) < 0) {
        close(sckF);
        IRCInterface_WriteSystemThread(NULL, "Error en la recepcion del fichero.6");
        return -1;
    }
    /*Creamos un hilo para conectarse al emisor y recibir el fichero*/
    sprintf(path, "./%s", filename);
    args->length = len;
    args->path = (char*) malloc((strlen(path) + 1) * sizeof (char));
    if (!args->path) {
        IRCInterface_WriteSystemThread(NULL, "Error en la recepcion del fichero.8");
        return -1;
    }
    strcpy(args->path, path);
    args->sckF = sckF;
    if (pthread_create(&thread, NULL, fileReceiver, (void *) args) < 0) {
        close(sckF);
        IRCInterface_WriteSystemThread(NULL, "Error en la recepcion del fichero.7");
        return -1;
    }

    pthread_detach(thread);
    return 0;
}

void* fileSender(void *fs) {
    struct timeval tv;
    fd_set rfds;
    int s = (*(struct fileSender_args*) fs).sck, cl_sck, offset = 0, sent, nbytes;

    /*ponemos el timeout a 30s*/
    tv.tv_sec = 30;
    tv.tv_usec = 0;
    FD_ZERO(&rfds);
    FD_SET(s, &rfds);
    /*Hacemos el accept*/
    if (select(s + 1, &rfds, NULL, NULL, &tv) > 0) {
        nbytes = (*(struct fileSender_args*) fs).length;
        /*El receptor ha aceptado el envio, enviamos el fichero*/
        cl_sck = acceptSocket(s);
        while ((sent = send(cl_sck, (*(struct fileSender_args*) fs).data + offset,
                nbytes, 0)) > 0 || (sent == -1 && errno == EINTR)) {
            if (sent > 0) {
                offset += sent;
                nbytes -= sent;
            }
        }
    } else {
        /*POSIBLEMENTE CAMBIAR POR UN ERROR DIALOG*/
        IRCInterface_WriteSystem(NULL, "Ha saltado el timeout esperando la recepcion del fichero");
        close(s);
        free(fs);
        pthread_exit(NULL);
    }
    free(fs);
    close(cl_sck);
    close((*(struct fileSender_args*) fs).sck);
    pthread_exit(NULL);
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
            IRCInterface_PlaneRegisterInMessage(command);
            if (fun != IRCERR_NOCOMMAND && fun != IRCERR_NOPARAMS && fun != IRCERR_UNKNOWNCOMMAND)
                Messages[fun](command);
            IRC_MFree(1, &command);
        } while (pBuffer);

    }

    pthread_exit(NULL);

}


