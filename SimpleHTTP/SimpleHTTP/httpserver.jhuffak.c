/*
	Jordan Huffaker - 0026480616 - jhuffak@purdue.edu
	ECE 463 Lab 1
*/

#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MAXLINE 300
#define LISTENQ 100

int open_listenfd(int port);
void echo(int connfd);
void get_page(int connfd);
int open_clientfd(char *hostname, int port);

// CREDIT: Adapted from Sanjay Rao - Lecture Socket Programming slide 30
int main(int argc, char **argv)
{
	//int test = open(argv[1], S_IWRITE | S_IREAD);

	//get_page(test);
	//close(test);

	int listenfd, connfd, port, clientlen;
	struct sockaddr_in clientaddr;
	struct hostent *hp;
	char *haddrp;
	port = atoi(argv[1]); // the server listens on a port passed on the command line
	listenfd = open_listenfd(port);
	while (1)
	{
		clientlen = sizeof(clientaddr);
		connfd = accept(listenfd, (struct sockaddr *) &clientaddr, &clientlen);
		hp = gethostbyaddr((const char *)&clientaddr.sin_addr.s_addr,
		sizeof(clientaddr.sin_addr.s_addr), AF_INET);
		haddrp = inet_ntoa(clientaddr.sin_addr);
		//printf("Fd %d connected to %s (%s:%s)\n", connfd, hp->h_name, haddrp, ntohs(clientaddr.sin_port));
		get_page(connfd);
		close(connfd);
	}
}

void get_page(int connfd)
{
	size_t n;
	char buf[MAXLINE];
	char* tok;
	const char e404[30] = "HTTP/1.0 404 Not Found\r\n\r\n\0";
	const char e403[30] = "HTTP/1.0 403 Forbidden\r\n\r\n\0";
	const char OK[30] = "HTTP/1.0 200 OK\r\n\r\n\0";
	int startI = 4;
	int test;
	const char stuff[100] = "./test_garbage.txt";

	test = open(stuff, O_WRONLY);

	n = read(test, buf, MAXLINE);
	buf[n - 1] = '\0';

	if (buf[4] == '/')
	{
		startI = 5;
	}
	if (strtok((char*)&buf[startI], " ") == NULL)
	{
		write(test, e404, 26);
		return;
	}

	if (access((char*)&buf[startI], F_OK) == -1)
	{
		write(test, e404, 26);
		return;
	}

	if (access((char*)&buf[startI], R_OK) == -1)
	{
		write(test, e403, 26);
		return;
	}

	int foundFile = open(&buf[startI], O_RDONLY, S_IREAD);
	
	write(test, OK, 19);
	while((n = read(foundFile, buf, MAXLINE)) != 0)
	{
		write(test, buf, n);
	}
	close(test);
	close(foundFile);
}

// CREDIT: Sanjay Rao - Lecture Socket Programming slide 35
void echo(int connfd)
{
	size_t n;
	char buf[MAXLINE];


	while ((n = read(connfd, buf, MAXLINE)) != 0)
	{
		printf("server received %d bytes\n", n);
		write(connfd, buf, n);
	}
}

// CREDIT: Sanjay Rao - Lecture Socket Programming slide 24-25
int open_listenfd(int port)
{
	int listenfd, optval=1;
	struct sockaddr_in serveraddr;

	/* Create a socket descriptor */
	if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		return -1;

	/* Eliminates "Address already in use" error from bind. */
	if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR,
		(const void *)&optval , sizeof(int)) < 0)
		return -1;
	/* Listenfd will be an endpoint for all requests to port
	on any IP address for this host */
	bzero((char *) &serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons((unsigned short)port);
	if (bind(listenfd, (struct sockaddr *) &serveraddr, sizeof(serveraddr)) < 0)
		return -1;
	/* Make it a listening socket ready to accept
	connection requests */
	if (listen(listenfd, LISTENQ) < 0)
		return -1;
	return listenfd;
}


/*
// CREDIT: Sanjay Rao - Lecture Socket Programming slide 14
int main(int argc, char **argv)
{
	int clientfd, port;
	char *host, buf[MAXLINE], temp[20];

	host = argv[1];
	port = atoi(argv[2]);
	clientfd = open_clientfd(host, port);
	
	strcpy(buf, "GET ");
	strcat(buf, argv[3]);
	strcpy(temp, " HTTP/1.0\r\n\r\n\0");
	strcat(buf, temp);
	
	write(clientfd, buf, strlen(buf));
	while (read(clientfd, buf, sizeof(char)) != 0) {
		fputc(*buf, stdout);
	}
	close(clientfd);
	exit(0);
}

*/

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
	if (connect(clientfd, (const struct sockaddr *) &serveraddr, sizeof(serveraddr)) < 0)
		return -1;
	return clientfd;
}
