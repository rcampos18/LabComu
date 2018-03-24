
all: server.exe

server.exe: server.c client.c clientG.c
	gcc -o server server.c
	gcc -o clientG clientG.c
	gcc -o client client.c

clean:
	 rm server.o clientG.o client.o
