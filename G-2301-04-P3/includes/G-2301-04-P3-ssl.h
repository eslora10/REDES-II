#ifndef SSL_H
#define SSL_H

/**
 * Modulo de ssl. Contiene funciones de alto nivel para la gestion
 * de uncliente en modo seguridad ssl.
 * @author Antonio Amor Mourelle &lt;antonio.amor@estudiante.uam.es&gt;
 * @author Esther Lopez Ramos &lt;esther.lopezramos@estudiante.uam.es&gt;
 * @author Mario Santiago Yepes &lt;mario.santiagoy@estudiante.uam.es&gt;
 * @file G-2301-04-P3-ssl.h
 */

#include <stdio.h>
#include <openssl/err.h>
#include <openssl/ssl.h>
#include "G-2301-04-P1-socket.h"

/**
 * @brief Realiza todas las llamadas necesarias para que la apli-
 * cación pueda usar la capa segura SSL.
 * @param ca_cert nombre del certificado de la CA
 * @param clserv_pem ruta del certificado de la CA
 * @return contexto creado
 * @return NULL en caso de error
*/
SSL_CTX* inicializar_nivel_SSL(char *ca_cert, char *clserv_pem);

/**
 * @brief Inicializa el contexto que será utilizado para la creación de canales seguros mediante SSL
 * @param ca_cert nombre del certificado de la CA
 * @param clserv_cert ruta del certificado de la CA
 * @return contexto creado
 * @return NULL en caso de error
*/
SSL_CTX* fijar_contexto_SSL(char* ca_cert, char* clserv_cert);

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
 * @param buffer cadena con los datos enviados
 * @param nbytes numero de bytes que se envian
 * @return -1 en caso de error
 * @return 0 correcto
 */
int enviar_datos_SSL(SSL* ssl, char* buffer, int nbytes);


/**
 * @brief Recibe datos a traves de la conexion ssl preestablecida
 * @param ssl puntero a la estructura de conexión ssl
 * @param buffer cadena con los datos recibidos
 * @param nbytes numero de bytes máximos que se pueden recibir
 * @return <=0 en caso de error
 * @return 0 correcto
 */
int recibir_datos_SSL(SSL* ssl, char* buffer, int nbytes);

/**
 * @brief Libera los recursos reservados para la capa ssl
 * @param ssl puntero a la estructura de conexión ssl
 * @param ctx contexto creado para la conexion ssl
 */
void cerrar_canal_SSL(SSL *ssl, SSL_CTX* ctx); 

#endif /*SSL_H*/
