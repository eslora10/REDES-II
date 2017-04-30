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
/**
 * @brief Rutina manejadora de la señal CTR+C
 * @param signal codigo de la señal
 * @return -1 en caso de fallo, 0 OK
 */
void handler(int signal) {
    close(sck);
    syslog(LOG_INFO, "Closing IRC Server");
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
    pthread_t thread;

    static struct option options[] = {
        {"ssl", no_argument, 0, '1'},
        {"s", no_argument, 0, '2'},
        {"help", no_argument, 0, '3'},
        {"h", no_argument, 0, '4'},
        {0, 0, 0, 0}
    };

    while ((opt = getopt_long_only(argc, argv, "1:2:3:4:5", options, &long_index)) != -1) {
        switch (opt) {
            case '1':
            case '2':
                /*Bandera de ssl*/
                /*Se implementa en la practica 3*/
                return 0;
            case '3':
            case '4':
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

    daemonizar();
    syslog(LOG_INFO, "Initializing IRC Server");

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
    Commands[31] = whoisCommand;
    Commands[34] = pingCommand;
    Commands[37] = awayCommand;


    if (signal(SIGINT, handler) == SIG_ERR) {
        syslog(LOG_ERR, "error handler");
        return -1;
    }

    sck = openSocket(TCP);
    if (sck < 0) {
        syslog(LOG_ERR, "Error socket");
        return -1;
    }

    if (bindSocket(sck, 6667, 10) < 0) {
        syslog(LOG_ERR, "Error bind");
        return -1;
    }

    while (1) {
        if ((cl_sck = acceptSocket(sck)) < 0) {
            syslog(LOG_ERR, "Error accept");
            return -1;
        }
        /*Creamos un hilo y le asignamos la funcion attendClientSockeet*/
        if (pthread_create(&thread, NULL, attendClientSocket, (void *) &cl_sck) < 0) {
            syslog(LOG_ERR, "Error creating thread");
            return -1;
        }

        if (pthread_detach(thread) < 0) {
            syslog(LOG_ERR, "Error  detach");
            return -1;
        }
    }


    close(sck);

    return 0;
}
