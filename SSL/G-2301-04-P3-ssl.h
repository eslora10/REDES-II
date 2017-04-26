#ifndef SSL_H
#define SSL_H

#include <stdio.h>
#include <openssl/err.h>
#include <openssl/ssl.h>
#include "sockets/G-2301-04-P1-socket.h"

FILE *debug;


int inicializar_nivel_ssl();
/**
 * @brief Inicializa el contexto que será utilizado para la creación de canales seguros mediante SSL
 * @param cert_file nombre del certificado de la CA
 * @param cert_path ruta del certificado de la CA
 * @return contexto creado
 * @return NULL en caso de error
*/
SSL_CTX* fijar_contexto_SSL(char* cert_file, char* cert_path);

/**
 * @brief Dado un contexto SSL y un descriptor de socket obtiene un canal seguro SSL iniciando 
 * el proceso de handshake con el otro extremo
 * @param ctx contexto de la aplicacion
 * @param sck descriptor del socket
 * @return puntero a una estructura ssl con la conexion creada
 * @return NULL en caso de error
*/
SSL* conectar_canal_seguro_SSL(SSL_CTX* ctx, int sck);

/**
 * @brief Dado un contexto SSL y un descriptor de socket esta función se queda
 * esperando hasta recibir un handshake por parte del cliente.
 * @param ctx contexto de la aplicacion
 * @param sck descriptor del socket
 * @return puntero a una estructura ssl con la conexion creada
 * @return NULL en caso de error
*/
SSL* aceptar_canal_seguro_SSL(SSL_CTX* ctx, int sck);


/**
 * @brief comprobará una vez realizado el handshake que el canal de comunicación se puede 
 * considerar seguro
 * @param ssl puntero a la estructura de conexión ssl
 * @return -1 en caso de error
 * @return 0 correcto
 */
int evaluar_post_connectar_SSL(const SSL* ssl);

/**
 * @brief Envia datos a traves de la conexion ssl preestablecida
 * @param ssl puntero a la estructura de conexión ssl
 * @return -1 en caso de error
 * @return 0 correcto
 */
int enviar_datos_SSL(SSL* ssl, char* buffer, int nbytes);


/**
 * @brief Recibe datos a traves de la conexion ssl preestablecida
 * @param ssl puntero a la estructura de conexión ssl
 * @return <=0 en caso de error
 * @return 0 correcto
 */
int recibir_datos_SSL(SSL* ssl, char* buffer, int nbytes); 

#endif /*SSL_H*/
