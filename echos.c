#include <stdio.h> 
#include <stdlib.h>
#include <string.h>
#include <assert.h> 
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#define TESTERREQ( X, Y, M) \
	if (X==Y) {\
	fprintf(stderr,"Errore %s\n",M);\
	}

#define TESTERRNQ( X, Y, M) \
	if (X!=Y) {\
	fprintf(stderr,"Errore %s\n",M);\
	}

int socket_init(struct hostent *h, int port) {
struct sockaddr_in saddr;
memset((void *)&saddr,0,sizeof saddr);
saddr.sin_family = AF_INET;
saddr.sin_port = htons(port);
saddr.sin_addr.s_addr= *(unsigned int *)(h->h_addr_list[0]);
int channel=socket(AF_INET,SOCK_STREAM,0); 
TESTERRNQ(bind(channel, (const struct sockaddr *) &saddr, sizeof saddr),0,"Errore in Bind")
TESTERRNQ(listen(channel,10),0,"Errore in Listen");
do {
  struct sockaddr_in saddr_cl;
	int len=sizeof saddr_cl;
	int sock=-1;
  TESTERREQ(sock=(accept(channel,(struct sockaddr *) &saddr_cl,&len)),-1,"Errore in accept")
	fprintf(stderr,"porta client  %d",ntohs(saddr_cl.sin_port));
} while(1);
}

int main(int argc,char *argv[]){
assert(argc==3);
struct hostent *h;
int port=atoi(argv[2]);
TESTERREQ((h=gethostbyname(argv[1])),NULL,"Errore in gethostbyname");
printf("LEN %d\n",h->h_length);
for (int i=0;h->h_addr_list[i]!=NULL;i++) {
fprintf(stderr,"indirizzo %s\n", inet_ntoa( *((struct in_addr **) h->h_addr_list)[i]));
}
socket_init(h,port);
}
