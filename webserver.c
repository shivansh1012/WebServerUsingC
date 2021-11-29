#include <stdio.h>

#include <stdlib.h>

#include <unistd.h>

#include <sys/types.h>

#include <sys/socket.h>

#include <netinet/in.h>

#include <netdb.h>

#include <arpa/inet.h>

#include <string.h>

#include <sys/stat.h>

#include <fcntl.h>

int main(int argc, char *argv[])
{

    struct sockaddr_in server_addr, client_addr;

    socklen_t sin_len = sizeof(client_addr);

    int fd_server, fd_client;

    char buf[2048];

    int fdimg;

    int on = 1;

    char http_header[13850] = "HTTP/1.1 200 OK\r\n";

    char http_header2[13850] = "HTTP/1.1 200 OK\r\n";

    char content_header[13850] = "Content-Type: text/html; charset=UTF-8\r\n\r\n";

    char lineReader1[10000];

    char lineReader2[10000];

    char homePageHTML[13800];

    FILE *homePageFileLink;

    homePageFileLink = fopen("homepage.html", "rb");

    strcat(homePageHTML, http_header);

    strcat(homePageHTML, content_header);

    while (fgets(lineReader1, 1000, homePageFileLink))
    {

        strcat(homePageHTML, lineReader1);
    }

    char mainPageHTML[8100];

    FILE *mainPageFileLink;

    mainPageFileLink = fopen("mainpage.html", "rb");

    strcat(mainPageHTML, http_header2);

    strcat(mainPageHTML, content_header);

    //printf("%s\n",http_header);

    //printf("%s\n",content_header);

    while (fgets(lineReader2, 1000, mainPageFileLink))
    {

        strcat(mainPageHTML, lineReader2);
    }

    //printf("%s\n",mainPageHTML);

    fd_server = socket(AF_INET, SOCK_STREAM, 0);

    if (fd_server < 0)
    {

        perror("socket");

        exit(1);
    }

    setsockopt(fd_server, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(int));

    server_addr.sin_family = AF_INET;

    server_addr.sin_addr.s_addr = INADDR_ANY;

    server_addr.sin_port = htons(8080);

    if (bind(fd_server, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
    {

        perror("bind");

        close(fd_server);

        exit(1);
    }

    if (listen(fd_server, 10) == 1)
    {

        perror("listen");

        close(fd_server);

        exit(1);
    }

    while (1)
    {

        fd_client = accept(fd_server, (struct sockaddr *)&client_addr, &sin_len);

        if (fd_client == -1)
        {

            perror("Connection failed.... \n");

            continue;
        }

        printf("Got client connection...... \n");

        if (!fork())
        {

            /* child process */

            close(fd_server);

            memset(buf, 0, 2048);

            read(fd_client, buf, 2047);

            printf("%s\n", buf);

            if (!strncmp(buf, "GET /home", 9))
            {

                write(fd_client, homePageHTML, sizeof(homePageHTML) - 1);

                printf("------------------------------------------------------Succesfully Responded Homepage\n");
            }

            else
            {

                write(fd_client, mainPageHTML, sizeof(mainPageHTML) - 1);

                printf("------------------------------------------------------Succesfully Responded Mainpage\n");
            }

            close(fd_client);

            printf("closing...\n");

            exit(0);

            /* parent process */
        }

        close(fd_client);
    }

    return 0;
}