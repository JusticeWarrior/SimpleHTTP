#include <stdlib.h>
#include <stdio.h>
#include <usistd.h>

// CREDIT: Sanjay Rao - Lecture Socket Programming slide 14
int main(int argc, char **argv)
{
	int clientfd, port;
	char *host, buf[MAXLINE];


	host = argv[1];
	port = atoi(argv[2]);
	clientfd = open_clientfd(host, port);
	while (fgets(buf, MAXLINE, stdin) != NULL) {
		write(clientfd, buf, strlen(buf));
		read(clientfd, buf, MAXLINE,);
		fputs(buf, stdout);
	}
	close(clientfd);
	exit(0);
}

// CREDIT: Sanjay Rao - Lecture Socket Programming slide 15
int open_clientfd(char *hostname, int port)
{
	int clientfd;
	struct hostent *hp;
	struct sockaddr_in serveraddr;
	if ((clientfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		return -1; /* check errno for cause of error */
	/* Fill in the server's IP address and port */
	if ((hp = gethostbyname(hostname)) == NULL)
		return -2; /* check h_errno for cause of error */
	bzero((char *) &serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	bcopy((char *)hp->h_addr,
		(char *)&serveraddr.sin_addr.s_addr, hp->h_length);
	serveraddr.sin_port = htons(port);
	/* Establish a connection with the server */
	if (connect(clientfd, (SA *) &serveraddr, sizeof(serveraddr)) < 0)
		return -1;
	return clientfd;
}
