/*  Author: Michael Oritz
 *  Email: mtortiz.mail@gmail.com
 *  Date: 5/29/2010
 *
 *  udp_server.c
 *  A simple udp server designed for a single client, but can
 *  be easily modified to handle multiple clients.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int sd; //UDP socket descriptor
struct sockaddr_in serveraddr;
struct sockaddr_in clientaddr;
unsigned int clientaddrlen = sizeof(clientaddr);

/* upd_server_init
 *
 * port - port number to listen on
 *
 * Initializes a socket and binds server information to it.
 * Returns 0 on success, -1 on failure.
 */
int udp_server_init(int port) {
    // Open a UDP socket
    if ((sd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        fprintf(stderr, "socket() failed\n");
        return -1;
    }

    // Set the server port and server ip addresses
    memset(&serveraddr, 0, sizeof(serveraddr));
    serveraddr.sin_family      = AF_INET;
    serveraddr.sin_port        = htons(port);
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);

    // Bind socket to server port and server ip addresses
    if ((bind(sd, (struct sockaddr *)&serveraddr, sizeof(serveraddr))) < 0) {
       fprintf(stderr, "bind() failed\n");
       close(sd);
       return -1;
    }

    return 0;
}

/* udp_receive
 * 
 * buf  - to return received data
 * size - size of buf 
 *
 * Receives data from udp client and stores it in buf.
 * NOTE: this must be called before the first call to udp_send to
 * allow client information to be gaterhed.
 * Returns 0 on success, -1 on failure.
 */
int udp_receive (void * buf, int size) {
    // Listen for incoming packets; gather client information
    if (recvfrom(sd, buf, size, 0, 
        (struct sockaddr *)&clientaddr, &clientaddrlen) < 0) {
        fprintf(stderr, "recvfrom() failed\n");
        close(sd);
        return -1;
    }

    fprintf(stderr, "server received the following:\n<%s>\n", (char *)buf);
    fprintf(stderr, "from port %d and address %s\n", ntohs(clientaddr.sin_port),
           inet_ntoa(clientaddr.sin_addr));

   return 0;
}

/* udp_send
 * 
 * buf  - data to send
 * size - number of bytes to send
 *
 * Sends data to udp client.
 * NOTE: udp_receive must be called before the first call to function to
 * allow client information to be gathered.
 * Returns 0 on success, -1 on failure.
 */
int udp_send (void * buf, int size) {
    if (sendto(sd, buf, size, 0,
        (struct sockaddr *)&clientaddr, sizeof(clientaddr)) < 0) {
        fprintf(stderr, "sendto() failed\n");
        close(sd);
        return -1;
    }
    return 0;
}
