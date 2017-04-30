#include "../includes/G-2301-04-P3-redes2.h"

/**
 * @brief Realiza todas las llamadas necesarias para que la apli-
 * cación pueda usar la capa segura SSL.
 * @param cert_file nombre del certificado de la CA
 * @param cert_path ruta del certificado de la CA
 * @return contexto creado
 * @return NULL en caso de error
*/
SSL_CTX* inicializar_nivel_SSL(char *ca_cert, char *clserv_pem){
    /*carga los mensajes de error*/
    SSL_load_error_strings();
    /*Carga todos los algoritmos sostenidos*/
    SSL_library_init();

    return fijar_contexto_SSL(ca_cert, clserv_pem);
}

/**
 * @brief Inicia un nuevo contexto para la conexion ssl del servidor
 * @return NULL en caso de fallo
 * @return puntero al contexto
 */
SSL_CTX* nuevo_contexto_ssl() {
    const SSL_METHOD *method = NULL;
    SSL_CTX *ctx = NULL;

    /*Obtenemos el metodo de conexion*/
    method = SSLv23_method();
    if (!method) {
        ERR_print_errors_fp(stderr);
        return NULL;
    }
    /*Crea el nuevo contexto*/
    ctx = SSL_CTX_new(method);
    if (ctx == NULL) {
        ERR_print_errors_fp(stderr);
        return NULL;
    }
    return ctx;
}

/**
 * @brief Carga los certificados con los que trabajara la aplicacion
 * @param ctx contexto de la conexion
 * @param CertFile nombre del fichero donde se encuentra el certificado 
 * de la CA y la clave privada 
 * @param CertPath direccion del fichero con el certificado 
 * de la CA y la clave privada 
 * @return NULL en caso de fallo
 * @return puntero al contexto
 */
int cargar_certificados(SSL_CTX* ctx, char* ca_cert, char* clserv_cert) {
    /*Carga las CA propias*/
    if (!SSL_CTX_load_verify_locations(ctx, ca_cert, ca_cert)) {
        ERR_print_errors_fp(stderr);
        return -1;
    }
    /*Carga las CA conocidas*/
    if (!SSL_CTX_set_default_verify_paths(ctx)) {
        ERR_print_errors_fp(stderr);
        return -1;
    }

    /*Especificamos el certificado que usara la aplicacion*/
    if (SSL_CTX_use_certificate_chain_file(ctx, clserv_cert) != 1) {
        ERR_print_errors_fp(stderr);
        return -1;
    }
    /*Especificamos la clave privada (chain)*/
    if (SSL_CTX_use_PrivateKey_file(ctx, clserv_cert, SSL_FILETYPE_PEM) != 1) {
        ERR_print_errors_fp(stderr);
        return -1;
    }
    /*Verifica la clave privada*/
    if (SSL_CTX_check_private_key(ctx) != 1) {
        fprintf(stderr, "La clave privada no coincide con la del certificado publico\n");
        return -1;
    }

    SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER | SSL_VERIFY_FAIL_IF_NO_PEER_CERT, NULL);

    return 0;
}

/**
 * @brief Inicializa el contexto que será utilizado para la creación de canales seguros mediante SSL
 * @param cert_file nombre del certificado de la CA
 * @param cert_path ruta del certificado de la CA
 * @return contexto creado
 * @return NULL en caso de error
 */
SSL_CTX* fijar_contexto_SSL(char* ca_cert, char* clserv_cert) {
    SSL_CTX *ctx = NULL;
    ctx = nuevo_contexto_ssl();
    if (!ctx)
        return NULL;
    if (cargar_certificados(ctx, ca_cert, clserv_cert) < 0)
        return NULL;

    return ctx;
}

SSL* nueva_conexion_ssl(SSL_CTX* ctx, int sck) {
    SSL* ssl = NULL;

    /*Creamos la estructura ssl con el canal seguro*/
    ssl = SSL_new(ctx);
    if (!ssl) {
        ERR_print_errors_fp(stderr);
        return NULL;
    }

    /*Asignamos a la estructura anterior el descriptor del socket en el que se conecta el cliente*/
    if (!SSL_set_fd(ssl, sck)) {
        ERR_print_errors_fp(stderr);
        return NULL;
    }

    return ssl;
}

/**
 * @brief Dado un contexto SSL y un descriptor de socket obtiene un canal seguro SSL iniciando 
 * el proceso de handshake con el otro extremo
 * @param ctx contexto de la aplicacion
 * @param sck descriptor del socket
 * @return puntero a una estructura ssl con la conexion creada
 * @return NULL en caso de error
 */
SSL* conectar_canal_seguro_SSL(SSL_CTX* ctx, int sck) {
    SSL* ssl = NULL;

    ssl = nueva_conexion_ssl(ctx, sck);
    if (!ssl)
        return NULL;

    /*Esperamos el handshake por parte del cliente*/
    if (SSL_connect(ssl) <= 0) {
        ERR_print_errors_fp(stderr);
        return NULL;
    }

    return ssl;
}

/**
 * @brief Dado un contexto SSL y un descriptor de socket esta función se queda
 * esperando hasta recibir un handshake por parte del cliente.
 * @param ssl puntero a la estructura de conexión ssl
 * @return 0 correcto
 * @return -1 error
 */
SSL* aceptar_canal_seguro_SSL(SSL_CTX* ctx, int sck) {
    SSL* ssl = NULL;

    ssl = nueva_conexion_ssl(ctx, sck);
    if (!ssl)
        return NULL;

    /*Esperamos el handshake por parte del cliente*/
    if (SSL_accept(ssl) <= 0) {
        ERR_print_errors_fp(stderr);
        return NULL;
    }

    return ssl;
}

/**
 * @brief comprobará una vez realizado el handshake que el canal de comunicación se puede 
 * considerar seguro
 * @param ssl puntero a la estructura de conexión ssl
 * @return -1 en caso de error
 * @return 0 correcto
 */
int evaluar_post_connectar_SSL(const SSL* ssl) {
    X509 *cert = NULL;
    cert = SSL_get_peer_certificate(ssl);
    if (!cert) {
        ERR_print_errors_fp(stderr);
        printf("Error no hay certificado\n");
        return -1;
    }
    X509_free(cert);
    if (SSL_get_verify_result(ssl) != X509_V_OK) {
        ERR_print_errors_fp(stderr);
        printf("Error la CA no lo verifica\n");
        return -1;
    }
    return 0;
}

/**
 * @brief Envia datos a traves de la conexion ssl preestablecida
 * @param ssl puntero a la estructura de conexión ssl
 * @return <=0 en caso de error
 * @return 0 correcto
 */
int enviar_datos_SSL(SSL* ssl, char* buffer, int nbytes) {
    return SSL_write(ssl, (void*) buffer, nbytes);
}

/**
 * @brief Recibe datos a traves de la conexion ssl preestablecida
 * @param ssl puntero a la estructura de conexión ssl
 * @return <=0 en caso de error
 * @return 0 correcto
 */
int recibir_datos_SSL(SSL* ssl, char* buffer, int nbytes) {
    return SSL_read(ssl, (void*) buffer, nbytes);
}


/**
 * @brief Libera los recursos reservados para la capa ssl
 * @param ssl puntero a la estructura de conexión ssl
 * @param ctx contexto creado para la conexion ssl
 */
void cerrar_canal_SSL(SSL *ssl, SSL_CTX* ctx){
    SSL_shutdown(ssl);
    SSL_free(ssl);
    SSL_CTX_free(ctx);
}




















