// Imagine there are imports here



int fd = socket(AF_INET, SOCK_STREAM, 0);
if (fd < 0) {
	die("socket()");
}
// Im assuming die() kills the process



struct sockaddr_in addr = {};

addr.sin_family = AF_INET;
addr.sin_port = ntohs(1234);
addr.sin_addr.s_addr = ntohl(INADDR_LOOPBACK); //127.0.0.1

int rv = connect(fd, (const struct sockaddr *)&addr, sizeof(addr));
if (rv) {
	die("connect");
}

char msg[] = "hello";
write(fd, msg, strlen(msg));

char rbuf[64] = {};
ssize_t n = read(fd, rbuf, sizeof(rbuf) - 1);
if (n < 0) {
	die("read");
}

printf("server says: %\n", rbuf);
close(fd);


