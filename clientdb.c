// gcc clientdb.c -o clientdb  `mysql_config --cflags --libs`
// ./clientdb

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
#include <pthread.h>                              //into
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

int buffer_message(char * message) {

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
    inbuf -= (where + 1);
    flag = 0;
  }
  // Step 4: update room and after, in preparation for the next read
  room = sizeof(message) - inbuf;
  after = message + inbuf;
  return flag;
}

int prueba_error(char * message) {
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
    inbuf -= (where + 1);
    flag = 0;
  }
  // Step 4: update room and after, in preparation for the next read
  room = sizeof(message) - inbuf;
  after = message + inbuf;
  return flag;
}

int find_network_newline(char * message, int bytes_inbuf) {
  int i;
  for (i = 0; i < inbuf; i++) {
    if ( *(message + i) == '\n')
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

int main(int argc , char *argv[]) {
  system ("./testdb");
  int sock, name, saldo, saldoA, saldoB, saldoC, iteration;
  struct sockaddr_in server;
  char message[256] , server_reply[256], buffer[256], message1[256], line[256], message2[256] ;   //server emisor: permite conocer quien transmite el mensaje

  //Create socket
  sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock == -1) {
    printf("Could not create socket");
  }

  puts("Socket created");

  //serv= gethostbyname(SOCKET_ADR);
  server.sin_addr.s_addr = inet_addr("127.0.0.1");// INADDR_ANY;54.215.182.161
  server.sin_family = AF_INET;
  server.sin_port = htons( 8034 );

  //Connect to remote server
  if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0) {
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
  for (;;) {
    r_set = all_set;
    //check to see if we can read from STDIN or sock
    select(maxfd, &r_set, NULL, NULL, &tv);

    if (FD_ISSET(sock, &r_set)) {
      //Receive a reply from the server
      if ( recv(sock , server_reply , 256 , 0) < 0)
      {
        puts("recv failed");
      }

      memmove(&server_reply[0], &server_reply[1], strlen(server_reply) - 0);
      if (server_reply[1] == 'S') {       //pedido de saldo

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
        saldoC = 0;
        printf("\n");
        while ((row = mysql_fetch_row(result)))
        {
          saldoC = atoi(row[0] ? row[1] : "NULL");

        }

        if (server_reply[2] == 'C') {
          printf("\tEl saldo de su cuenta es : %d para C > \n", saldoC);
          if (saldoC >0){
          strcpy(message1, "C$");
          sprintf(message1, "%d" , saldoC );
          strcat(message1, message2);
          printf("\nYour message is: %s\n", message1);
          if (send(sock, message1, strlen(message1) + 1, 0) < 0) //NOTE: we have to do strlen(message) + 1 because we MUST include '\0'
          {
            puts("Send failed");
            return 1;
          }
          }
          else if (saldoC <=0){
          strcpy(message1, "C&");
          sprintf(message1, "%d" , saldoC );
          strcat(message1, message2);
          printf("\nYour message is: %s\n", message1);
          if (send(sock, message1, strlen(message1) + 1, 0) < 0) //NOTE: we have to do strlen(message) + 1 because we MUST include '\0'
          {
            puts("Send failed");
            return 1;
          }
          }
        }
        
        else {
          strcpy(message1, " Invalid User ");
          sprintf(message2, "%c" , server_reply[2]);
          strcat(message1, message2);
          printf("\nYour message is: %s\n", message1);
          if (send(sock, message1, strlen(message1) + 1, 0) < 0) //NOTE: we have to do strlen(message) + 1 because we MUST include '\0'
          {
            puts("Send failed");
            return 1;
          }
        }
      }   //end consulta de saldo
      else if (server_reply[1] == 'U') {        //update, actualización de saldo(descarga) DSC en C

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
        int num;
        MYSQL_ROW row;
        char query1[999];

        while ((row = mysql_fetch_row(result)))
        {
          saldoC = atoi(row[0] ? row[1] : "NULL");

          memset(line, 0, sizeof(line));
          strcpy(line, server_reply);
          memmove(&line[0], &line[1], strlen(line) - 0);
     		printf("%s",line);
          memmove(&line[0], &line[1], strlen(line) - 0);
         		
          memmove(&line[0], &line[1], strlen(line) - 0);
          
          int val = atoi(line);

          if (server_reply[2] == 'C') {
            int saldoCn = saldoC - val;
            num = sprintf(query1, "UPDATE ClientS SET Saldo = '%d'  WHERE Name = 'ClientC' ;" ,
                          saldoCn);
            if (num > sizeof(query1))
            {
              printf("Error: Query too long.\n");
              exit (1);
            }
            if (mysql_query(con, query1))
            {
              printf("Error: mysql_query failed.");
              exit (1);
            }

            if (saldoCn <= 0) {
              strcpy(message1, "C&" );//no tiene saldo
              sprintf(message2, "%d ", saldoCn);
              strcat(message1, message2);
              if (send(sock, message1, strlen(message1) + 1, 0) < 0) //NOTE: we have to do strlen(message) + 1 because we MUST include '\0'
              {
                puts("Send failed");
                return 1;
              }
            }
            //else {
              printf(" El servicio C fue descargado exitosamente, el saldo de su cuenta nuevo es : %d ", saldoCn);
             
            
          }
          else {
            strcpy(message1, " Invalid User ");
            sprintf(message2, "%c" , server_reply[2]);
            strcat(message1, message2);
            printf("\nYour message is: %s\n", message1);
            if (send(sock, message1, strlen(message1) + 1, 0) < 0) //NOTE: we have to do strlen(message) + 1 because we MUST include '\0'
            {
              puts("Send failed");
              return 1;
            }
          }

        }
      } //end update saldo
      else if (server_reply[1] == 'R') {        //recarga de saldo

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
        int num;
        MYSQL_ROW row;
        char query1[999];

        while ((row = mysql_fetch_row(result)))
        {
          saldoC = atoi(row[0] ? row[1] : "NULL");
          printf("El valor de saldo que sera recargado es: %s \n", server_reply);
          memset(line, 0, sizeof(line));
          printf("Contenido del buffer en server es: %s \n", server_reply);
          strcpy(line, server_reply);
          memmove(&line[0], &line[1], strlen(line) - 0);
          memmove(&line[0], &line[1], strlen(line) - 0);
          memmove(&line[0], &line[1], strlen(line) - 0);
          int val = atoi(line);

          if (server_reply[2] == 'C') {
            saldoC = saldoC + val;
            num = sprintf(query1, "UPDATE ClientS SET Saldo = '%d'  WHERE Name = 'ClientC' ;" ,
                          saldoC);
            //           printf(" Paso 2 \n");
            if (num > sizeof(query1))
            {
              printf("Error: Query too long.\n");
              exit (1);
            }
            //                       printf(" Paso 2 \n");
            if (mysql_query(con, query1))
            {
              printf("Error: mysql_query failed.");
              exit (1);
            }
            //  printf(" El servicio C fue recargado exitosamente, el saldo de su cuenta nuevo es : %d \n", saldoC);
            strcpy(message1, "El servicio C fue recargado exitosamente, el saldo de su cuenta  es : ");
            sprintf(message2, "%d" , saldoC);
            strcat(message1, message2);
            printf("\nYour message is: %s\n", message1);
            if (send(sock, message1, strlen(message1) + 1, 0) < 0) //NOTE: we have to do strlen(message) + 1 because we MUST include '\0'
            {
              puts("Send failed");
              return 1;
            }

          }
          else {
            strcpy(message1, " Invalid User ");
            sprintf(message2, "%c" , server_reply[2]);
            strcat(message1, message2);
            printf("\nYour message is: %s\n", message1);
            if (send(sock, message1, strlen(message1) + 1, 0) < 0) //NOTE: we have to do strlen(message) + 1 because we MUST include '\0'
            {
              puts("Send failed");
              return 1;
            }
          }
        }

      }

      else {
        printf("Invalid Message: %s \n", server_reply);
      }  //Imprime cliente que intenta acceder
    }//end if
    else {
      //        printf("\t\t...\n");//en espera
      memset(server_reply, 0, sizeof(server_reply));
    }
  }

  close(sock);
  return 0;
}//end of main
