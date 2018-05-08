
all: server.exe

server.exe: server.c clientG.c
	gcc -o server server.c
	gcc -o clientG clientG.c
clean:
	 rm server.o clientG.o
