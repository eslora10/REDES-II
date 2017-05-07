/**
* Fichero principal.
* @author Antonio Amor Mourelle &lt;antonio.amor@estudiante.uam.es&gt;
* @author Esther Lopez Ramos &lt;esther.lopezramos@estudiante.uam.es&gt;
* @author Mario Santiago Yepes &lt;mario.santiagoy@estudiante.uam.es&gt;
* @file G-2301-04-P1-server.c
*/

#include "../includes/G-2301-04-P3-redes2.h"
#include <getopt.h>

/**
 * @var sck
 * @brief Variable global, socket en el que se reciben las conexiones de los
 * clientes
 */
int sck;
int sck_ssl;
int port = 6669;
SSL_CTX *ctx = NULL;
SSL *ssl = NULL;

/**
 * @brief Funcion que se encarga de aceptar conexiones seguras
 * @param ssl_ctx socket en el que se espera clientes
 * @return siempre devuelve NULL
 */
void* SSLConnections(void *kk){
    int cl_sck;
    pthread_t thread;
    Sck_SSL *argAttend = NULL;

    /*Se abre un socket seguro que escucha en el puerto 6669*/
    ctx = inicializar_nivel_SSL("certs/ca.pem", "certs/servidor.pem");
    if (!ctx) {
        syslog(LOG_ERR, "Error loading certificates");
        pthread_exit(NULL);
    }

    sck_ssl = openSocket(TCP);
    if (sck_ssl < 0) {
        syslog(LOG_ERR, "Error opening socket");
        pthread_exit(NULL);
    }

    if (bindSocket(sck_ssl, port, 1, TCP) < 0) {
        syslog(LOG_ERR, "Error binding socket");
        close(sck_ssl);
        pthread_exit(NULL);
    }

    while(1){
        /*Recibimos conexiones de clientes*/
        cl_sck = acceptSocket(sck_ssl);
        if (cl_sck < 0) {
            syslog(LOG_ERR, "Error acepting clients");
            close(sck);
            pthread_exit(NULL);
        }

        /*Intentamos el hanshake con el cliente*/
        ssl = aceptar_canal_seguro_SSL(ctx, cl_sck);
        if (!ssl) {
            syslog(LOG_ERR, "Error during the initial handshake");
            close(sck);
            close(cl_sck);
            pthread_exit(NULL);
        }

        /*Comprobamos si los certificados del cliente son correctos*/
        if (evaluar_post_connectar_SSL(ssl) < 0) {
            syslog(LOG_ERR, "Error obtaining client's certificates");
            close(sck);
            close(cl_sck);
            pthread_exit(NULL);
        }

        argAttend = (Sck_SSL*) malloc(sizeof(Sck_SSL));
        if(!argAttend){
            syslog(LOG_ERR, "Memory error");
            close(sck);
            close(cl_sck);
            pthread_exit(NULL);
        }
        argAttend->sck = cl_sck;
        argAttend->ssl = ssl;

        /*Creamos un hilo y le asignamos la funcion attendClientSockeet*/
        if (pthread_create(&thread, NULL, attendClientSocket, (void *) argAttend) < 0) {
            syslog(LOG_ERR, "Error creating thread");
            pthread_exit(NULL);
        }


        if (pthread_detach(thread) < 0) {
            syslog(LOG_ERR, "Error  detach");
            pthread_exit(NULL);
        }
    }
    pthread_exit(NULL);

}
/**
 * @brief Rutina manejadora de la señal CTR+C
 * @param signal codigo de la señal
 * @return -1 en caso de fallo, 0 OK
 */
void handler(int signal) {
    close(sck);
    close(sck_ssl);
    syslog(LOG_INFO, "Closing IRC Server");
    cerrar_canal_SSL(ssl, ctx);
    closelog();
    exit(EXIT_SUCCESS);
}

/**
 * @brief Funcion principal manejadora del sevidor y sus hilos
 * @param argc numero de argumentos
 * @param argv argumentos variables. -h para mostrar
 * @return -1 en caso de fallo, 0 OK
 */
int main(int argc, char** argv) {
    int i, cl_sck;
    int long_index;
    char opt;
    pthread_t thread, ssl_thread, pingthread;
    Sck_SSL *argAttend = NULL;

    static struct option options[] = {
		{"port", required_argument, 0,'1'},
        {"ssl", no_argument, 0, '2'},
        {"s", no_argument, 0, '3'},
        {"help", no_argument, 0, '4'},
        {"h", no_argument, 0, '5'},
        {0, 0, 0, 0}
    };


    if (signal(SIGINT, handler) == SIG_ERR) {
        syslog(LOG_ERR, "error handler");
        return -1;
    }


    for (i = 0; i < 37; i++)
        Commands[i] = FuncDefault;
    Commands[2] = nickCommand;
    Commands[3] = userCommand;
    Commands[5] = modeCommand;
    Commands[7] = quitCommand;
    Commands[9] = joinCommand;
    Commands[10] = partCommand;
    Commands[11] = topicCommand;
    Commands[12] = namesCommand;
    Commands[13] = listCommand;
    Commands[15] = kickCommand;
    Commands[16] = privmsgCommand;
    Commands[18] = motdCommand;
    Commands[30] = whoCommand;
    Commands[31] = whoisCommand;
    Commands[34] = pingCommand;
    Commands[35] = pongCommand;
    Commands[37] = awayCommand;


    //daemonizar();
    syslog(LOG_INFO, "Initializing IRC Server");




    while ((opt = getopt_long_only(argc, argv, "1:2:3:4:5:6", options, &long_index)) != -1) {
        switch (opt) {
            case '1':
                port = atoi(optarg);

                /*Creamos un hilo y le asignamos la funcion attendClientSockeet*/
                if (pthread_create(&ssl_thread, NULL, SSLConnections, NULL) < 0) {
                    syslog(LOG_ERR, "Error creating thread");
                    return -1;
                }

                if (pthread_detach(ssl_thread) < 0) {
                    syslog(LOG_ERR, "Error  detach");
                    return -1;
                }
            case '2':
            case '3':
                /*Bandera de ssl*/
                break;
            case '4':
            case '5':
            case '?':
                /*Ayuda*/
                printf("Opciones:\n");
                printf("-h --help\tMuestra esta pagina\n");
                printf("-s --ssl\tActiva la ejecucion con SSL(Sin implementar)\n");
                return 0;
            default:
                break;
        }
    }

    /*Se abre un socket no seguro que escucha en el puerto 6667*/
    sck = openSocket(TCP);
    if (sck < 0) {
        syslog(LOG_ERR, "Error socket");
        return -1;
    }

    if (bindSocket(sck, 6667, 10, TCP) < 0) {
        syslog(LOG_ERR, "Error bind");
        return -1;
    }
	/*Hilo pingpong*/
	if (pthread_create(&pingthread, NULL, pingpong, NULL) < 0) {
            syslog(LOG_ERR, "Error creating thread");
            return -1;
        }
	if (pthread_detach(pingthread) < 0) {
            syslog(LOG_ERR, "Error  detach");
            return -1;
        }

    while (1) {
        if ((cl_sck = acceptSocket(sck)) < 0) {
            syslog(LOG_ERR, "Error accept");
            return -1;
        }
        argAttend = (Sck_SSL*)malloc(sizeof(Sck_SSL));
        if(!argAttend) {
            close(cl_sck);
            syslog(LOG_ERR, "Error de memoria");
            return -1;
        }
        argAttend->sck = cl_sck;
        argAttend->ssl = NULL;
        /*Creamos un hilo y le asignamos la funcion attendClientSockeet*/
        if (pthread_create(&thread, NULL, attendClientSocket, (void *) argAttend) < 0) {
            close(cl_sck);
            free(argAttend);
            syslog(LOG_ERR, "Error creating thread");
            return -1;
        }

        if (pthread_detach(thread) < 0) {
            close(cl_sck);
            free(argAttend);
            syslog(LOG_ERR, "Error  detach");
            return -1;
        }
    }


    close(sck);

    return 0;
}
