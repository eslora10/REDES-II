#include "G-2301-04-P3-ssl.h"

/**
 * @brief Inicia un nuevo contexto para la conexion ssl del servidor
 * @return NULL en caso de fallo
 * @return puntero al contexto
*/
SSL_CTX* nuevo_contexto_ssl(){   
    const SSL_METHOD *method;
    SSL_CTX *ctx;
 
    /*carga los mensajes de error*/
    SSL_load_error_strings();   
    /*Carga todos los algoritmos sostenidos*/
    SSL_library_init();
    /*Obtenemos el metodo de conexion*/
    method = SSLv23_method();
    if (!method){
        ERR_print_errors_fp(stderr);
        return NULL;
    }  
    /*Crea el nuevo contexto*/
    ctx = SSL_CTX_new(method);  
    if (ctx == NULL){
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
int cargar_certificados(SSL_CTX* ctx, char* CertFile, char* CertPath){
    /*Carga las CA propias*/
    if(!SSL_CTX_load_verify_locations(ctx, CertFile, CertPath)){
        ERR_print_errors_fp(stderr);
        return -1;
    }
    /*Carga las CA conocidas*/
    if(!SSL_CTX_set_default_verify_paths(ctx)){
        ERR_print_errors_fp(stderr);
        return -1;
    }

    /*Especificamos el certificado que usara la aplicacion*/
    if (SSL_CTX_use_certificate_chain_file(ctx, CertFile) <= 0){
        ERR_print_errors_fp(stderr);
        return -1;
    }
    /*Especificamos la clave privada (chain)*/
    if (SSL_CTX_use_PrivateKey_file(ctx, CertFile, SSL_FILETYPE_PEM) <= 0){
        ERR_print_errors_fp(stderr);
        return -1;
    }
    /*Verifica la clave privada*/
    if (!SSL_CTX_check_private_key(ctx)){
        fprintf(stderr, "La clave privada no coincide con la del certificado publico\n");
        return -1;
    }
    return 0;
}


/**
 * @brief Inicializa el contexto que será utilizado para la creación de canales seguros mediante SSL
 * @param cert_file nombre del certificado de la CA
 * @param cert_path ruta del certificado de la CA
 * @return contexto creado
 * @return NULL en caso de error
*/
SSL_CTX* fijar_contexto_SSL(char* cert_file, char* cert_path){
    SSL_CTX *ctx;
    ctx = nuevo_contexto_ssl();
    if(!ctx)
        return NULL;
    if(cargar_certificados(ctx, cert_file, cert_path) < 0)
        return NULL;

    return ctx;
}

SSL* nueva_conexion_ssl(SSL_CTX* ctx, int sck){
    SSL* ssl;
    
    /*Creamos la estructura ssl con el canal seguro*/
    ssl = SSL_new(ctx);
    if(!ssl){
        ERR_print_errors_fp(stderr);
        return NULL;
    }

    /*Asignamos a la estructura anterior el descriptor del socket en el que se conecta el cliente*/
    if(!SSL_set_fd(ssl, sck)){
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
SSL* conectar_canal_seguro_SSL(SSL_CTX* ctx, int sck){
    SSL* ssl;
    
    ssl = nueva_conexion_ssl(ctx, sck);
    if(!ssl)
        return NULL;

    /*Esperamos el handshake por parte del cliente*/
    if(SSL_connect(ssl) <= 0){
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
SSL* aceptar_canal_seguro_SSL(SSL_CTX* ctx, int sck){
    SSL* ssl;
    
    ssl = nueva_conexion_ssl(ctx, sck);
    if(!ssl)
        return NULL;

    /*Esperamos el handshake por parte del cliente*/
    if(SSL_accept(ssl) <= 0){
        ERR_print_errors_fp(stderr);
        return NULL;
    }
    
    return ssl;
}


















