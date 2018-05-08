
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define PORT "9034"   // port we're listening on

int A,B,C,D, client_n, ident ;
char a[256], b[256], c[256], d[256], client_name[6]; //punteros

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa){
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

//message buffer related declarations/macros
int buffer_message(char * message);
int find_network_newline(char * message, int inbuf);
#define COMPLETE 0
#define BUF_SIZE 256

static int inbuf; // how many bytes are currently in the buffer?
static int room; // how much room left in buffer?
static char *after; // pointer to position after the received characters
//main starts below

int main(void){
    fd_set master;    // master file descriptor list
    fd_set read_fds;  // temp file descriptor list for select()
    int fdmax;        // maximum file descriptor number

    int listener;     // listening socket descriptor
    int newfd;        // newly accept()ed socket descriptor
    struct sockaddr_storage remoteaddr; // client address
    socklen_t addrlen;

    char buf[256], em[12] ,line[256] ;    // buffer for client data
    int nbytes, rx, tx;

    char remoteIP[INET6_ADDRSTRLEN];

    int yes=1;        // for setsockopt() SO_REUSEADDR, below
    int i, j, rv;

    struct addrinfo hints, *ai, *p;

    FD_ZERO(&master);    // clear the master and temp sets
    FD_ZERO(&read_fds);
	printf("Waiting for Clients \n");
    // get us a socket and bind it
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    if ((rv = getaddrinfo(NULL, PORT, &hints, &ai)) != 0) {
        fprintf(stderr, "selectserver: %s\n", gai_strerror(rv));
        exit(1);
    }

    for(p = ai; p != NULL; p = p->ai_next) {
        listener = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (listener < 0) { 
            continue;
        }

        // lose the pesky "address already in use" error message
        setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

        if (bind(listener, p->ai_addr, p->ai_addrlen) < 0) {
            close(listener);
            continue;
        }

        break;
    }

    // if we got here, it means we didn't get bound
    if (p == NULL) {
        fprintf(stderr, "selectserver: failed to bind\n");
        exit(2);
    }

    freeaddrinfo(ai); // all done with this

    // listen
    if (listen(listener, 10) == -1) {
        perror("listen");
        exit(3);
    }

    // add the listener to the master set
    FD_SET(listener, &master);

    // keep track of the biggest file descriptor
    fdmax = listener; // so far, it's this one

    // main loop
    for(;;) {
        read_fds = master; // copy it
        if (select(fdmax+1, &read_fds, NULL, NULL, NULL) == -1) {
            perror("select");
            exit(4);
        }

        // run through the existing connections looking for data to read
        for(i = 0; i <= fdmax; i++) {
            if (FD_ISSET(i, &read_fds)) { // we got one!!
                if (i == listener) {
                    // handle new connections
                    addrlen = sizeof remoteaddr;
                    newfd = accept(listener,
                        (struct sockaddr *)&remoteaddr,
                        &addrlen);

                    if (newfd == -1) {
                        perror("accept");
                    } else {
                        FD_SET(newfd, &master); // add to master set
                        if (newfd > fdmax) {    // keep track of the max
                            fdmax = newfd;
                        }
                        printf("selectserver: new connection from %s on socket %d\n", inet_ntop(remoteaddr.ss_family, get_in_addr((struct sockaddr*)&remoteaddr), remoteIP, INET6_ADDRSTRLEN), newfd);
                        client_n=1;//espera que le diga quien es
                  }//end else
                }
                else {
                    // handle data from a client
                    memset(buf, 0, 256);

                    if ((nbytes = recv(i, buf, sizeof buf, 0)) <= 0) {
                        // got error or connection closed by client

                        if (nbytes == 0) {
                            // connection closed
                            printf("selectserver: socket %d hung up\n", i);
                        } else {
                            perror("recv");
                        }
                        close(i); // bye!
                        FD_CLR(i, &master); // remove from master set
                    } else {
                        // we got some data from a client
                        
                        if (client_n==1){
                        	ident=atoi(buf);
                        	switch(ident){
                        		case 1:
                        			A = i;
                        			printf("A");
                        			client_n=0;
                        			break;
                        		case 2:
                        			B = i;
                        			printf("B");
                        			client_n=0;
                        			break;
                        		case 3:
                        			C = i;
                        			printf("C");
                        			client_n=0;
                        			break;
								default:
									D = i;
                        			printf("D");
                        			client_n=0;
                        			break;
								
									//printf("conexion no valida");
									//client_n=0;
									//break;
                        	}
                        
                        }
                        
                        else{
                        
		                    for(j = 0; j <= fdmax; j++) {
		                        // send to everyone!
		                        	
		                        if (FD_ISSET(j, &master)) {
		                            // except the listener and ourselves
		                            if (j != listener && j != i) {
										if (buf[0]=='A') {rx=A; }
										else if (buf[0]=='B') {rx=B; }
										else if (buf[0]=='C') {rx=C; }
										else if (buf[0]=='D') {rx=D; }
										else if (buf[0]=='S') { 
										//memmove(&buf[0], &buf[1], strlen(buf)-0);
										memset(line, 0, sizeof(line));
										printf("Contenido del buffer en server es: %s \n", buf);
										strcpy(line, buf);
										memmove(&line[0], &line[1], strlen(line)-0);
										printf("Contenido del buffer en server es: %s \n", line);
										
										int val = atoi(line);
										printf("\t\tValor es: %d", val);
										memset(line, 0, sizeof(line));
										printf("\n");
									
										 }
										if (j == rx){
											memmove(&buf[0], &buf[1], strlen(buf)-0);
											if (i==A){
												tx=1;
											}
											else if (i==B){
												tx=2;
											}
											else if (i==C){
												tx=3;
											}
											else if (i==D){
												tx=4;
											}
											sprintf(em,"%d", tx);
											printf("Contenido del buffer es: %s \nTX: %d identificador: %d\n", buf, i ,tx);	
											printf("RX: %d \n", rx); 
											strcat(em, buf);//guarda valor buffer concatenado con emisor y receptor
											if (send(rx, em, strlen(em), 0) == -1) {
												perror("send");}
		                               	}
		                            }
		                        }
		                    }
		                }
                    }
                } // END handle data from client
            } // END got new incoming connection
        } // END looping through file descriptors
    } // END for(;;)--and you thought it would never end!

    return 0;
}
