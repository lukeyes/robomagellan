/**************************************************************************/
/* This sample program provides a code for a connectionless client.       */
/**************************************************************************/

/**************************************************************************/
/* Header files needed for this sample program                            */
/**************************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

/**************************************************************************/
/* Constants used by this program                                         */
/**************************************************************************/
#define SERVER_PORT     3555
#define BUF_LENGTH    15000
#define SERVER_NAME     "10.0.50.1"

int main(int argc, char *argv[])
{
    int    sd, rc, i, row=0;
    char   server[32];
    char   buf[BUF_LENGTH];
    struct sockaddr_in serveraddr;
    unsigned int    serveraddrlen = sizeof(serveraddr);

    do
    {
        sd = socket(AF_INET, SOCK_DGRAM, 0);
        if (sd < 0)
        {
            fprintf(stderr, "socket() failed\n");
            break;
        }

        strcpy(server, SERVER_NAME);

        memset(&serveraddr, 0, sizeof(serveraddr));
        serveraddr.sin_family      = AF_INET;
        serveraddr.sin_port        = htons(SERVER_PORT);
        serveraddr.sin_addr.s_addr = inet_addr(server);

        memset(buf, 0, sizeof(buf));
        strcpy(buf, "SEND RAW IMAGE");

        rc = sendto(sd, buf, strlen(buf), 0,
                  (struct sockaddr *)&serveraddr,
                  sizeof(serveraddr));
        if (rc < 0)
        {
            fprintf(stderr, "sendto() failed\n");
            break;
        }

        while (1) {
            rc = recvfrom(sd, buf, sizeof(buf), 0,
                        (struct sockaddr *)&serveraddr,
                        & serveraddrlen);
            if (rc < 0)
            {
                fprintf(stderr, "recvfrom() failed\n");
                break;
            }

            for(i=0; i<rc; ++i) {
                fprintf(stderr, "%x ", (unsigned char)buf[i]);
            }

            fprintf(stderr, "\n[row %d], %d bytes received", ++row, rc);
            fprintf(stderr, "from port %d, from address %s\n\n",
                ntohs(serveraddr.sin_port),
                inet_ntoa(serveraddr.sin_addr));
        }
    } while (0);

    if (sd != -1)
        close(sd);

    return 0;
}
