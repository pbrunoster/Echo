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
#include <unistd.h>


#define TESTERR( X, M) \
	if X { \
		fprintf(stderr,"Errore %s\n",M);\
	}


typedef int (* proc)(int i, void *data);




int echo(int i,void *data) {
	int len;
	char tmp[1024];
	memset(tmp,0L,1024);
	len=recv(i,tmp,1024,MSG_DONTWAIT);
	fprintf(stderr,"%s\n",tmp);
	send(i,tmp,len,MSG_DONTWAIT);
  return len;
}



int socket_init(struct hostent *h, int port,proc proc_fun) {
	int sock=-1;
	int num;
	fd_set ms, ws;
	struct sockaddr_in saddr;
	struct linger lin ;
	lin.l_onoff=0;
	lin.l_linger=0;
	struct timeval t;
	t.tv_sec=0;
	t.tv_usec=100;
	memset((void *)&saddr,0,sizeof saddr);
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(port);
	saddr.sin_addr.s_addr= *(unsigned int *)(h->h_addr_list[0]);
	int channel=socket(AF_INET,SOCK_STREAM,0); 
	TESTERR((setsockopt(channel,SOL_SOCKET,SO_LINGER,(const void *) &lin,sizeof lin)!=0),"SOCK LINGER FALLITO ")
		TESTERR((bind(channel, (const struct sockaddr *) &saddr, sizeof saddr)!=0),"Errore in Bind")
		TESTERR((listen(channel,10)!=0),"Errore in Listen");
	FD_ZERO(&ms);
	FD_SET(channel,&ms);
	do {
		struct sockaddr_in saddr_cl;
		int len=sizeof saddr_cl;
		memcpy(&ws,&ms,sizeof ms);
		TESTERR(((num=select(FD_SETSIZE,&ws,NULL,NULL,NULL))<0),"Errore in select");
		for (int i=0;i<FD_SETSIZE;++i) {
			if (FD_ISSET(i,&ws)) {
				if (i==channel) { // Nuova socket da accettare
						TESTERR(((sock=accept(channel,(struct sockaddr *) &saddr_cl,&len))==-1),"Errore in accept")
					FD_SET(sock,&ms);
					fprintf(stderr,"indirizzo client  %s\n",inet_ntoa(saddr_cl.sin_addr));
					fprintf(stderr,"porta client  %d\n",ntohs(saddr_cl.sin_port));
				}
				else {
					if (proc_fun(i,NULL)<0) {
						close(i);
						FD_CLR(i,&ms);
					}
				}
			}
		}
	} while(1);
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
	socket_init(h,port,echo);
}
