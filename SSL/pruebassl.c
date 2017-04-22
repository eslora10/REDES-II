#include <stdio.h>
#include <openssl/err.h>
#include <openssl/ssl.h>

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

    /*Especificamos el certificado que usara la aplicaion*/
    
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
}

int main(){
	SSL_CTX* ctx;
	ctx = nuevo_contexto_ssl();
	cargar_certificados(ctx, "root.pem", "/home/alumnos/e321098/Desktop/root.pem");

	return 0;
}
