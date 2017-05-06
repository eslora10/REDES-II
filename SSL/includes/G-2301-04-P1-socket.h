#ifndef SOCKET_H
#define SOCKET_H

/**
 * Modulo de sockets. Contiene funciones de alto nivel para el manejo de
 * sockets así como la función que manejan los hilos.
 * @author Antonio Amor Mourelle &lt;antonio.amor@estudiante.uam.es&gt;
 * @author Esther Lopez Ramos &lt;esther.lopezramos@estudiante.uam.es&gt;
 * @author Mario Santiago Yepes &lt;mario.santiagoy@estudiante.uam.es&gt;
 * @file G-2301-04-P1-socket.h
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h> 
#include <netdb.h>
#include <redes2/irc.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <syslog.h>
#include <fcntl.h>

#include "G-2301-04-P3-ssl.h"

/**
 * @def MAXLEN
 * @brief Longitud maxima de un comando IRC
 */
#define MAXLEN 512 

typedef enum {
    TCP, UDP
} protocol;

/**
 * @brief Daemoniza el proceso
 */
void daemonizar();

/**
 * @brief Abre un socket para comunicarse con el servidor
 * @param protocol UDP en caso de querer conexion no fiable
 * TCP conexion segura (valor por defecto)
 * @return devuelve el descriptor de fichero del socket,
                  -1 en caso de error
 */
int openSocket(protocol p);

/**
 * @brief Dado un socket notifica al SO de la apertura del mismo
 * @param sck descriptor del socket
 * @param port puerto con el protocolo de nivel de aplicacion
 * @param max_clients numero maximo de clientes esperando a ser atendidos
 * @return 0 si se realiza todo correctamente, 	
                  -1 en caso de error
 */
int bindSocket(int sck, uint16_t port, int max_clients);

/**
 * @brief Acepta conexiones de clientes
 * @param sck identificador de fichero del socket
 * @return -1 en caso de error, identificador de fichero del socket del cliente
 */
int acceptSocket(int sck);

/**
 * @brief Dado un socket, conecta con una IP y un puerto especifico.
 * Específica para clientes.
 * @param sck identificador de fichero con el socket
 * @param host_name url o direccion IP del servidor
 * @param port numero de puerto
 * @return 0 si todo ha ido bien, -1 en caso de error
 */
int connectClientSocket(int sck, char* host_name, int port);

/**
 * @brief Dado un socket o una estructura ssl envia datos al otro
 * extremo de la conexion
 * @param sck identificador de fichero con el socket
 * @param ssl puntero a una estructura ssl. Si vale NULL se asume canal no seguro
 * @param p protocolo TCP o UDP (se asume TCP por defecto)
 * @param dest_addrUDP nombre del host al que vamos a enviar en UDP
 * @param portUDP puerto al que enviamos los datos en UDP, debe coincidir con el del socket
 * @param data datos que se envian
 * @param longitud de los datos que se envian
 * @return numero de bytes leidos si todo correcto
 * @return -1 en caso de error
 */
int sendData(int sck, SSL *ssl, protocol p, char *dest_addrUDP, int portUDP, char *data, int len);


/**
 * @brief Dado un socket o una estructura ssl recibe datos al otro
 * extremo de la conexion
 * @param sck identificador de fichero con el socket
 * @param ssl puntero a una estructura ssl. Si vale NULL se asume canal no seguro
 * @param p protocolo TCP o UDP (se asume TCP por defecto)
 * @param dest_addrUDP nombre del host al que vamos a enviar en UDP
 * @param portUDP puerto al que enviamos los datos en UDP, debe coincidir con el del socket
 * @param data datos que se envian
 * @param longitud de los datos que se envian
 * @return numero de bytes leidos si todo correcto
 * @return -1 en caso de error
 */
int receiveData(int sck, SSL *ssl, protocol p, char *dest_addrUDP, int portUDP, char *data, int len);

#endif /*SOCKET_H*/
