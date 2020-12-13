#include <stdio.h> 
#include <stdlib.h>
#include <string.h>
#include <assert.h> 
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <arpa/inet.h>

#define TESTERR( X, M) \
	if X { \
		fprintf(stderr,"Errore %s\n",M);\
	}

typedef int (*proc)(int port, void *data);

int socket_connect(struct hostent *h, int port,proc proc_fun) {
	int num;
	char msg[1024];
	int len;
	fd_set ms, ws;
	struct sockaddr_in saddr;
	struct linger lin ;
	lin.l_onoff=0;
	lin.l_linger=0;
	memset((void *)&saddr,0,sizeof saddr);
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(port);
	saddr.sin_addr.s_addr= *(unsigned int *)(h->h_addr_list[0]);
	int channel=socket(AF_INET,SOCK_STREAM,0); 
	TESTERR((setsockopt(channel,SOL_SOCKET,SO_LINGER,(const void *) &lin,sizeof lin)!=0),"SOCK LINGER FALLITO ")
		TESTERR((connect(channel, (const struct sockaddr *) &saddr, sizeof saddr)==0),"Errore in Connect")
		FD_ZERO(&ms);
	FD_SET(channel,&ms);
	FD_SET(0,&ms);
	while(1) {
		memcpy(&ws,&ms,sizeof ms);
		TESTERR(((num=select(FD_SETSIZE,&ws,NULL,NULL,NULL))<0),"Errore in select")
			for (int i=0;i<FD_SETSIZE;i++) {
				if (i==channel) {
					len=recv(i,msg,1024,MSG_DONTWAIT);
					msg[len+1]='\0';
					fprintf(stderr,"messaggio ricevuto  %s\n",msg);
				} else {
					len=recv(i,msg,1024,MSG_DONTWAIT);
					send(channel,msg,len,MSG_DONTWAIT);
				}
			}
	}
}

int main(int argc,char *argv[]){
	assert(argc==3);
	struct hostent *h;
	int port=atoi(argv[2]);
	TESTERR(((h=gethostbyname(argv[1]))==NULL),"Errore in gethostbyname");
	printf("LEN %d\n",h->h_length);
	for (int i=0;h->h_addr_list[i]!=NULL;i++) {
		fprintf(stderr,"indirizzo %s\n", inet_ntoa( *((struct in_addr **) h->h_addr_list)[i]));
	}
	socket_connect(h,port,NULL);
}
