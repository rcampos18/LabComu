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
#include <pthread.h>															//into
#include <my_global.h>
#include <mysql.h>

//directives are above (e.g. #include ...)

//message buffer related declarations/macros
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
void finish_with_error(MYSQL *con)
{
  fprintf(stderr, "%s\n", mysql_error(con));
  mysql_close(con);
  exit(1);        
}

int main(int argc , char *argv[]){
	system ("./testdb");
   int sock, name, saldo ;
    struct sockaddr_in server;
    char message[256] , server_reply[256],buffer[256],message1[256], line[256] ; 			//server emisor: permite conocer quien transmite el mensaje

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
	for(;;){	usleep(10000000);
				r_set = all_set;
			   	//check to see if we can read from STDIN or sock
				select(maxfd, &r_set, NULL, NULL, &tv);
		 	
				if(FD_ISSET(sock, &r_set)){
				//Receive a reply from the server
					if( recv(sock , server_reply , 256 , 0) < 0)
					{
						puts("recv failed");
					}
					
					if (server_reply[1]=='S') {					//pedido de saldo
								
									MYSQL *con = mysql_init(NULL);
									if (mysql_real_connect(con, "localhost", "root", "7855", 
											  NULL, 0, NULL, 0) == NULL) 
									  {
										  fprintf(stderr, "%s\n", mysql_error(con));
										  mysql_close(con);
										  exit(1);
									  }
									if (mysql_query(con, "USE testdb")) 
									  {
										  fprintf(stderr, "%s\n", mysql_error(con));
										  mysql_close(con);
										  exit(1);
									  }  
								   if (mysql_query(con, "SELECT * FROM ClientS")) {
									  finish_with_error(con);
								  }

								MYSQL_RES *result = mysql_store_result(con);
								  
								  if (result == NULL) 
								  {
									  finish_with_error(con);
								  }

								  int num_fields = mysql_num_fields(result);

								  MYSQL_ROW row;
								  
								  while ((row = mysql_fetch_row(result))) 
								  { 
									  for(int i = 0; i < num_fields; i++) 
									  { 
										  printf("%s ", row[i] ? row[i] : "NULL"); 
									  } 
										  printf("\n"); 
										  saldo = atoi(row[1]);
										  printf(" el saldo de su cuenta es : %d ",saldo);
										  sprintf(message1, "S%d", saldo);
										  printf("%s",message1);
								  }
								  
								 

								if(send(sock, message1, strlen(message1) + 1, 0) < 0)//NOTE: we have to do strlen(message) + 1 because we MUST include '\0'
									{
										puts("Send failed");
										return 1;
									}
								
								
								}		
					else if (server_reply[1]=='U') {					//update, actualización de saldo
								
									MYSQL *con = mysql_init(NULL);
									if (mysql_real_connect(con, "localhost", "root", "7855", 
											  NULL, 0, NULL, 0) == NULL) 
									  {
										  fprintf(stderr, "%s\n", mysql_error(con));
										  mysql_close(con);
										  exit(1);
									  }
									if (mysql_query(con, "USE testdb")) 
									  {
										  fprintf(stderr, "%s\n", mysql_error(con));
										  mysql_close(con);
										  exit(1);
									  }  
								 //  memmove(&server_reply[0], &server_reply[1], strlen(server_reply)-0);
								 //  memmove(&server_reply[0], &server_reply[1], strlen(server_reply)-0);
								   printf("El valor de saldo fue actualizado a: %s",server_reply);
								 		 memset(line, 0, sizeof(line));
										printf("Contenido del buffer en server es: %s \n", server_reply);
										strcpy(line, server_reply);
										memmove(&line[0], &line[1], strlen(line)-0);
										memmove(&line[0], &line[1], strlen(line)-0);
									
										int val = atoi(line);
										printf("\t\tValor es: %d", val);
										printf("\n");
								 if (mysql_query(con, "SELECT * FROM ClientS")) {
									  finish_with_error(con);
								  }
									MYSQL_RES *result = mysql_store_result(con);
								  
								  if (result== NULL) 
								  {
									  finish_with_error(con);
								  }

								  int num_fields = mysql_num_fields(result);

								  MYSQL_ROW row;
								  
								  while ((row = mysql_fetch_row(result))) 
								  { 
									  for(int i = 0; i < num_fields; i++) 
									  { 
										  printf("%s ", row[i] ? row[i] : "NULL"); 
									  } 
										  printf("\n"); 
										  saldo = atoi(row[1]);
										  printf(" el saldo de su cuenta era : %d ",saldo);
										  saldo=saldo-val;
										  printf(" el saldo de su cuenta nuevo es : %d ",saldo);
										
								  }
								    
								   if (mysql_query(con, "UPDATE ClientS SET Saldo = saldo  WHERE Name = 'ClientC' ")) 												{      
										  finish_with_error(con);
										  }
									printf(" El servicio C fue descargado exitosamente, el saldo de su cuenta nuevo es : %d ",saldo);	 
								}		
					else if (server_reply[1]=='R') {					//update, actualización de saldo
								
									MYSQL *con = mysql_init(NULL);
									if (mysql_real_connect(con, "localhost", "root", "7855", 
											  NULL, 0, NULL, 0) == NULL) 
									  {
										  fprintf(stderr, "%s\n", mysql_error(con));
										  mysql_close(con);
										  exit(1);
									  }
									if (mysql_query(con, "USE testdb")) 
									  {
										  fprintf(stderr, "%s\n", mysql_error(con));
										  mysql_close(con);
										  exit(1);
									  }  
								 //  memmove(&server_reply[0], &server_reply[1], strlen(server_reply)-0);
								 //  memmove(&server_reply[0], &server_reply[1], strlen(server_reply)-0);
								   printf("El valor de saldo fue actualizado a: %s",server_reply);
								 		 memset(line, 0, sizeof(line));
										printf("Contenido del buffer en server es: %s \n", server_reply);
										strcpy(line, server_reply);
										memmove(&line[0], &line[1], strlen(line)-0);
										memmove(&line[0], &line[1], strlen(line)-0);
									
										int val = atoi(line);
										printf("\t\tValor es: %d", val);
										printf("\n");
								 if (mysql_query(con, "SELECT * FROM ClientS")) {
									  finish_with_error(con);
								  }
									MYSQL_RES *result = mysql_store_result(con);
								  
								  if (result== NULL) 
								  {
									  finish_with_error(con);
								  }

								  int num_fields = mysql_num_fields(result);

								  MYSQL_ROW row;
								  
								  while ((row = mysql_fetch_row(result))) 
								  { 
									  for(int i = 0; i < num_fields; i++) 
									  { 
										  printf("%s ", row[i] ? row[i] : "NULL"); 
									  } 
										  printf("\n"); 
										  saldo = atoi(row[1]);
										  printf(" el saldo de su cuenta era : %d ",saldo);
										  saldo=saldo-val;
										  printf(" el saldo de su cuenta nuevo es : %d ",saldo);
										
								  }
								    
								   if (mysql_query(con, "UPDATE ClientS SET Saldo = Saldo-10  WHERE Name = 'ClientC' ")) {      
										  finish_with_error(con);
										  }
									printf(" El servicio C fue recargado exitosamente");	 
								}					
					else{
						printf("TX invalido: %c \n", server_reply[1]); }	//Imprime cliente que intenta acceder 
					
					//	memset(server_reply, 0, sizeof(server_reply));							
					}//end if		
	  	else{
				printf("\t\tEspera...\n\n\n\n");
			}
	}

    	close(sock);
		return 0;
		
}//end of main
