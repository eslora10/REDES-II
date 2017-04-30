#include "../includes/G-2301-04-P3-redes2.h"

int main(){
    SSL_CTX* ctx = NULL;
    SSL* ssl = NULL;
    int sck;
    char buffer[MAXLEN];

    ctx = inicializar_nivel_SSL("certs/ca.pem", "certs/cliente.pem");
    if(!ctx){
        fprintf(stderr, "Los certificados del cliente no son correctos\n");
        return -1;
    }
    sck = openSocket(TCP);
    if (sck < 0){
        perror("Error abriendo socket");
        fprintf(stderr, "Error abriendo socket\n");
        return -1;
    }
    
    if(connectClientSocket(sck, "localhost", 6667) < 0){
	fprintf(stderr, "Error connect\n");
        perror("Error accept");
        close(sck);
        return -1;
    }
    
    ssl = conectar_canal_seguro_SSL(ctx, sck);
    if(!ssl){
        perror("Error en el handshake inicial");
        close(sck);
        return -1;
    }  
    if(evaluar_post_connectar_SSL(ssl) < 0){
        fprintf(stderr, "El servidor no ha enviado ningun certificado o no es verificado por la CA\n");
        close(sck);
        return -1;
    }

    /*ECHO*/
    do{
	bzero(buffer, MAXLEN);
        scanf("%s[^\n]\n", buffer);

        if(enviar_datos_SSL(ssl, buffer, MAXLEN) <= 0){
            fprintf(stderr, "Error al recibir datos\n");
            close(sck);
            return -1;
        }   
	
	bzero(buffer, MAXLEN);
     
	if(recibir_datos_SSL(ssl, buffer, MAXLEN) <= 0){
            fprintf(stderr, "Error al recibir datos\n");
            close(sck);
            return -1;
        }

	printf("%s\n", buffer);



    }while(strcmp(buffer, "exit"));

    cerrar_canal_SSL(ssl, ctx);
    close(sck);
         
    return 0;
}
