#include "G-2301-04-P3-ssl.h"

int main(){
    SSL_CTX* ctx = NULL;
    SSL* ssl = NULL;
    int sck;
    char buffer[MAXLEN];

    ctx = fijar_contexto_SSL("certs/ca.pem", "certs/cliente.pem");
    if(!ctx){
        fprintf(stderr, "Los certificados del cliente no son correctos\n");
        return -1;
    }
    sck = openSocket(TCP);
    if (sck < 0){
        perror("Error abriendo socket");
        return -1;
    }
    
    if(connectClientSocket(sck, "localhost", 6667) < 0){
        perror("Error accept");
        return -1;
    }
    
    ssl = conectar_canal_seguro_SSL(ctx, sck);
    if(!ssl){
        perror("Error en el handshake inicial");
        return -1;
    }  
    if(evaluar_post_connectar_SSL(ssl) < 0){
        fprintf(stderr, "El servidor no ha enviado ningun certificado o no es verificado por la CA\n");
        return -1;
    }

    /*ECHO*/
    do{
        scanf("%s[^\n]\n", buffer);

        if(enviar_datos_SSL(ssl, buffer, MAXLEN) <= 0){
            fprintf(stderr, "Error al recibir datos\n");
            return -1;
        }   
	
	bzero(buffer, MAXLEN);
     
	if(recibir_datos_SSL(ssl, buffer, MAXLEN) <= 0){
            fprintf(stderr, "Error al recibir datos\n");
            return -1;
        }

	printf("%s\n", buffer);



    }while(strcmp(buffer, "exit"));

	return 0;
}
