CXX = g++
LIBS = -lcrypto -lssl -lpthread

BIO = bio_client bio_server
BIO_ENCRYPT = bio_client_encrypt bio_server_decrypt
SOCKET = cpp_client cpp_server
SOCKET_ENCRYPT = cpp_client_encrypt cpp_server_decrypt
SSL = bio_ssl_client bio_ssl_server cpp_ssl_server


bio: $(BIO)
bio_encrypt: $(BIO_ENCRYPT)
socket: $(SOCKET)
socket_encrypt: $(SOCKET_ENCRYPT)
ssl: $(SSL)

bio_client: bio/bio_client.o
	$(CXX) -o client bio/bio_client.o $(LIBS)
bio_server: bio/bio_server.o
	$(CXX) -o server bio/bio_client.o  $(LIBS)

bio_client_encrypt: bio_encrypt/bio_client_encrypt.o
	$(CXX) -o client bio_encrypt/bio_client_encrypt.o $(LIBS)
bio_server_decrypt: bio_encrypt/bio_server_decrypt.o
	$(CXX) -o server bio_encrypt/bio_server_decrypt.o $(LIBS)

cpp_client: socket/cpp_client.o
	$(CXX) -o client socket/cpp_client.o $(LIBS)
cpp_server: socket/cpp_server.o
	$(CXX) -o server socket/cpp_server.o $(LIBS)

cpp_client_encrypt: socket_encrypt/cpp_client_encrypt.o
	$(CXX) -o client socket_encrypt/cpp_client_encrypt.o $(LIBS)
cpp_server_decrypt: socket_encrypt/cpp_server_decrypt.o
	$(CXX) -o server socket_encrypt/cpp_server_decrypt.o $(LIBS)

bio_ssl_client: ssl/bio_ssl_client.o
	$(CXX) -o client ssl/bio_ssl_client.o $(LIBS)
bio_ssl_server: ssl/bio_ssl_server.o
	$(CXX) -o bio_server ssl/bio_ssl_server.o $(LIBS)
cpp_ssl_server: ssl/cpp_ssl_server.o
	$(CXX) -o cpp_server ssl/cpp_ssl_server.o $(LIBS)


.PHONY : clean
clean :
	-rm -f */*.o
	-rm -f server client bio_server cpp_server