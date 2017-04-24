/**
 * Modulo de sockets. Contiene funciones de alto nivel para el manejo de
 * sockets así como la función que manejan los hilos y la que daemoniza.
 * @author Antonio Amor Mourelle &lt;antonio.amor@estudiante.uam.es&gt;
 * @author Esther Lopez Ramos &lt;esther.lopezramos@estudiante.uam.es&gt;
 * @author Mario Santiago Yepes &lt;mario.santiagoy@estudiante.uam.es&gt;
 * @file G-2301-04-P1-socket.h
 */

#include "G-2301-04-P1-socket.h"

/**
 * @brief Transforma al proceso que la llama en daemon y abre el log del sistema,
 * dicho fichero debe ser cerrado posteriormente
 */
void daemonizar() {

    pid_t pid;
    int numFiles, i;

    pid = fork();

    if (pid < 0)
        exit(EXIT_FAILURE);

    if (pid > 0)
        exit(EXIT_SUCCESS);

    /*Cerramos todos los ficheros*/
    numFiles = getdtablesize();
    for (i = 0; i < numFiles; i++) {
        close(i);
    }

    openlog("Server system messages:", LOG_CONS | LOG_PID | LOG_NDELAY, LOG_LOCAL3);

    syslog(LOG_ERR, "Initiating new server.");


    if (setsid() < 0) {
        syslog(LOG_ERR, "Error creating a new SID for the child process.");
        exit(EXIT_FAILURE);
    }

    if ((chdir("/")) < 0) {
        syslog(LOG_ERR, "Error changing the current working directory = \"/\"");
        exit(EXIT_FAILURE);
    }

    return;
}

/**
 * @brief Abre un socket para comunicarse con el servidor
 * @param protocol UDP en caso de querer conexion no fiable
 * TCP conexion segura (valor por defecto)
 * @return devuelve el descriptor de fichero del socket,
                  -1 en caso de error
 */
int openSocket(protocol p) {
    int sck;

    /*Abrimos el socket, STREAM significa que usaremos el protocolo TCP*/
    switch (p) {
        case UDP:
            sck = socket(AF_INET, SOCK_DGRAM, 0);
            if (sck < 0)
                return -1;
            break;
        default:
            sck = socket(AF_INET, SOCK_STREAM, 0);
            if (sck < 0)
                return -1;
            break;
    }
    return sck;
}

/**
 * @brief Dado un socket notifica al SO de la apertura del mismo
 * @param sck descriptor del socket
 * @param port puerto con el protocolo de nivel de aplicacion
 * @param max_clients numero maximo de clientes esperando a ser atendidos
 * @return 0 si se realiza todo correctamente,
                  -1 en caso de error
 */
int bindSocket(int sck, uint16_t port, int max_clients) {

    struct sockaddr_in addr;

    /*Ponemos todos los bytes de la estructura a 0*/
    memset(&addr, 0, sizeof (struct sockaddr_in));

    /*Rellenamos los campos de la estructura*/
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    /*Llamamos a bind para notificarle al SO del socket*/
    if (bind(sck, (struct sockaddr *) &addr, sizeof (addr)) < 0)
        return -1;

    /*Preparamos el servidor para recibir peticiones de clientes*/
    if (listen(sck, max_clients) < 0)
        return -1;

    return 0;
}

/**
 * @brief Acepta conexiones de clientes
 * @param sck identificador de fichero del socket
 * @return -1 en caso de error, identificador de fichero del socket del cliente
 */
int acceptSocket(int sck) {
    int cl_sck;
    socklen_t len;
    struct sockaddr_in cl_addr;

    len = sizeof (cl_addr);
    /*Llamamos a la funcion acept para recibir paquetes de clientes*/
    cl_sck = accept(sck, (struct sockaddr *) &cl_addr, &len);
    if (cl_sck < 0)
        return -1;
    return cl_sck;
}

/**
 * @brief Dado un socket, conecta con una IP y un puerto especifico.
 * Específica para clientes.
 * @param sck identificador de fichero con el socket
 * @param host_name url o direccion IP del servidor
 * @param port numero de puerto
 * @return 0 si todo ha ido bien, -1 en caso de error
 */
int connectClientSocket(int sck, char* host_name, int port) {
    struct sockaddr_in addr;
    struct hostent *server;

    /*Ponemos a 0 todos los bytes de la estructura*/
    memset(&addr, 0, sizeof (struct sockaddr_in));
    /*Rellenamos la estructura con la informacion del servidor*/
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    /*Obtenemos la direccion del servidor*/
    server = gethostbyname(host_name);
    if (!server)
        return -1;
    memcpy(&addr.sin_addr.s_addr, server->h_addr_list[0], server->h_length);

    /*Nos conectamos al servidor*/
    if (connect(sck, (struct sockaddr *) &addr, sizeof (addr)))
        return -1;

    return 0;

}
