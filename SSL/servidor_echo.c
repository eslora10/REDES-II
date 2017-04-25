#include "G-2301-04-P3-ssl.h"

int main() {
    SSL_CTX* ctx = NULL;
    SSL* ssl = NULL;
    int sck, cl_sck;
    char buffer[MAXLEN];

    ctx = fijar_contexto_SSL("certs/ca.pem", "certs/servidor.pem");
    if (!ctx) {
        fprintf(stderr, "Los certificados del servidor no son correctos\n");
        return -1;
    }
    sck = openSocket(TCP);
    if (sck < 0) {
        perror("Error abriendo socket");
        return -1;
    }
    if (bindSocket(sck, 6667, 1) < 0) {
        perror("Error en bind");
        close(sck);
        return -1;
    }

    cl_sck = acceptSocket(sck);
    if (cl_sck < 0) {
        perror("Error accept");
        close(sck);
        return -1;
    }

    ssl = aceptar_canal_seguro_SSL(ctx, cl_sck);
    if (!ssl) {
        perror("Error en el handshake inicial");
        close(sck);
        close(cl_sck);
        return -1;
    }
    if (evaluar_post_connectar_SSL(ssl) < 0) {
        fprintf(stderr, "El cliente no ha enviado ningun certificado o no es verificado por la CA\n");
        /*close(sck);
        close(cl_sck);
        return -1;*/
    }

    /*ECHO*/
    do {
        bzero(buffer, MAXLEN);

        if (recibir_datos_SSL(ssl, buffer, MAXLEN) <= 0) {
            fprintf(stderr, "Error al recibir datos\n");
            close(sck);
            close(cl_sck);
            return -1;
        }


        if (enviar_datos_SSL(ssl, buffer, MAXLEN) <= 0) {
            fprintf(stderr, "Error al enviar datos\n");
            close(sck);
            close(cl_sck);
            return -1;
        }
    } while(strcmp(buffer, "exit"));

    close(sck);
    close(cl_sck);
    return 0;
}
