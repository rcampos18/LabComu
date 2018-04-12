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
    int bytes_read = sizeof(message);
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
	char linea[256];
	int na;
	char arr[5];
	char cdr[4];
	char c;
   	int cs= 0;
   	char rpl[1]; //respuesta de verificacion de error
   	system("test=$(base64 prueba.png > tes)");//convertir imagen a base64
   	//system("rm -r output");
    //FILE *file;
  	//file = fopen("tes", "r");
	//unsigned long fsize;
	FILE *out;
	out= fopen("output","a+t");
	/*if(out==NULL){
	    fclose(out);
	    out= fopen("output","a+t");
	}
	else{
	    fclose(out);
	    system("rm{output}");
	    out= fopen("output","a+t");
	}*/

	char line[10] = {0};
	char per[10]={0};

   while (1){ //Menu principal
      printf("Elige: \n1.Enviar mensaje \n 2.Enviar imagen \n 3.Salir ");
      fgets(line, sizeof(line), stdin);

      if (line[0] == '1'){
         printf("Elige a quien enviar el mensaje:\n");
         while(1){
         printf("Digita: \n1.Usuario A \n 2.Usuario B ");
         fgets(per, sizeof(per), stdin);

       if (per[0] == '1' || per[0] == '2' ){
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
        	if( recv(sock , server_reply , 256, 0) < 0){
            	puts("recv failed");
            	rpl[0]='n';
            	send(sock, rpl, 1, 0);
            	break;
        	}
        	cs=0;
            int re=0;
        	while(cs<77){
                if((server_reply[cs]=='\n')&&(cs<76)){
                	//server_reply[cs]=EOF;
                	//server_reply[cs]='\0';
                	//memmove(server_reply, server_reply, cs+2);
                	//printf("\ncs es: %i\n",cs);
                	fputc(EOF, out);
                	fclose(out);
                	system("com=$(base64 -d output > out.png)");
                	usleep(10000);
                	system("rm -r output");
                	break;
                }
                //re+=(server_reply[cs]-'0');
                fputc(server_reply[cs], out);
                cs+=1;
        	}
        	//re+=38;
        	/*printf("\nRE es:%d\n",re);
        	printf("SR es %c%c%c%c\n", server_reply[cs],server_reply[cs+1],server_reply[cs+2],server_reply[cs+3]);
        	sprintf(cdr, "%d", re); //creo arreglo para detectar errores
        	printf("CDR1 es %s\n", cdr);
        	if((cdr[0]==server_reply[cs])&&(cdr[1]==server_reply[cs+1])&&(cdr[2]==server_reply[cs+2])&&(cdr[3]==server_reply[na+3])){//Detecto igualdad con lo que recibo
        	    printf("El paquete llego bien");
        	}*/
        	
        	printf("\nReply:%s", server_reply);
        	//printf("\nCaracter:%i\n", server_reply[cs]);
        	rpl[0]='y';
            send(sock, rpl, 1, 0);
        	server_reply[0]='\0';
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
      else if(line[0] == '2'){
        printf("Elige a quien enviar la imagen:\n");
        printf("Digita: \n1.Usuario A \n 2.Usuario B ");
        fgets(per, sizeof(per), stdin);
        
      	if(per[0] == '1' || per[0] == '2'){
    	FILE *file;
      	file = fopen("tes", "r");
		while ((c=getc(file)) != EOF){
        	na=0;
        	int deter=0; //Para contar el valor de las variables y detectar errores
			while (c!='\n')//|| (c!=EOF))
			{
				linea[na]=c;//almacene datos en el arreglo
				//printf("este es el contenido de mensaje %c\n", c);
				na+=1;
				//int d = c-'0';
				//deter+=d;
				//usleep(100000);
				c= getc(file);
				if(c==EOF){
					printf("Hay un EOF");
				    break;
				}
				
			}
			    //printf("Deter es: %d\n", deter);
			    linea[na]=c;
			    //sprintf(arr, "%d", deter);
			    //linea[na+1]=arr[0];
			    //linea[na+2]=arr[1];
			    //linea[na+3]=arr[2];
			    //linea[na+4]=arr[3];
			    na+=1;
			    linea[na]='\0';
                //strcpy(buffer, linea);
				//if(prueba_error(buffer)==COMPLETE){
	        	if(send(sock, linea, 256, 0) < 0)
	        	    {
	        	        puts("Send failed");
	        	        return 1;
	        	    }
	        	while(1){//Para recibir la verificacion de error
	        	    if(FD_ISSET(sock, &r_set)){
        	            if( recv(sock , server_reply , 256, 0) < 0){
            	            puts("recv failed");
            	            send(sock, linea, 256, 0);
        	            }
        	            else{
        	                if((server_reply[0]=='y')){
        	                    printf("\nDatos enviados correctamente\n");
        	                    break;
        	                }
        	                else{
        	                    send(sock, linea, 256, 0);
        	                }
        	            }
	        	    }
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

	
