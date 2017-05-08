#ifndef FILES_H
#define FILES_H

/**
 * Modulo de ficheros. Contiene funciones de alto nivel para el envio de ficheros
 * desde la interfaz de usuario.
 * @author Antonio Amor Mourelle &lt;antonio.amor@estudiante.uam.es&gt;
 * @author Esther Lopez Ramos &lt;esther.lopezramos@estudiante.uam.es&gt;
 * @author Mario Santiago Yepes &lt;mario.santiagoy@estudiante.uam.es&gt;
 * @file G-2301-04-P2-files.h
 */

/**
 * @struct fileReceiver_args
 * @brief Estructura argumento de la funcion fileReceiver, con los parámetros necesarios para esta
 */
/** 
 * @var fileReceiver_args::sckF
 * Socket en el que previamente se ha abierto la conexion con el otro extremo
 */
/** 
 * @var fileReceiver_args::path
 * Ruta en la que se va a guardar el fichero
 */
/** 
 * @var fileReceiver_args::length
 * Longitud del fichero
 */
struct fileReceiver_args {
    int sckF;
    char *path;
    long unsigned int length;
};

/**
 * @struct fileSender_args
 * @brief Estructura argumento de la funcion fileSender, con los parámetros necesarios para esta
 */
/** 
 * @var fileSender_args::data
 * Datos del fichero
 */
/** 
 * @var fileSender_args::length
 * Longitud del fichero
 */
/** 
 * @var fileSender_args::sck
 * Socket en el que se espera al receptor del fichero
 */
struct fileSender_args {
    char *data;
    long unsigned int length;
    int sck; 
};

/**
 * @brief Gestiona el proceso de recepcion de ficheros
 * @param args estructura que contiene informacion sobre la recepcion
 * @return -1 en caso de error, identificador de fichero del socket del cliente
 */
void* fileReceiver(void *args);

/**
 * @brief Acepta la recepcion del envio de ficheros y abre un socket para ello
 * @param nick nick del emisor
 * @param msg comando que contiene la informacion necesaria para gestionar la recepcion
 * @return -1 en caso de error, identificador de fichero del socket del cliente
 */
int fileDialog(char *nick, char *msg);

/**
 * @brief Gestiona el proceso de envio
 * @param fs estructura que contiene informacion sobre el envio
 * @return -1 en caso de error, identificador de fichero del socket del cliente
 */
void* fileSender(void *fs);

#endif /*FILES_H*/
