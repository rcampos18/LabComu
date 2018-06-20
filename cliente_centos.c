

#include <stdio.h> 		//printf
#include <string.h> 	//strlen
#include <stdlib.h>		//sistema
#include <sys/socket.h>  //socket
#include <arpa/inet.h> 	//inet_addr
#include <unistd.h>      
#include <errno.h>
#include <fcntl.h> 		//Usado para UART
#include <termios.h>	//Usado para UART
#include <time.h>
/*------------------------------------------------------------------------------------------
 ----------------------------Declaración de variables Constantes:---------------------------
 -------------------------------------------------------------------------------------------*/
#define COMPLETE 0
#define BUF_SIZE 100

/*------------------------------------------------------------------------------------------
 -----------------------------Declaración de variables globales:----------------------------
 -------------------------------------------------------------------------------------------*/
static int inbuf; // how many bytes are currently in the buffer?
static int room; // how much room left in buffer?
static char *after; // pointer to position after the received characters


int sock;
struct sockaddr_in server;
char tx_buffer[10];
char rx_buffer[10];
char reply[100];	//tamaño del buffer del mensaje y respuesta
char IDuser[1]= "3";
char linea[100];
int flagI=0;
int rcount;
char buffer[100];					//buffer para el uart

unsigned char buffer_sv[100];
unsigned char buffer_temporal[100];
int contador=0;					//contador de caracteres en lectura uart
int uart0_filestream = -1;
/*------------------------------------------------------------------------------------------
 -------------------Declaracion de prototipos de las funciones a utilzar:-------------------
 -------------------------------------------------------------------------------------------*/
//int buffer_message(char * message);
int find_network_newline(char * message, int bytes_inbuf);		
int crear_socket(void);								//Crea el socket para conexion
int enviar_msg(void);								//Para enviar el mensaje
int leer_msg(void);									//Para leer mensaje del servidor
int iniciar_UART(void);								//prototipo de la funcion UART
int leer_uart(void);

/********************************************************************************************
 ********************************************************************************************
 ****************************Inicio de la funcion principal(main)****************************
 ********************************************************************************************
 ********************************************************************************************/
int main(void)
{

	iniciar_UART();	//llama a la funcion que setea el UART	

	//Create socket
	if (crear_socket()==-1){printf("Error: No se pudo crear el socket!");}
	puts("Socket creado...");
	//Connect to remote server
	if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0){
	   perror("Error: Conexion fallida!");
	   return 1;
	}
	puts("Conectado!\n"); 
  	//Hasta acá ya estamos conectados con el servidor
  	
	//Esta parte es para poder asignar el socket del cliente al "nick" cuando conecta con el server:
	//Enviar identificador solo una vez al inicio de la ejecucion:
	if(send(sock, IDuser, strlen(IDuser) + 1, 0) < 0)
     {
	    return 1;
     }

	//set up variables for select()
	fd_set all_set, r_set;
	int maxfd = sock + 1;		//sock= socket del server---maxfd= +1 para empezar a asignar socket client
	FD_ZERO(&all_set);	
	FD_SET(STDIN_FILENO, &all_set); FD_SET(sock, &all_set);
	r_set = all_set;
	struct timeval tv; tv.tv_sec = 2; tv.tv_usec = 0;

	//set the initial position of after
	//after = message;
	
	//Mantener la comunicacion con el server:
	for(;;)
	{
			
	    	r_set = all_set;
	   		//check to see if we can read from STDIN or sock
	    	select(maxfd, &r_set, NULL, NULL, &tv);

	   	//Enviar mensaje:
	   	/*if(FD_ISSET(STDIN_FILENO, &r_set)) //Para enviar el mensaje:
		{	   	
		    	if(enviar_msg()==-1){puts("Error al enviar el mensaje");}
		    	puts("Mensaje enviado!");
	    	}*/
	    	//Leer mensaje:
	    	if(FD_ISSET(sock, &r_set))	//para recibir el mensaje del server...
		{
		    	if(leer_msg()==-1){puts("Error al leer el mensaje del servidor");}
		    	puts("Mensaje leido!");
		    	  	
		}	    
		leer_uart();			//llama a leer uart...	
	}//fin del for...
	close(sock);
	return 0;
}
/********************************************************************************************
 ****************************Final de la funcion principal(main)*****************************
 ********************************************************************************************/


//funcion para encontrar una nueva linea
int find_network_newline(char * message, int bytes_inbuf)
{
    int i;
    for(i = 0; i<inbuf; i++)
    {
        if( *(message + i) == '\n')
        return i;
    }
    return -1;
}//finaliza funcion encontrar nueva linea de red


//Crea el Socket con el servidor:
int crear_socket(void)
{
	sock = socket(AF_INET , SOCK_STREAM , 0);
	server.sin_addr.s_addr = inet_addr("192.168.43.122");//54.215.182.161
	if (sock == -1){return -1;}
	server.sin_port = htons( 8034 );
	server.sin_family = AF_INET;
	return 1;
}//Finaliza funcion crear socket


//Funcion para enviar mensaje al servidor
int enviar_msg(void)
{

		
	//strcat(message, "\0");		//copio el mensaje final en message---> message \0
	if(send(sock, buffer_sv, strlen(buffer_sv) + 1, 0) < 0)
	//NOTE: we have to do strlen(buffer_aws) + 1 because we MUST include '\0'
	{
		puts("Error:Fallo al enviar el mensaje!");
		return -1;							//retorne -1 si no puede enviar el mensaje
	}
	memset(buffer_sv,0,strlen(buffer_sv));				//limpia el bufer del mensaje
	//puts("Enter message del buffer: ");	    
		   											
}//finaliza la funcion enviar mensaje


//Funcion para leer mensaje del servidor:
int leer_msg(void)
{
	if( recv(sock, reply, sizeof reply, 0) < 0){
					puts("recv failed");
		}
		if (uart0_filestream != -1){
				int wcount =write(uart0_filestream, reply, sizeof(reply));
				usleep(200000);
				if (wcount < 0){
					printf("UART TX error\n");
				}
				memset(reply, 0, strlen(reply));
		}
			
}//termina la funcion leer mensaje


int iniciar_UART(void)
{
	
	uart0_filestream = open("/dev/ttyUSB0", O_RDWR | O_NOCTTY | O_NDELAY);		//Open in non blocking read/write mode
	if (uart0_filestream == -1)		//retorna -1 si no puede abrir el puerto usb...
	{
		//ERROR - CAN'T OPEN SERIAL PORT
		printf("Error - Unable to open UART.  Ensure it is not in use by another application\n");
	}

	
	struct termios options;
	tcgetattr(uart0_filestream, &options);
	options.c_cflag = B9600 | CS8 | CLOCAL | CREAD;		//<Set baud rate
	options.c_iflag = IGNPAR;
	options.c_oflag = 0;
	options.c_lflag = 0;
	tcflush(uart0_filestream, TCIFLUSH);
	tcsetattr(uart0_filestream, TCSANOW, &options);
	/*Fin de configurar UART*/
	return 0;
}

int leer_uart(void)
{
	//iniciar_UART();		//llama a la funcion que setea el UART	
	//----- CHECK FOR ANY RX BYTES -----
	if (uart0_filestream != -1)
	{
	//	usleep(500000);
		rcount = read(uart0_filestream, rx_buffer, 1);
		
		if (rcount > 0){
			printf("Recibido: %c\n", rx_buffer[0]);
			if(rx_buffer[0]=='A' || rx_buffer[0]=='B'){
			if(flagI==0){
					strcat(linea, rx_buffer);
					flagI=1;
			}
			else{
				memmove(&rx_buffer[0], &rx_buffer[1], strlen(rx_buffer)-0);
				strcat(linea, rx_buffer);
				if(rx_buffer[0]=='\n' || rx_buffer[0]==EOF){
					if(send(sock, linea, sizeof linea, 0) < 0)
					{
						puts("Error:Fallo al enviar el mensaje!");
					}
					flagI=0;
					memset(linea,0,strlen(linea));
				}
				printf("\nlongitud:%i \n",rcount);
			}
			
		}
		else{
		/*		if(send(sock, rx_buffer, sizeof rx_buffer, 0) < 0)
				{
					puts("Error:Fallo al enviar el mensaje!");
				}*/
				printf("Recibido else: %s\n", rx_buffer);
				memset(linea,0,strlen(linea));
			}	
			
		memset(rx_buffer,'\0',strlen(rx_buffer));		
		
		}	
	}		
}

//----- CLOSE THE UART -----
//close(uart0_filestream);







