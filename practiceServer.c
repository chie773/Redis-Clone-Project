/*
 * Psuedo-Code:
 *
 * fd = socket()
 *
 * bind(fd,address)
 * listen(fd)
 *
 * while True:
 * 	conn_fd = accept(fd)
 * 	do_something_with(conn_fd)
 * 	close(conn_fd)
 * */

#include <iostream>;
#include <sys/socket.h>;
#include <netinet/in.h>;
#include <netinet/ip.h>; 


int fd = socket(AF_INET, SOCK_STREAM, 0); 

// AF_INET is for IPv4. USe AF_INET6 fo rIPv6 or dual-stack sockets.
// SOCK_SCTREAM is for TCP. USE SOCK_DGRAM for UDP.
// Third arg is useless, just 0 (is it a checksum or just a parameter filler. Maybe its a packet length limiter or sum)

// The combination of the 3 args determines the socket protocol
//
// IPv4+TCP -- socket(AF_INET, SOCK_STREAM, 0)
// IPv6+TCP -- socket(AF_INET6, SOCK_STREAM, 0)
// IPv4+UDP -- socket(AF_INET, SOCK_DGRAM, 0)
// IPv6+UDP -- socket(AF_INET6, SOCK_DGRAM, 0)
// Required includes are in 'man ip.7'


int val = 1;
setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val));

// The combination of the 2nd and 3rd arguments specifies which option to set
// 4th Argument is the option value
// Different options use different types, so the size of the option value is also needed

// SO_REUSEADDR gets set to 1, which allows the server program to bind to the same IP:port after restart. Enable on all !listening! sockets



struct sockaddr_in addr = {};

addr.sin_family = AF_INET; 
addr.sin_port = htons(1234);
addr.sin_addr.s_addr = htonl(0);
int rv = bind(fd, (const struct sockaddr *)&addr, sizeof(addr));
if (rv) { die("bind()"); }

// Sets an empty struct of type sockaddr_in named addr
// Adds the ipv4 address aswell as the port
// Then, sets the addr to any ip, meaning its listening for all IPs
// Proceeds, to bind socket to address

/* Side Note:
 * Little-Endian: Least significant byte comes first
 * Big-Endian: Most significant byte comes first
 *
 *
 *
 * htonl(): Host To Network Long. "Host" means CPU Endian. "Network" means big-endian. "long" means uint32_t
 * ---> little endian cpus do a byte swap, big endian CPUS do nothing
*/


rv = listen(fd, SOMAXCONN); // This is where the socket is created 

if (rv) { die("listen()"); }

while (true) {
	struct sockaddr_in client_addr = {};

	socklen_t addrlen = sizeof(client_addr);
	int connfd = accept(fd, (struct sockarr *)&client_addr, &addrlen);
	if (connfd < 0) {
		continue; // Error, Accept returns client address, meaning if it's less than 0 an error occured
	}

	do_something(connfd);
	close(connfd);
}


static void do_something(int connfd) {
	char rbuf[64] = {};
	ssize_t n = read(connfd, rbuf, sizeof(rbuf) - 1);
	if (n < 0) {
		msg("read() error");
		return;
	}
	printf("client says: %s/n", rbuf);

	char wbuf[] = "world";
	write(connfd, wbuf, strlen(wbuf));
}

// Read/Write == Send/Recv without the additional flags

// Note: 
// getsockname(int fd, struct sockaddr *addr, socklen_t *addrlen); --> local address
// getpeername(int fd, struct sockaddr *addr, socklen_t *addrlen); --> remote address
// getaddrinfo() resolves a domain name into IP addresses. Check the man page.







