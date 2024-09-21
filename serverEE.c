/*
** listener.c -- a datagram sockets "server" demo
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <signal.h>
#define MYPORT "23105" // the port users will be connecting to

#define MAXBUFLEN 100

int sockfd;
// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET)
	{
		return &(((struct sockaddr_in *)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}

void INThandler(int sig)
{

	signal(sig, SIG_IGN);
	close(sockfd);
	exit(0);
}

char *get_data(char *course, char *cat)
{
	static char str[100];
	static char strc[100];
	char *token;
	FILE *textfile;
	char line[1000];
	char *array[100];
	int i = 0;
	textfile = fopen("ee.txt", "r");

	while (fgets(line, sizeof(line), textfile))
	{
		str[0] = 0;
		strc[0] = 0;
		strcpy(str, line);
		int length = strlen(str);
		// printf("length is %d \n",length);
		for (i = 0; i < length; i++)
		{
			if (str[i] == 10)
			{
				str[i] = 0;
			}
			if (str[i] == 13)
			{
				str[i] = 0;
			}
		}
		i = 0;
		strcpy(strc, str);
		strcat(strc, "\n");
		strc[5] = ':';
		token = strtok(str, ",");
		while (token)
		{
			array[i++] = token;
			token = strtok(NULL, ",");
		}

		if ((strcmp(array[0], course) == 0))
		{
			if (strcmp(cat, "Credit") == 0)
			{
				printf("\nThe course information has been found: The %s of %s is %s.", cat, course, array[1]);
				fclose(textfile);
				return array[1];
			}
			if (strcmp(cat, "Professor") == 0)
			{
				printf("\nThe course information has been found: The %s of %s is %s.", cat, course, array[2]);
				fclose(textfile);
				return array[2];
			}
			if (strcmp(cat, "Days") == 0)
			{
				printf("\nThe course information has been found: The %s of %s is %s.", cat, course, array[3]);
				fclose(textfile);
				return array[3];
			}
			if (strcmp(cat, "CourseName") == 0)
			{
				printf("\nThe course information has been found: The %s of %s is %s.", cat, course, array[4]);
				fclose(textfile);
				return array[4];
			}
			if (strcmp(cat, "all") == 0)
			{
				printf("\nThe course information has been found for %s", course);
				fclose(textfile);
				return strc;
			}
		}
	}
	printf("\nDidn’t find the course: %s.", course);
	fclose(textfile);
	return "FAIL_NO_INFO";
}

int main(void)
{

	struct addrinfo hints, *servinfo, *p;
	int rv;
	int numbytes;
	struct sockaddr_storage their_addr;
	char buf[MAXBUFLEN];
	socklen_t addr_len;
	// char s[INET6_ADDRSTRLEN];
	char *z;
	char *array[3];
	char *ret_arr;
	int i = 0;
	// int len;
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_INET; // set to AF_INET to use IPv4
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_flags = AI_PASSIVE; // use my IP

	if ((rv = getaddrinfo(NULL, MYPORT, &hints, &servinfo)) != 0)
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
			perror("listener: socket");
			continue;
		}

		if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1)
		{
			close(sockfd);
			perror("listener: bind");
			continue;
		}

		break;
	}

	if (p == NULL)
	{
		fprintf(stderr, "listener: failed to bind socket\n");
		return 2;
	}

	freeaddrinfo(servinfo);

	printf("The ServerEE is up and running using UDP on port %s\n", MYPORT);
	signal(SIGINT, INThandler);
	while (1)
	{
		addr_len = sizeof their_addr;
		if ((numbytes = recvfrom(sockfd, buf, MAXBUFLEN - 1, 0,
								 (struct sockaddr *)&their_addr, &addr_len)) == -1)
		{
			perror("recvfrom");
			exit(1);
		}

		buf[numbytes] = '\0';
		// printf("%s", buf);
		z = strtok(buf, ",");
		i = 0;
		while (z != NULL)
		{
			array[i++] = z;
			z = strtok(NULL, ",");
		}
		printf("The ServerEE received a request from the Main Server about %s of %s", array[1], array[0]);
		ret_arr = get_data(array[0], array[1]);
		if ((numbytes = sendto(sockfd, ret_arr, strlen(ret_arr), 0,
							   (struct sockaddr *)&their_addr, addr_len)) == -1)
		{
			perror("talker: sendto");
			exit(1);
		}
		else
			printf("\nThe serverEE finished sending the response to the Main Server.\n");
	}
	close(sockfd);

	return 0;
}