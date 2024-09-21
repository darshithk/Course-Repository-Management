/*
** server.c -- a stream socket server demo
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <ctype.h>
#define PORT "25105" // the port users will be connecting to
#define BACKLOG 10	 // how many pending connections queue will hold
#define MAXDATASIZE 100
#define MAXLINE 1024
void sigchld_handler(int s)
{
	(void)s; // quiet unused variable warning

	// waitpid() might overwrite errno, so we save and restore it:
	int saved_errno = errno;

	while (waitpid(-1, NULL, WNOHANG) > 0)
		;

	errno = saved_errno;
}

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET)
	{
		return &(((struct sockaddr_in *)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}

char *udptalker(int port_udp, char *msg)
{
	int sockfd;
	static char buffer[MAXLINE];
	struct sockaddr_in servaddr, my_addr1;
	socklen_t len = sizeof(servaddr);
	// Creating socket file descriptor
	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		perror("socket creation failed");
		exit(EXIT_FAILURE);
	}

	memset(&servaddr, 0, sizeof(servaddr));

	// Filling server information
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(port_udp);
	servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");

	int n;
	int udp_client_port = 24105;
	my_addr1.sin_family = AF_INET;
	my_addr1.sin_addr.s_addr = INADDR_ANY;
	my_addr1.sin_port = htons(udp_client_port);
	my_addr1.sin_addr.s_addr = inet_addr("127.0.0.1");

	if (bind(sockfd, (struct sockaddr *)&my_addr1, sizeof(struct sockaddr_in)) != 0)
		// printf("Binded Correctly\n");
		printf("Unable to bind\n");

	sendto(sockfd, msg, strlen(msg),
		   MSG_CONFIRM, (const struct sockaddr *)&servaddr,
		   sizeof(servaddr));
	// printf("Hello message sent.\n");

	n = recvfrom(sockfd, (char *)buffer, MAXLINE,
				 MSG_WAITALL, (struct sockaddr *)&servaddr,
				 &len);
	buffer[n] = '\0';
	// printf("Server : %s\n", buffer);
	if (strcmp(buffer, "PASS") == 0 || strcmp(buffer, "FAIL_PASS_NO_MATCH") == 0 || strcmp(buffer, "FAIL_NO_USER") == 0)
		printf("\nThe main server received the result of the authentication request from ServerC using UDP over port %d.", udp_client_port);
	else if (port_udp == 23105)
		printf("\nThe main server received the response from serverEE using UDP over port %d.", udp_client_port);
	else if (port_udp == 22105)
		printf("\nThe main server received the response from serverEE using UDP over port %d.", udp_client_port);
	close(sockfd);
	return buffer;
}

int main(void)
{
	int sockfd, new_fd, numbytes; // listen on sock_fd, new connection on new_fd
	struct addrinfo hints, *servinfo, *p;
	struct sockaddr_storage their_addr; // connector's address information
	socklen_t sin_size;
	char buf[MAXDATASIZE];
	struct sigaction sa;
	int yes = 1;
	// char s[INET_ADDRSTRLEN];
	int rv;
	int i = 0;
	char *z;
	char *array[3];
	char arr[100];
	char arr1[100];
	char arr2[100];
	char checkstr[20];
	char retstrq[1000];
	char *x;
	// int x;
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE; // use my IP

	if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0)
	{
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}

	// loop through all the results and bind to the first we can
	for (p = servinfo; p != NULL; p = p->ai_next)
	{
		if ((sockfd = socket(p->ai_family, p->ai_socktype,
							 p->ai_protocol)) == -1)
		{
			perror("server: socket");
			continue;
		}

		if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
					   sizeof(int)) == -1)
		{
			perror("setsockopt");
			exit(1);
		}

		if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1)
		{
			close(sockfd);
			perror("server: bind");
			continue;
		}

		break;
	}

	freeaddrinfo(servinfo); // all done with this structure

	if (p == NULL)
	{
		fprintf(stderr, "server: failed to bind\n");
		exit(1);
	}

	if (listen(sockfd, BACKLOG) == -1)
	{
		perror("listen");
		exit(1);
	}

	sa.sa_handler = sigchld_handler; // reap all dead processes
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	if (sigaction(SIGCHLD, &sa, NULL) == -1)
	{
		perror("sigaction");
		exit(1);
	}

	printf("The main server is up and running.\n");

	while (1)
	{ // main accept() loop
		sin_size = sizeof their_addr;
		new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
		if (new_fd == -1)
		{
			perror("accept");
			continue;
		}

		if (!fork())
		{				   // this is the child process
			close(sockfd); // child doesn't need the listener
			if ((numbytes = recv(new_fd, buf, MAXDATASIZE - 1, 0)) == -1)
			{
				perror("recv");
				exit(1);
			}
			i = 0;
			buf[numbytes] = '\0';
			z = strtok(buf, ",");
			while (z != NULL)
			{
				array[i++] = z;
				z = strtok(NULL, ",");
			}

			if (strcmp(array[0], "1") == 0)
			{
				arr[0] = 0;
				printf("The main server received the authentication for %s using TCP over port %s.", array[1], PORT);
				for (i = 0; (unsigned)i < strlen(array[1]); i++)
				{
					if (islower(array[1][i]))
					{
						if ((array[1][i] + 4) > 122)
						{
							array[1][i] = (array[1][i] + 4) - 122 + 96;
						}
						else
							array[1][i] = array[1][i] + 4;
					}
					else if (isupper(array[1][i]))
					{
						if ((array[1][i] + 4) > 90)
						{
							array[1][i] = (array[1][i] + 4) - 90 + 64;
						}
						else
							array[1][i] = array[1][i] + 4;
					}

					else if (isdigit(array[1][i]))
					{

						if ((array[1][i] + 4) > 57)
						{
							array[1][i] = (array[1][i] + 4) - 57 + 47;
						}
						else
							array[1][i] = array[1][i] + 4;
					}
				}
				for (i = 0; (unsigned)i < strlen(array[2]); i++)
				{
					if (islower(array[2][i]))
					{
						if ((array[2][i] + 4) > 122)
						{
							array[2][i] = (array[2][i] + 4) - 122 + 96;
						}
						else
							array[2][i] = array[2][i] + 4;
					}
					else if (isupper(array[2][i]))
					{
						if ((array[2][i] + 4) > 90)
						{
							array[2][i] = (array[2][i] + 4) - 90 + 64;
						}
						else
							array[2][i] = array[2][i] + 4;
					}

					else if (isdigit(array[2][i]))
					{

						if ((array[2][i] + 4) > 57)
						{
							array[2][i] = (array[2][i] + 4) - 57 + 47;
						}
						else
							array[2][i] = array[2][i] + 4;
					}
				}
				strcat(arr, array[1]);
				strcat(arr, ",");
				strcat(arr, array[2]);
				// printf("\n %s",arr);
				printf("\nThe main server sent an authentication request to serverC.");
				char *ret_arr = udptalker(21105, arr);
				// printf("client: received '%s'\n", buf);
				if (send(new_fd, ret_arr, strlen(ret_arr), 0) == -1)
					perror("send");
				printf("\nThe main server sent the authentication result to the client.\n");
				close(new_fd);
				exit(0);
			}
			else if (strcmp(array[0], "2") == 0)
			{
				arr1[0] = 0;
				arr2[0] = 0;
				retstrq[0] = 0;
				printf("The main server received from %s to query course/courses %s about %s", array[3], array[1], array[2]);
				x = strtok(array[1], " ");
				while (x != NULL)
				{
					strncpy(checkstr, x, 2);
					checkstr[2] = '\0';
					// printf("%s check str",checkstr);
					if (strcmp(checkstr, "EE") == 0)
					{
						arr1[0] = 0;
						printf("\nThe main server sent a request to serverEE");
						strcat(arr1, x);
						strcat(arr1, ",");
						strcat(arr1, array[2]);
						// printf("%s",arr1);
						char *ret_arr = udptalker(23105, arr1);
						strcat(retstrq, ret_arr);
						// printf("%s", ret_arr);
					}
					else if (strcmp(checkstr, "CS") == 0)
					{
						arr2[0] = 0;
						printf("\nThe main server sent a request to serverCS");
						strcat(arr2, x);
						strcat(arr2, ",");
						strcat(arr2, array[2]);
						// printf("%s",arr1);
						char *ret_arr = udptalker(22105, arr2);
						strcat(retstrq, ret_arr);
						// printf("%s", ret_arr);
					}
					else
						// printf("\n not proper send error");
						strcat(retstrq, "FAIL_NO_INFO");
					x = strtok(NULL, " ");
				}
				// printf("%s",retstrq);
				if (send(new_fd, retstrq, strlen(retstrq), 0) == -1)
					perror("send");
				printf("\nThe main server sent the query information to the client.\n");
				close(new_fd);
				exit(0);
			}
		}
	}
	close(new_fd); // parent doesn't need this

	return 0;
}