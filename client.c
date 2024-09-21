/*
** client.c -- a stream socket client demo
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include <arpa/inet.h>

#define PORT "25105" // the port client will be connecting to

#define MAXDATASIZE 1024 // max number of bytes we can get at once

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET)
    {
        return &(((struct sockaddr_in *)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}

char *send_tcp(char *data)
{
    int sockfd, numbytes;
    char buf[MAXDATASIZE];
    struct addrinfo hints, *servinfo, *p;
    struct sockaddr_in addr_1;
    socklen_t len = sizeof(addr_1);
    int rv;
    // char s[INET6_ADDRSTRLEN];
    char x[100];
    strcpy(x, data);
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    int i = 0;
    char *z;
    char *array[3];
    static char retarr[10];
    if ((rv = getaddrinfo("127.0.0.1", PORT, &hints, &servinfo)) != 0)
    {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return "getaddr error";
    }

    // loop through all the results and connect to the first we can
    for (p = servinfo; p != NULL; p = p->ai_next)
    {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                             p->ai_protocol)) == -1)
        {
            perror("client: socket");
            continue;
        }

        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1)
        {
            perror("client: connect");
            close(sockfd);
            continue;
        }

        break;
    }

    if (p == NULL)
    {
        fprintf(stderr, "client: failed to connect\n");
        return "failed to connect";
    }

    if ((getsockname(sockfd, (struct sockaddr *)&addr_1, &len)) == -1)
        perror("getsockname");

    freeaddrinfo(servinfo); // all done with this structure

    if (send(sockfd, data, strlen(data), 0) == -1)
        perror("send");
    z = strtok(x, ",");
    while (z != NULL)
    {
        array[i++] = z;
        z = strtok(NULL, ",");
    }
    if (strcmp(array[0], "1") == 0)
        printf("\n%s sent an authentication request to the main server.", array[1]);

    if ((strcmp(array[0], "2") == 0) && (strcmp(array[2], "all") == 0))
        printf("\n%s sent a request with multiple CourseCode to the main server.", array[3]);

    else if (strcmp(array[0], "2") == 0)
        printf("\n%s sent a request to the main server.", array[3]);

    if ((numbytes = recv(sockfd, buf, MAXDATASIZE - 1, 0)) == -1)
    {
        perror("recv");
        exit(1);
    }

    buf[numbytes] = '\0';
    strcpy(retarr, buf);
    // printf("client: received '%s'\n", buf);
    if (strcmp(buf, "PASS") == 0)
    {
        printf("\n\n%s received the result of authentication using TCP over port %d. Authentication is successful", array[1], ntohs(addr_1.sin_port));
        return "PASS";
    }
    if (strcmp(buf, "FAIL_PASS_NO_MATCH") == 0)
    {
        printf("\n\n%s received the result of authentication using TCP over port %d. Authentication failed: Password does not match", array[1], ntohs(addr_1.sin_port));
        return "FAIL";
    }
    if (strcmp(buf, "FAIL_NO_USER") == 0)
    {
        printf("\n\n%s received the result of authentication using TCP over port %d. Authentication failed: Username Does not exist", array[1], ntohs(addr_1.sin_port));
        return "FAIL";
    }
    if (strcmp(array[0], "2") == 0)
    {
        printf("\n\nthe client received the response from Main Server using TCP over port %d.\n", ntohs(addr_1.sin_port));
        return retarr;
    }

    close(sockfd);

    return "exit";
}

int main(void)
{
    char authstr[100];
    char *querrystr;
    querrystr = malloc(100 * sizeof(char));
    int auth = 0;
    int attempt = 0;
    char username[20];
    char password[20];
    char *course;
    course = malloc(62 * sizeof(char));
    char category[20];
    int space = 0;
    while ((!auth) && (attempt < 3))
    {
        printf("The client is up and running.\n");
        printf("\nPlease enter the username:");
        scanf("%[^\n]%*c", username);
        printf("\nPlease enter the password:");
        scanf("%[^\n]%*c", password);
        memset(authstr, 0, 100);
        strcat(authstr, "1,");
        strcat(authstr, username);
        strcat(authstr, ",");
        strcat(authstr, password);
        // printf("%s", authstr);
        char *retarr = send_tcp(authstr);
        if (strcmp(retarr, "PASS") == 0)
            auth = 1;
        attempt = attempt + 1;
        if (strcmp(retarr, "FAIL") == 0)
            printf("\n\nAttempts remaining:%d", 3 - attempt);
    }
    if (auth == 0)
    {
        printf("\n\nAuthentication Failed for 3 attempts. Client will shut down.");
        exit(0);
    }
    while (auth)
    {
        space = 0;
        printf("\n\nPlease enter the course code to query:");
        scanf("%[^\n]%*c", course);
        for (unsigned int i = 0; i < strlen(course); i++)
        {
            if (course[i] == ' ')
            {
                space = space + 1;
            }
        }
        if (space == 0)
        {
            printf("\nPlease enter the category (Credit / Professor / Days / CourseName):");
            scanf("%[^\n]%*c", category);
        }
        else
        {
            memset(category, 0, 100);
            strcpy(category, "all");
        }
        memset(querrystr, 0, 100);
        strcat(querrystr, "2,");
        strcat(querrystr, course);
        strcat(querrystr, ",");
        strcat(querrystr, category);
        strcat(querrystr, ",");
        strcat(querrystr, username);
        // printf("%s", querrystr);
        char *retqarr = send_tcp(querrystr);
        if (space == 0)
        {
            if (strcmp(retqarr, "FAIL_NO_INFO") == 0)
                printf("\nDidn’t find the course: %s", course);

            else
                printf("\nThe %s of %s is %s", category, course, retqarr);
        }
        else
        {
            printf("\nCourseCode: Credits, Professor, Days, Course Name");
            printf("\n%s", retqarr);
        }
        printf("\n\n-----Start a new request-----");
    }

    return 0;
}
