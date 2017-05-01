CC = gcc
CFLAGS = -pedantic -Wall -pthread  -I $(IDIR)
LIBS = -lssl -lcrypto -lircredes -lircinterface -lirctad -lsoundredes -lpthread 

#directorios
IDIR=includes
ODIR=obj
LDIR=lib
SDIR=src
SLDIR=srclib
OLDIR=objlib

#rutas de los ficheros
_DEPS = G-2301-04-P1-socket.h G-2301-04-P3-ssl.h G-2301-04-P3-redes2.h

DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = cliente_echo.o servidor_echo.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

_SRCL = G-2301-04-P1-socket.c G-2301-04-P3-ssl.c
SRCL = $(patsubst %,$(SLDIR)/%,$(_SRCL)) 

_OBJL = G-2301-04-P1-socket.o G-2301-04-P3-ssl.o G-2301-04-P1-command.o
OBJL = $(patsubst %,$(OLDIR)/%,$(_OBJL))


all: certificados echo/servidor_echo echo/cliente_echo cliente_servidor/servidor_IRC

certificados:
	openssl genrsa -out cakey.pem 2048
	openssl req -new -x509 -key cakey.pem -out cacert.pem -subj /CN=Redes2\ CA
	cat cacert.pem cakey.pem > certs/ca.pem
	
	openssl genrsa -out clientekey.pem 2048
	openssl req -new -key clientekey.pem -out clientecert.pem -subj /CN=G-2301-04-P3-client
	openssl x509 -req -CAcreateserial -CA certs/ca.pem -in clientecert.pem -out clientecert.pem 
	cat clientecert.pem clientekey.pem cacert.pem > certs/cliente.pem
	rm clientekey.pem clientecert.pem

	openssl genrsa -out servidorkey.pem 2048
	openssl req -new -key servidorkey.pem -out servidorcert.pem -subj /CN=G-2301-04-P3-server
	openssl x509 -req -CAcreateserial -CA certs/ca.pem -in servidorcert.pem -out servidorcert.pem 
	cat servidorcert.pem servidorkey.pem cacert.pem > certs/servidor.pem
	rm servidorkey.pem servidorcert.pem

	rm cakey.pem cacert.pem

#$@ == parte izq de ":"
#$< == primer elemento de la lista de dependencias
#$^ == parte der de ":"

$(ODIR)/%.o: $(SDIR)/%.c $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $<

$(OLDIR)/%.o: $(SLDIR)/%.c $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $<

$(LDIR)/libredes2.a: $(OBJL)
	ar rcs $@ $^

cliente_servidor/servidor_IRC: $(LDIR)/libredes2.a src/G-2301-04-P1-server.c
	$(CC) -o $@ src/G-2301-04-P1-server.c -L$(LDIR) -lredes2 $(CFLAGS) $(LIBS)

echo/servidor_echo: $(LDIR)/libredes2.a src/servidor_echo.c
	$(CC) -o $@ src/servidor_echo.c -L$(LDIR) -lredes2 $(CFLAGS) $(LIBS)

echo/cliente_echo: $(LDIR)/libredes2.a src/cliente_echo.c
	$(CC) -o $@ src/cliente_echo.c -L$(LDIR) -lredes2 $(CFLAGS) $(LIBS)

.PHONY: clean
clean:
	rm -f $(ODIR)/*.o $(OLDIR)/*.o *~ core $(INCDIR)/*~
	rm -f $(LDIR)/libredes2.a
	rm -f certs/*.* echo/* cliente_servidor/*
