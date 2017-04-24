#include "G-2301-04-P3-ssl.h"

int main(){
	SSL_CTX* ctx;
    SSL* ssl;
    int sck, cl_sck;

	ctx = fijar_contexto_SSL("certs/ca.pem", "certs/ca.pem");
    sck = openSocket(TCP);
    if (sck < 0){
        perror("Error abriendo socket");
        return -1;
    }
    if(bindSocket(sck, 6667, 1) < 0){
        perror("Error en bind");
        return -1;
    }
    
    cl_sck = acceptSocket(sck);
    if(cl_sck < 0){
        perror("Error accept");
        return -1;
    }
    
    ssl = conectar_canal_seguro_SSL(ctx, cl_sck);
    if(!ssl){
        perror("Error en el handshake inicial");
        return -1;
    }
    
    

    
    

	return 0;
}
