#ifndef FILES_H
#define FILES_H

struct fileReceiver_args {
    int sckF;
    char *path;
    long unsigned int length;
};


struct fileSender_args {
    char *data;
    long unsigned int length;
    int sck;
};

void* fileReceiver(void *args);
int fileDialog(char *nick, char *msg);

void* fileSender(void *fs);
/**
 * Funcion manejada por los hilos que se encarga de recibir y parsear las
 * respuestas del servidor
 * @param sck descriptor de fichero del socket en el que se reciben las respuestas
 * del server
 * @return NULL en caso de error
 * @return en caso de correcto funcionamiento el hilo se cierra
 */
void* recvInfo(void* sck);

#endif /*FILES_H*/
