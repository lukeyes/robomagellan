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
#define BUFFER_LENGTH    1024
#define SERVER_NAME     "10.0.50.1"

/* Pass in 1 parameter which is either the */
/* address or host name of the server, or  */
/* set the server name in the #define      */
/* SERVER_NAME                             */
int main(int argc, char *argv[])
{
    /***********************************************************************/
    /* Variable and structure definitions.                                 */
    /***********************************************************************/
    int    sd, rc;
    char   server[32];
    char   buffer[BUFFER_LENGTH];
    struct sockaddr_in serveraddr;
    unsigned int    serveraddrlen = sizeof(serveraddr);

    do
    {
        /********************************************************************/
        /* The socket() function returns a socket descriptor representing   */
        /* an endpoint.  The statement also identifies that the INET        */
        /* (Internet Protocol) address family with the UDP transport        */
        /* (SOCK_STREAM) will be used for this socket.                      */
        /********************************************************************/
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

        /********************************************************************/
        /* Initialize the data block that is going to be sent to the server */
        /********************************************************************/
        memset(buffer, 0, sizeof(buffer));
        strcpy(buffer, "KATHERINE VILCHEZ IS AMAZINGLY BEAUTIFUL, SMART, AND PRECIOUS TO HER BOYFRIEND MICHAEL ORTIZ");

        /********************************************************************/
        /* Use the sendto() function to send the data to the server.        */
        /********************************************************************/
        rc = sendto(sd, buffer, sizeof(buffer), 0,
                  (struct sockaddr *)&serveraddr,
                  sizeof(serveraddr));
        if (rc < 0)
        {
            fprintf(stderr, "sendto() failed\n");
            break;
        }

        /********************************************************************/
        /* Use the recvfrom() function to receive the data back from the    */
        /* server.                                                          */
        /********************************************************************/
        rc = recvfrom(sd, buffer, sizeof(buffer), 0,
                    (struct sockaddr *)&serveraddr,
                    & serveraddrlen);
        if (rc < 0)
        {
            fprintf(stderr, "recvfrom() failed\n");
            break;
        }

        printf("client received the following:\n<%s>\n", buffer);
        printf("from port %d, from address %s\n",
             ntohs(serveraddr.sin_port),
             inet_ntoa(serveraddr.sin_addr));

        /********************************************************************/
        /* Program complete                                                 */
        /********************************************************************/

    } while (0);

    /***********************************************************************/
    /* Close down any open socket descriptors                              */
    /***********************************************************************/
    if (sd != -1)
        close(sd);

    return 0;
}
