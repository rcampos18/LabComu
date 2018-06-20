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

//message buffer related declarations/macros
int buffer_message(char * message);
int find_network_newline(char * message, int inbuf);

#define COMPLETE 0
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
   int sock, name;
    struct sockaddr_in server;
    char message[256] , server_reply[256],buffer[256], server_emisor[256],message1[256]; 			//server emisor: permite conocer quien transmite el mensaje
	char message2[256]; 
    //Create socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1){
        printf("Could not create socket");
    }
   
    puts("Socket created");
    
    //serv= gethostbyname(SOCKET_ADR);
    server.sin_addr.s_addr = inet_addr("54.215.182.161");//54.215.182.161
    server.sin_family = AF_INET;
    server.sin_port = htons( 9034 );

    //Connect to remote server
    if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0){
        perror("connect failed. Error");
        return 1;
    }

   //insert the code below into main, after you've connected to the server
    
    //pasar el argumento obtenido en la entrada del cliente a un valor numerico
	//1. A, 2. B, 3. C, 4. D
	strncat(message, argv[1], 1);
    send(sock, message, strlen(message) + 1, 0);//envia identificador
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
	system("> output");
	FILE *out;
	out= fopen("output","a+t");
	char line[10] = {0};
	char per[10]={0};
	int cs;
	while (1){ //Menu principal
		int count=1;
		printf("\t\t*********************************\n");		
		printf("\t\tBienvenido al menu principal: \n");
		printf("\t\tElige: \n\t\t1.Recibir imagen \n\t\t2.Enviar imagen a A \n\t\t3.Enviar imagen a B \n\t\t4.Recarga \n\t\tq.Salir \n\n\n\n");
		fgets(line, sizeof(line), stdin);
		//usleep(1000000);
		//scanf("%s", line);

	if (strlen(line) == 2 && (line[0] == '1')){   //Recibir imagen
		FILE *out;
		out= fopen("output","a+t");
		int detEOF=0;
		while(detEOF==0){	
			if(FD_ISSET(sock, &r_set)){
		    	//Receive a reply from the server
		    	if( recv(sock , server_reply , 256, 0) < 0){
		        	puts("recv failed");
		        	break;
		    	}
		    	printf("LLega esto: %s\n",server_reply);
		    	memmove(&server_reply[0], &server_reply[1], strlen(server_reply)-0);
		    	cs=0;
		    	while(cs<77){
	            	fputc(server_reply[cs], out);
		        	fclose(out);
		        	system("com=$(base64 -d output > out.png)");
		        	detEOF=1;
		        	break;
	           		cs+=1;
        	}
        	//printf("\nCaracter:%i\n", server_reply[cs]);
        	memset(server_reply,0,strlen(server_reply));
    	}
    }	
}//end mensaje Usuario A

	else if (strlen(line) == 2 && (line[0] == '2')){ //USUARIO A
		printf("\t\tPulse el mensaje que quiere enviar al usuario B \n\n\n\n"); //DEBE DIGITARSE 1 O 2  	
			printf("\t\t*********************************\n");	
			//int count=1;			
			//for(;;){
			if(count==1){
				usleep(10000000);
				r_set = all_set;
			   	//check to see if we can read from STDIN or sock
				select(maxfd, &r_set, NULL, NULL, &tv);
			
			   	if(FD_ISSET(STDIN_FILENO, &r_set)){
					if(buffer_message(message) == COMPLETE){
					//Send some data
						printf("Este es el contenido del mensaje %s\n ", message);
						if (message == "q"){
							printf("\n\t\t Saliendo ...");
							count= 0;
						}
						else{
							memset(message1, 0, sizeof(message1));
							message1[0]='A';
							strcat(message1, message);
							printf("Este es el contenido del mensaje acoplado %s\n ", message1);
							if(send(sock, message1, strlen(message1) + 1, 0) < 0)//NOTE: we have to do strlen(message) + 1 because we MUST include '\0'
							{
								puts("Send failed");
								return 1;
							}
							puts("Enter message:");
						}
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
				//memmove(&server_reply[0], &server_reply[1], strlen(server_reply)-0);
					if (server_emisor[0]=='1') {					//compara si el emisor es 4
						server_emisor[0]='A';					//asigna valor A al emisor para impresion
						printf("TX es : %s \n", server_emisor); }		
					else if (server_emisor[0]=='2'){				//compara si el emisor es 5
						server_emisor[0]='B';					//asigna valor B al emisor para impresion
						printf("TX es : %s \n", server_emisor);  }
					else if (server_emisor[0]=='3'){				//compara si el emisor es 6
						server_emisor[0]='C';					//asigna valor C al emisor para impresion
						printf("TX es : %s \n", server_emisor);  }
					else if (server_emisor[0]=='4') {				//compara si el emisor es 7
						server_emisor[0]='D';					//asigna valor D al emisor para impresion
						printf("TX es : %s \n", server_emisor);  }
					else{
						printf("TX es incorrecto: %s \n", server_emisor); }	//REVISA QUE EL SERVIDOR ES IN

					printf("\nYour message is: %s\n", server_reply);
						//server_reply[0]='\0';
						//server_emisor=0;
						memset(server_reply, 0, sizeof(server_reply));
						memset(server_emisor, 0, sizeof(server_emisor));
							}
						}//end if
		else{
			break;
	}
}

	else if (strlen(line) == 2 && (line[0] == '3')){ //USUARIO B
		printf("\t\tPulse el mensaje que quiere enviar al usuario B \n\n\n\n"); //DEBE DIGITARSE 1 O 2  	
			printf("\t\t*********************************\n");	
			//int count=1;			
			//for(;;){
			if(count==1){
				usleep(10000000);
				r_set = all_set;
			   	//check to see if we can read from STDIN or sock
				select(maxfd, &r_set, NULL, NULL, &tv);
			
			   	if(FD_ISSET(STDIN_FILENO, &r_set)){
					if(buffer_message(message) == COMPLETE){
					//Send some data
						printf("Este es el contenido del mensaje %s\n ", message);
						if (message == "q"){
							printf("\n\t\t Saliendo ...");
							count= 0;
						}
						else{
							puts("Enter message:");
						}
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
				//memmove(&server_reply[0], &server_reply[1], strlen(server_reply)-0);
					if (server_emisor[0]=='1') {					//compara si el emisor es 4
						server_emisor[0]='A';					//asigna valor A al emisor para impresion
						printf("TX es : %s \n", server_emisor); }		
					else if (server_emisor[0]=='2'){				//compara si el emisor es 5
						server_emisor[0]='B';					//asigna valor B al emisor para impresion
						printf("TX es : %s \n", server_emisor);  }
					else if (server_emisor[0]=='3'){				//compara si el emisor es 6
						server_emisor[0]='C';					//asigna valor C al emisor para impresion
						printf("TX es : %s \n", server_emisor);  }
					else if (server_emisor[0]=='4') {				//compara si el emisor es 7
						server_emisor[0]='D';					//asigna valor D al emisor para impresion
						printf("TX es : %s \n", server_emisor);  }
					else{
						printf("TX es incorrecto: %s \n", server_emisor); }	//REVISA QUE EL SERVIDOR ES IN

					printf("\nYour message is: %s\n", server_reply);
						//server_reply[0]='\0';
						//server_emisor=0;
						memset(server_reply, 0, sizeof(server_reply));
						memset(server_emisor, 0, sizeof(server_emisor));
							}
						}//end if
		else{
			break;
	}
}
	
	else if (strlen(line) == 2 && (line[0] == '4')){
		printf("\t\tEntrando al sistema de recarga de cliente...\n\n\n\n\n\n");
		printf("\t\tDigite el monto a recargar: \n\t\t1.1000 \n\t\t2.2000 \n\t\t3.Otro\n\n\n\n");
			fgets(per, sizeof(per), stdin); 
			if (strlen(per) == 2 && per[0] == '1' ){//mil
				printf("\t\tAplicando recarga de 1000 al Celular \n");
				strcpy(message1, "DRC1000 ");
				if (send(sock, message1, strlen(message1) + 1, 0) < 0) //NOTE: we have to do strlen(message) + 1 because we MUST include '\0'
				{
				    puts("Send failed");
				    return 1;
				}
			}
			else if (strlen(per) == 2 && per[0] == '2' ){//2 mil
				printf("\t\tAplicando recarga de 2000 al Celular \n");
				strcpy(message1, "DRC2000 ");
				if (send(sock, message1, strlen(message1) + 1, 0) < 0) //NOTE: we have to do strlen(message) + 1 because we MUST include '\0'
				{
				    puts("Send failed");
				    return 1;
				}
			}
			else if (strlen(per) == 2 && per[0] == '3' ){//Otro 
			printf("\t\tDigite el monto a recargar al Celular sin espacios ni signos ej: 1500\n\n\n\n");
			fgets(per, sizeof(per), stdin); 
					printf("\t\tAplicando recarga de %s al Celular \n", per);
					strcpy(message1, "DRC");//DUC100 (saldo-100) DSC (saldo)
				   	sprintf(message2, "%s" , per);
					strcat(message1, message2);
					if (send(sock, message1, strlen(message1) + 1, 0) < 0) //NOTE: we have to do strlen(message) + 1 because we MUST include '\0'
				{
				    puts("Send failed");
				    return 1;
				}
			}
			else{
			printf("\t\tHubo un error de digitacion, vuelva a intentarlo mas tarde...\n\n");
			}
}	
	else if (strlen(line) == 2 && line[0] == 'q'){
		printf("\t\tCerrando cliente...\n\n\n\n\n\n");
		close(sock);
		return 0;
	}	
	else{
		printf("\t\tBad input; try again...\n\n");
    }
   }//FIN MENU

    	close(sock);
		return 0;
		
}//end of main
