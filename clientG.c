#include <stdio.h> //printf
#include <string.h>    //strlen
#include <sys/socket.h>    //socket
#include <arpa/inet.h> //inet_addr
#include <unistd.h> 
#include <stdlib.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <pthread.h>

//directives are above (e.g. #include ...)

//message buffer related delcartions/macros
int buffer_message(char * message);
int find_network_newline(char * message, int inbuf);

#define COMPLETE 0
#define BUF_SIZE 256
#define SOCKET_PORT "5000"
#define SOCKET_ADR "localhost"
#define filename "tes" //ubicacion de archivo de texto de imagen


static int inbuf; // how many bytes are currently in the buffer?
static int room; // how much room left in buffer?
static char *after; // pointer to position after the received characters
//main starts below


void error(const char *msg){
    perror(msg);
    exit(0);
}

int buffer_message(char * message){

    //read(STDIN_FILENO, after, 256 - inbuf);
    short flag = -1; // indicates if returned_data has been set 
    inbuf += 77;
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
    int sock;
    struct sockaddr_in server; //hostent *serv;
    char message[256] , server_reply[256];
	
	//signal (SIGCHLD, SIG_IGN);
    //Create socket
    sock = socket(AF_INET , SOCK_STREAM , 0);
    if (sock == -1){
        printf("Could not create socket");
    }
    puts("Socket created");
    
    //serv= gethostbyname(SOCKET_ADR);
    server.sin_addr.s_addr = inet_addr("127.0.0.1");// INADDR_ANY;
    server.sin_family = AF_INET;
    server.sin_port = htons( 9034 );

    //Connect to remote server
    if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0){
        perror("connect failed. Error");
        return 1;
    }
    
	//listen(sock,5);
   //insert the code below into main, after you've connected to the server
	puts("Connected\n");    

//set up variables for select()
	fd_set all_set, r_set;
	int maxfd = sock + 1;
	FD_ZERO(&all_set);	
	FD_SET(STDIN_FILENO, &all_set); FD_SET(sock, &all_set);
	r_set = all_set;
	struct timeval tv; tv.tv_sec = 2; tv.tv_usec = 0;

	//set the initial position of after
	after = message;
	//char linea[256];
	char buffer[256];//={0};
	int na;
	char c;
	FILE *file;
	file = fopen("tes", "r");
	//unsigned long fsize;
	char chs;//='0';
	//chs = getchar();

	char line[10] = {0};
	char per[10]={0};

   while (1){ //Menu principal
      printf("Elige: \n1.Enviar mensaje \n 2.Enviar imagen \n 3.Salir ");
      fgets(line, sizeof(line), stdin);

      if (strlen(line) == 2 && (line[0] == '1'))
      {
         printf("Elige a quien enviar el mensaje:\n");
         while(1){
         printf("Digita: \n1.Usuario A \n 2.Usuario B ");
         fgets(per, sizeof(per), stdin);

         if (strlen(per) == 2 && (per[0] == '1' || per[0] == '2' )){
         //printf("You did it\n");
         
         //continue;
      	}
      else{
         printf("Error, intenta de nuevo...\n\n");

         const size_t pos = sizeof(per) - 1; 
         if (strlen(per) == pos && per[pos] != '\n') while (fgetc(stdin) != '\n');
      }
     }
      }
      else if(strlen(line) == 2 && (line[0] == '2')){
      printf("Elige a quien enviar la imagen:\n");
        while(1){
        printf("Digita: \n1.Usuario A \n 2.Usuario B ");
        fgets(per, sizeof(per), stdin);
      	if(strlen(per) == 2 && (per[0] == '1' || per[0] == '2')){
			
		for(;;){
    	r_set = all_set;
   		//check to see if we can read from STDIN or sock
    		//select(maxfd, &r_set, NULL, NULL, &tv);
	
		while ((c= getc(file)) != EOF){
        	na=0;
			while (c != '\n'){
				buffer[na]=c;//char *strncpy(char *buffer[na], char *c,size_t 1);
				//printf("este es el contenido de mensaje %c\n", buffer[na]);
				na+=1;
				//usleep(500000);
				c= getc(file);
			}
				buffer[na]=c;
				printf("este es el contenido de mensaje %s\t", buffer);
				//Send some data
				send(sock, buffer, strlen(buffer), 0);
	        	   //if(send(sock, buffer, strlen(buffer), 0) < 0)//sock, message, strlen(message) + 1, 0) < 0) ::NOTE: we have to do strlen(message) + 1 because we MUST include '\0'
	        	    //{
	        	    //    puts("Send failed");
	        	    //    return 1;
	        	    //}
        	}

		fclose(file);
    	}
    	}      
      	else{
         printf("Error, intenta de nuevo...\n\n");

         const size_t pos = sizeof(per) - 1; 
         if (strlen(per) == pos && per[pos] != '\n') while (fgetc(stdin) != '\n');
      	}
      	
      	}	
      }
      else
      {
         printf("Bad input; try again...\n\n");

         const size_t pos = sizeof(line) - 1; 
         if (strlen(line) == pos && line[pos] != '\n') while (fgetc(stdin) != '\n');
      }
   }

	/*if (file == NULL) {
    	printf("File not found!\n");
    	return 1;
	}
	else {
    	fseek(file, 0, SEEK_END);
    	fsize = ftell(file);
    	rewind(file);

    	printf("File contains %ld bytes!\n", fsize);
    	//printf("Sending the file now");
	}*/
	//keep communicating with server

	
	
	for(;;){
		//void *p = buffer;
    	r_set = all_set;
   		//check to see if we can read from STDIN or sock
    	select(maxfd, &r_set, NULL, NULL, &tv);
	
	//int bytes_read = read(c=getc(file), buffer,fsize);
	

    	/*if (bytes_read == 0) // We're done reading from the file
        	break;

	    if (bytes_read < 0){
	        error("ERROR reading from file, too bad"); 
	    }*/
    	
   		//if(FD_ISSET(STDIN_FILENO, &r_set)){
		while ((c= getc(file)) != EOF){

        		//if(buffer_message(c) == COMPLETE){
        	na=0;
			while (c != '\n'){
				buffer[na]=c;
				printf("este es el contenido de mensaje %c\n", buffer[na]);
				na+=1;
				//usleep(500000);
				c= getc(file);
			}
				buffer[na]=c;
				//Send some data
	        	   if(send(sock, buffer, strlen(buffer)+1, 0) < 0)//sock, message, strlen(message) + 1, 0) < 0) ::NOTE: we have to do strlen(message) + 1 because we MUST include '\0'
	        	    {
	        	        puts("Send failed");
	        	        return 1;
	        	    }	
	        	    //puts("Enter message:");
        	}
        	/*while (bytes_read > 0){
        		int bytes_written = write(sock, buffer, bytes_read);
		        if (bytes_written <= 0){
		            error("ERROR writing to socket\n");
        		}
        		bytes_read -= bytes_written;
        		p += bytes_written;
    		}*/
		fclose(file);
    	}
		
    	if(FD_ISSET(sock, &r_set)){
        	//Receive a reply from the server
        	if( recv(sock , server_reply , 78 , 0) < 0){
        		
            	puts("recv failed");
            	//break;
        	}

        	printf("\nServer Reply: %s\n", server_reply);
        	server_reply[0]='\0';

    	}
    	close(sock);
		return 0;
		//end of main
}

	
