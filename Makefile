all: client server

CLIENT_OBJS = client.o init_socket.o handle_error.o
SERVER_OBJS = server.o init_socket.o handle_error.o

client: $(CLIENT_OBJS)
	gcc -o client $(CLIENT_OBJS)

server: $(SERVER_OBJS)
	gcc -o server $(SERVER_OBJS)

client.o: client.c init_socket.h handle_error.h
	gcc -c client.c

server.o: server.c init_socket.h handle_error.h
	gcc -c server.c

init_socket.o: init_socket.c init_socket.h
	gcc -c init_socket.c

handle_error.o: handle_error.c handle_error.h
	gcc -c handle_error.c

clean:
	rm -f client server $(CLIENT_OBJS) $(SERVER_OBJS)
	