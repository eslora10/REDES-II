CC = gcc -g
CFLAGS = -pedantic -Wall -pthread `pkg-config --cflags gtk+-3.0` -I $(IDIR)

#directorios
IDIR=includes
ODIR=obj
LDIR=lib
SDIR=src
SLDIR=srclib
OLDIR=objlib


LIBS = -L$(LDIR) -lredes2 -lssl -lcrypto -lpthread -lircinterface -lircredes -lirctad -lsoundredes -rdynamic `pkg-config --libs gtk+-3.0` -lpulse -lpulse-simple

#rutas de los ficheros
_DEPS = G-2301-04-P1-socket.h G-2301-04-P3-ssl.h G-2301-04-P3-redes2.h G-2301-04-P1-command.h G-2301-04-P2-userCommands.h G-2301-04-P2-messages.h G-2301-04-P2-files.h G-2301-04-P2-audio.h

DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = cliente_echo.o servidor_echo.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

_SRCL = G-2301-04-P1-socket.c G-2301-04-P3-ssl.c G-2301-04-P1-command.c G-2301-04-P2-userCommands.c G-2301-04-P2-messages.c G-2301-04-P2-files.c G-2301-04-P2-audio.c
SRCL = $(patsubst %,$(SLDIR)/%,$(_SRCL)) 

_OBJL = G-2301-04-P1-socket.o G-2301-04-P3-ssl.o G-2301-04-P1-command.o G-2301-04-P2-userCommands.o G-2301-04-P2-messages.o G-2301-04-P2-files.o G-2301-04-P2-audio.o
OBJL = $(patsubst %,$(OLDIR)/%,$(_OBJL))


all: certificados echo/servidor_echo echo/cliente_echo cliente_servidor/servidor_IRC cliente_servidor/cliente_IRC

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

cliente_servidor/cliente_IRC: $(LDIR)/libredes2.a src/xchat2.c
	$(CC) -o $@ src/xchat2.c $(CFLAGS) $(LIBS)

cliente_servidor/servidor_IRC: $(LDIR)/libredes2.a src/G-2301-04-P1-server.c
	$(CC) -o $@ src/G-2301-04-P1-server.c $(CFLAGS) $(LIBS)

echo/servidor_echo: $(LDIR)/libredes2.a src/servidor_echo.c
	$(CC) -o $@ src/servidor_echo.c $(CFLAGS) $(LIBS)

echo/cliente_echo: $(LDIR)/libredes2.a src/cliente_echo.c
	$(CC) -o $@ src/cliente_echo.c $(CFLAGS) $(LIBS)

.PHONY: clean
clean:
	rm -f $(ODIR)/*.o $(OLDIR)/*.o *~ core $(INCDIR)/*~
	rm -f $(LDIR)/libredes2.a
	rm -f certs/*.* echo/* cliente_servidor/*
