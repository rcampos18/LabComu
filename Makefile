
all: server.exe

server.exe: server.c clientG.c clientdb.c
	gcc -o server server.c -lpthread
	gcc -o clientG clientG.c
	gcc -o clientdb clientdb.c `mysql_config --cflags --libs`

clean:
	 rm server.o clientG.o clientdb.o
