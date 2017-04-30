CC = gcc
CFLAGS = -pedantic -Wall  -I $(IDIR)
LIBS = -lircredes -lircinterface -lirctad -lsoundredes -lpthread -lredes2 -lssl -lcrypto

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

_OBJL = G-2301-04-P1-socket.o G-2301-04-P3-ssl.o
OBJL = $(patsubst %,$(OLDIR)/%,$(_OBJL))


all: certificados echo/servidor_echo echo/cliente_echo

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

libredes2.a: $(OBJL)
	ar rcs $@ $^

#G-2301-04-P1-server: libredes2.a
#	$(CC) -L. $(SDIR)/G-2301-04-P1-server.c $(SLDIR)/*.c $(CFLAGS) $(LIBS)
#	mv a.out G-2301-04-P1-server

echo/servidor_echo: libredes2.a
	$(CC) -L. $(SDIR)/servidor_echo.c $(SLDIR)/*.c $(CFLAGS) $(LIBS)
	mv a.out echo/servidor_echo 

echo/cliente_echo: libredes2.a
	$(CC) -L. $(SDIR)/cliente_echo.c $(SLDIR)/*.c $(CFLAGS) $(LIBS)
	mv a.out echo/cliente_echo  
	mv libredes2.a $(LDIR)

.PHONY: clean
clean:
	rm -f $(ODIR)/*.o $(OLDIR)/*.o *~ core $(INCDIR)/*~
	rm -f G-2301-04-P1-server $(LDIR)/libredes2.a
	rm -f certs/*.* echo/*
