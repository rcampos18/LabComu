#include <stdio.h> //printf
#include <string.h>    //strlen
#include <sys/socket.h>    //socket
#include <arpa/inet.h> //inet_addr
#include <unistd.h> 
#include <stdlib.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <time.h> //usleep
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

int prueba_error(char * message){
    int bytes_read=sizeof(message);
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
    int sock;
    struct sockaddr_in server;
    //char *message , *server_reply, *buffer;
    //message = (char *) malloc(256);
    //server_reply = (char *) malloc(256);
    char message[256] , server_reply[256], buffer[256];
    //Create socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
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
	//system("/home/gabino/Documentos/ProyectoLabComu/imgb64.sh");
	int na;
	char c;
   	int cs= 0;
   	char zero[256]={'\0'};
    FILE *file;
  	file = fopen("tes", "r");
	//unsigned long fsize;
	system("rm{output}");
	FILE *out;
	out= fopen("output","a+t");

	char line[10] = {0};
	char per[10]={0};

   while (1){ //Menu principal
      printf("Elige: \n1.Enviar mensaje \n 2.Enviar imagen \n 3.Salir ");
      fgets(line, sizeof(line), stdin);

      if (strlen(line) == 2 && (line[0] == '1')){
         printf("Elige a quien enviar el mensaje:\n");
         while(1){
         printf("Digita: \n1.Usuario A \n 2.Usuario B ");
         fgets(per, sizeof(per), stdin);

       if (strlen(per) == 2 && (per[0] == '1' || per[0] == '2' )){
        //puts("Enter message: ");
        for(;;){
    	r_set = all_set;
    	select(maxfd, &r_set, NULL, NULL, &tv);
    	
   		if(FD_ISSET(STDIN_FILENO, &r_set)){
   		
   		    if(buffer_message(message) == COMPLETE){
        	    //Send some data
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
        	if( recv(sock , server_reply , 77, 0) < 0){
            	puts("recv failed");
            	break;
        	}
        	cs=0;
        	while(cs<77){
                if((server_reply[cs]=='\n')&&(cs<76)){
                	//server_reply[cs]=EOF;
                	server_reply[cs]='\0';
                	//memmove(server_reply, server_reply, cs+2);
                	printf("\ncs es: %i\n",cs);
                	fputc(EOF, out);
                	fclose(out);
                	system("com=$(base64 -d output > out.png)");
                	usleep(10000);
                	break;
                }
                fputc(server_reply[cs], out);
                 
                cs+=1;
        	}
        	printf("\nReply:%s", server_reply);
        	printf("\nCaracter:%i\n", server_reply[cs]);
        	//server_reply[0]='\0';
        	memmove(server_reply, server_reply, 256-cs);
        	strcpy(server_reply, zero);
        	//usleep(10000);
    	}
      	}
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
        //while(1){
        printf("Digita: \n1.Usuario A \n 2.Usuario B ");
        fgets(per, sizeof(per), stdin);
        
      	if(strlen(per) == 2 && (per[0] == '1' || per[0] == '2')){
    	r_set = all_set;
    
   		//check to see if we can read from STDIN or sock
        //select(maxfd, &r_set, NULL, NULL, &tv);
		while ((c=getc(file)) != EOF){
        	na=0;
			while (c!='\n')//|| (c!=EOF))
			{
				buffer[na]=c;//almacene datos en el arreglo
				//printf("este es el contenido de mensaje %c\n", c);
				na+=1;
				//usleep(100000);
				c= getc(file);
				if(c==EOF){
					printf("Hay un EOF");
				    break;
				}
				
			}
			    //printf("c tiene %i\n", c);
			    buffer[na]=c;
			    //na+=1;
			    //buffer[0]= na +'0'; //longitud de linea

				//if(prueba_error(buffer)==COMPLETE){
	        	if(send(sock, buffer, 77, 0) < 0)// ::NOTE: we have to do strlen(message) + 1 because we MUST include '\0'
	        	    {
	        	        puts("Send failed");
	        	        return 1;
	        	    }
        	//}
        	}
		fclose(file);
    	}      
      	else{
         printf("Error, intenta de nuevo...\n\n");

         const size_t pos = sizeof(per) - 1; 
         if (strlen(per) == pos && per[pos] != '\n') while (fgetc(stdin) != '\n');
      	}
      	
      	//}	
      }
      else
      {
         printf("Bad input; try again...\n\n");

         const size_t pos = sizeof(line) - 1; 
         if (strlen(line) == pos && line[pos] != '\n') while (fgetc(stdin) != '\n');
      }
   }

    	close(sock);
		return 0;
		//end of main
}

	
