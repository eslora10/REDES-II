all: certificados ssl

certificados:
	openssl genrsa -out cakey.pem 2048
	openssl req -new -x509 -key cakey.pem -out cacert.pem -subj /CN=Redes2\ CA
	cat cacert.pem cakey.pem > certs/ca.pem
	rm cakey.pem cacert.pem

	openssl genrsa -out clientekey.pem 2048
	openssl req -new -key clientekey.pem -out clientecert.pem -subj /CN=G-2301-04-P3-client
	openssl x509 -req -CAcreateserial -CA certs/ca.pem -in clientecert.pem -out clientecert.pem 
	cat clientecert.pem clientekey.pem > certs/cliente.pem
	rm clientekey.pem clientecert.pem

	openssl genrsa -out servidorkey.pem 2048
	openssl req -new -key servidorkey.pem -out servidorcert.pem -subj /CN=G-2301-04-P3-server
	openssl x509 -req -CAcreateserial -CA certs/ca.pem -in servidorcert.pem -out servidorcert.pem 
	cat servidorcert.pem servidorkey.pem > certs/servidor.pem
	rm servidorkey.pem servidorcert.pem

ssl: G-2301-04-P1-socket.o pruebassl.o G-2301-04-P3-ssl.o
	gcc -o ssl pruebassl.o G-2301-04-P1-socket.o G-2301-04-P3-ssl.o -lssl -lcrypto

pruebassl.o: sockets/G-2301-04-P1-socket.h G-2301-04-P3-ssl.h
	gcc -c pruebassl.c

G-2301-04-P1-socket.o: sockets/G-2301-04-P1-socket.h
	gcc -c sockets/G-2301-04-P1-socket.c

G-2301-04-P3-ssl.o: sockets/G-2301-04-P1-socket.h G-2301-04-P3-ssl.h
	gcc -c G-2301-04-P3-ssl.c

clean: 
	rm -f ssl *.o certs/*.* 
