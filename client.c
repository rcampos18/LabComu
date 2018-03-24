#include <stdio.h> //printf
#include <string.h>    //strlen
#include <sys/socket.h>    //socket
#include <arpa/inet.h> //inet_addr
#include <unistd.h> 
#include <stdlib.h>
#include <errno.h>

//directives are above (e.g. #include ...)

//message buffer related delcartions/macros
int buffer_message(char * message);
int find_network_newline(char * message, int inbuf);
#define COMPLETE 0
#define BUF_SIZE 256

static int inbuf; // how many bytes are currently in the buffer?
static int room; // how much room left in buffer?
static char *after; // pointer to position after the received characters
//main starts below

int buffer_message(char * message){

    int bytes_read = read(STDIN_FILENO, after, 256 - inbuf);
    short flag = -1; // indicates if returned_data has been set 
    inbuf += bytes_read;
    int where; // location of network newline

    // Step 1: call findeol, store result in where
    where = find_network_newline(message, inbuf);
    if (where >= 0) { // OK. we have a full line

        // Step 2: place a null terminator at the end of the string
        char * null_c = {'\0'};
        memcpy(message + where, &null_c, 1); 

        // Step 3: update inbuf and remove the full line from the clients's buffer
        memmove(message, message + where + 1, inbuf - (where + 1)); 
        inbuf -= (where+1);
        flag = 0;
    }

    // Step 4: update room and after, in preparation for the next read
    room = sizeof(message) - inbuf;
    after = message + inbuf;

    return flag;
}

int find_network_newline(char * message, int bytes_inbuf){
    int i;
    for(i = 0; i<inbuf; i++){
        if( *(message + i) == '\n')
        return i;
    }
    return -1;
}

int main(int argc , char *argv[]){
    int sock, name;
    struct sockaddr_in server;
    char message[256] , server_reply[256], server_emisor[256]; 			//server emisor: permite conocer quien transmite el mensaje

	
    //Create socket
    sock = socket(AF_INET , SOCK_STREAM , 0);
    if (sock == -1)
    {
        printf("Could not create socket");
    }
    puts("Socket created");


    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_family = AF_INET;
    server.sin_port = htons( 9034 );

    //Connect to remote server
    if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0){
        perror("connect failed. Error");
        return 1;
    }

   //insert the code below into main, after you've connected to the server
	puts("Connected\n");

//set up variables for select()
	fd_set all_set, r_set;
	int maxfd = sock + 1;
	FD_ZERO(&all_set);	
	FD_SET(STDIN_FILENO, &all_set); FD_SET(sock, &all_set);
	r_set = all_set;
	struct timeval tv; tv.tv_sec = 2; tv.tv_usec = 0;



	send(sock, argv[1], 1 , 0);//enviar el nombre de socket(1.A 2.B 3.C 4.D)



	
	after = message;

	puts("Enter message: ");
	//keep communicating with server
	for(;;){

    	r_set = all_set;
   		//check to see if we can read from STDIN or sock
    	select(maxfd, &r_set, NULL, NULL, &tv);

   		if(FD_ISSET(STDIN_FILENO, &r_set)){

        	if(buffer_message(message) == COMPLETE){
        	    //Send some data
		printf("Este es el contenido del mensaje %s\n ", message);
        	    if(send(sock, message, strlen(message) + 1, 0) < 0)//NOTE: we have to do strlen(message) + 1 because we MUST include '\0'
        	    {
        	        puts("Send failed");
        	        return 1;
        	    }

        	    puts("Enter message:");
        	}
    	}

    	if(FD_ISSET(sock, &r_set)){
        	//Receive a reply from the server
        	if( recv(sock , server_reply , 256 , 0) < 0)
        	{
            	puts("recv failed");
            	break;
        	}
		strcpy(server_emisor, server_reply);
		server_emisor[1]='\0';
		memmove(&server_reply[0], &server_reply[1], strlen(server_reply)-0);
		memmove(&server_reply[0], &server_reply[1], strlen(server_reply)-0);
		
		if (server_emisor[0]=='4') {					//compara si el emisor es 4
			server_emisor[0]='A';					//asigna valor A al emisor para impresion
			printf("TX es : %s \n", server_emisor); }		
		else if (server_emisor[0]=='5'){				//compara si el emisor es 5
			server_emisor[0]='B';					//asigna valor B al emisor para impresion
			printf("TX es : %s \n", server_emisor);  }
		else if (server_emisor[0]=='6'){				//compara si el emisor es 6
			server_emisor[0]='C';					//asigna valor C al emisor para impresion
			printf("TX es : %s \n", server_emisor);  }
		else if (server_emisor[0]=='7') {				//compara si el emisor es 7
			server_emisor[0]='D';					//asigna valor D al emisor para impresion
			printf("TX es : %s \n", server_emisor);  }
		else{
			printf("TX es incorrecto: %s \n", server_emisor); }	//REVISA QUE EL SERVIDOR ES IN

		printf("\nYour message is: %s\n", server_reply);
        	server_reply[0]='\0';

    	}
	}

	close(sock);
	return 0;
	//end of main

}
