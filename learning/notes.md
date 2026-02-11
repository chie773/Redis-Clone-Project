# This is my notepad for my redis project.


```
I can't find my apple pen.
```


## Introduction

### What is Redis?

Redis is an in-memory key-value store used primarily for caching as no storage is faster than memory. Caching servers are inevitable because it's the easiest way to scale. A cache can take slow DB queries out of the equation.


A cache server is a map<string, string> over a network. But in Redis, the 'value' part is not just a sring, it can be any of the data sructures: hash, list, or sorted set, enabling complex use cases such as ranking, list pagination, etc. This is why Redis is called a data structure server.


### What I can not create, I do not understand


### Why C++?

- C doesn't have built-in data structures or networking, forcing learning
- Softwares like this that incorporate hardware require low level control
- C is minimal, motiving learning with C

## What to Learn

### Network Programming
- Network Programming is not trivial

#### TCP Byte Stream and Protocols

People often visualize computer network as peers exchanging "messages". But the most common protocol, TCP -- Transmission Control Protocol, doesn't acutally produce messages. It produces a continuous stream of bytes, with no internal boundaries. Interpreting this *byte stream* is the job of an *application protocol* -- rules for making sense of the byte stream, including how to split it into messages.

Splitting a byte stream into messages is trickier than you might think, especially within an event loop. It's not like parsing some file formats.

#### Data Serialization 

The "message" you want to send via a network can be high-level objects like strings, structs, list. However, computer networks only know 0s and 1s. Thus, it's necessary to create a mapping between objects and bytes. This is called serialization (object to bytes) and deserialization (bytes to object). 

While you can do this trivially via JSON or Protobuf libraries, rolling your own by manipulating bits and bytes is a good start to low-level programming.

#### Concurrent Programming

With a protocol specification, you can easily build the client application. But the server side is more complicated because it deals with multiple connections. Dealing with many concurrent connections (even mostly idle) is historically difficult (the C10K problem). Although 10K is a tiny number on modern hardware, efficient software is required to make a full use of the hardware. 

The modern software solution is *event-based concurrency* with *event loops*, which drive modern scalable software like NGINX, Redis, Golang's runtime, etc. Event-based concurrency may be aa new programming paradigm for you, and it's really complicated, so you should learn it by doing.


### Network from a programmers' perspective 

#### Layers of protocols

Abstract Idea: Network protocols are divieded into layers. A lower layer can contain higher layer can contain a higher layer as payload, and the higher layer adds new functions. 

Reality: Ethernet contains IP, IP caontains UDP or TCP, UDP or TCP contains application protocols.

Alternatively, we can alos divide the layers by function:


#### The layer of small, discrete messages (IP)

When you download a large file, the hardware cannot possibly store the whole data before forwarding, it can only handle smaller units (IP packets). That's why the lowest layer is packet-based. The ability to asssemble packets into application data is provided by a higher layer, usually TCP.

#### The layer of multiplexing (Port number)

Multiple apps can share the same network on a single computer. Hoow does the computer know which packet belongs to which app? This is called *demultiplexing*. The next layer of IP (UDP or TCP) adds a 16-bit *port number* to distinguish different apps. Each app must claim an unused local port number before it can send or receive data. The computer uses the 4-tuple to identify a "flow" of information:

(src-ip, src-port, dst-ip, dst-port)

#### The layer of reliable and ordered bytes (TCP)

Small messages are usually not what you want. The file transfer use case requires arbitrarily large data. Worse, the network is unreliable, IP packets can be dropped, reordered. TCP provides a layer of reliable and ordered bytes on top of IP packets, it handles retransmissino, reordering automatically.

#### TCP/IP Model

Netowkr protocols layered by function:

Higher --> TCP ---> Reliable & Ordered Bytes
Middle --> Port in TCP/UDP --> Multiplex to programs
Lower --> IP --> Small, discrete messages
 
3 Layers represent 3 needs in networking. They are mapped nicely  to TCP/IP concepts. THere are other models, such as the TCP/IP model:

Application --> Transport Layer (TCP/UDP) --> IP Layer --> Link Layer (Below IP)

The TCP/IP model represents the protocol header structures, which puts TCP and UDP on the same leve, but TCP has a higher function, while UDP is just IP+port.


##### We will be using everything above the IP layer

 
#### Request-Response protocols

Redis, HTTP/1.1, and most RPC protocols are *request-response* protocols. Each request message is paired with a response message. If the messages are not both reliable and ordered, there will be troubles pairing a response with a request. Thus, most request-response protocols are based on TCP (DNS is an exception).


#### Packet vs. Stream

TCP provides a byte stream, but typical apps expect messages; few apps use the byte stream without interpreting it. Thus, we either need to add a message layer to TCP, or add reliability and order to UDP. THe formeris far easier, so most apps use TCP, either by using a well-known protocol on top of TCP, or by rolling their own protocol.

TCP and UDP are not only functionally different, their semantics are incompatible. TCP or UDP is the first thing to decide for networked applications.


### Socket Primitives

A socket is a *handle* to refer to a connection or something else. The API for networking is called the socket API, which is similar on different OSes.

A handle is an opaque integer used to referr to things that cross an API boundary. In the same way that a twitter handle refers to a twitter user. On linux, a hjandle is called a file descriptor (fd) and it's local to the process. THe name "file descriptor" is just a name; it has nothing to do with files, nor does it describe anything.

Socket() --> allocates and returns a socket fd (handle), which is used later to actually create connections.

A handle must be closed when you're done to free the associated resources on the OS side. This is the only thing in common between different types of handles

#### Listening Socket and connection socket

*Listening* -- Telling the OS that an app is ready to accept TCP connections from a given prot. The OS then returns a socket handle for apps to refer to that port. From the listening socket, apps can retrieve (*accept*) incoming TCP connections, which is also represented as a socket handle. So there are 2 types of handles: listening socket & connection socket.

Creating a listening socket requires at least 3 API calls:

- Obtain a socket handle via socket()
- Set the listening IP:port via bind()
- Create the listening socket via listen()

The use the accept() API to wait for incoming TCP connections. Pseudo code:

#### Connect from a client

A connection socket is created from the client side with 2 API calls:

- Obtain a socket handle via socket()
- Create the connection socket via connect()

```python

fd = socket()
connect(fd, address)
do_something_with(fd)
close(fd)

```

socket() creates a typeless socket; the socket type is determined after the listen() or connect() call. The bind() between socket() and listen() merely sets a parameter. The setsockop() API can set other socket parameters that will be used later.

#### Read and write

Although TCP and UDP provide different types of services, they share the same socket API, including the send() and recv() methods. For message-based sockets (UDP), each send/recv corresponds to a single packet. For byte-stream-based sockets (TCP), each send/recv appends to/consumes from the byte stream.

On Linux, send/recv are just a variant of the more generic read/write syscalls used for both sockets, disk files, pips, and etc. Different types of handle sharing the same read/write API is merely a coincidence; it's unlikely to have piece of code that works for both TCP and UDP, as they are semantically incompatible.

#### Summary: list of socket primitives

Listening TCP socket:
- bind() and listen()
- accept()
- close()

Using a TCP socket:
- read()
- write()
- close()

Create a TCP connection: connect()



#### Documentation Commands

- man read.2 --> returns the read() syscall
- man read --> returns the read() shell command
- man socket.2 return the socket() syscall --- All socketAPI methods are syscalls
- man socket.7 returns the socket interface overview, not the syscall.

*Create a TCP Server --> See practiceServer + practiceClient C files*

#### Sockets and Inter-Process Communication (IPC)

There are mechanisms that allow processes within the same machine to communicate such as Unix domain sockets, pipes, etc. They are just a computer network confined to a single machine, so the programming techniques are the same.

Unix domain sockets share teh same API with network sockets. You can create either packet-based or byte-stream-based Unix domain sockets, like UDP or TCP. A Unix domain socket is created with different flags on the socket() method and uses struct sockaddr-un, but the rest is the same. *Read man unix.7 for more info*

A *pipe* is a one-way byte stream. So you need a protocol like a TCP socket, which is not as trivial as you might think.


### Request Respone Protocols

Protocols have 2 levels of structure (usually)

- High-Level structure to split the byte stream into messages
- The structure within a message, aka deserialization

#### Simple Binary Protocol

Each message consists of a 4byte little endian integer indicating the length of the request and the variable-length payload. This is not the real Redis protocol.

#### Parse the Protocol

**Check the return value of read/write**

read/write returns the number of bytes read/written. The return value is -1 on err. (read also returns 0 after EOF.

To read a message, first read the 4-byte integer, then read the payload.

However, read/write can return less than the requested number of bytes under normal conditions. Will be explained later


To actually read/write n butes from/to a TCP socket. You must do it in a loop.

*This is the code from the website (me lazy)*

static int32_t read_full(int fd, char *buf, size_t n) {
    while (n > 0) {
        ssize_t rv = read(fd, buf, n);
        if (rv <= 0) {
            return -1;  // error, or unexpected EOF
        }
        assert((size_t)rv <= n);
        n -= (size_t)rv;
        buf += rv;
    }
    return 0;
}

static int32_t write_all(int fd, const char *buf, size_t n) {
    while (n > 0) {
        ssize_t rv = write(fd, buf, n);
        if (rv <= 0) {
            return -1;  // error
        }
        assert((size_t)rv <= n);
        n -= (size_t)rv;
        buf += rv;
    }
    return 0;
}


Whatever a read returns is accumulated in a buffer. It's how much data you have that matters, how much a single read returns matters not.

#### Producing the Response

*In the server program, reaed__full and write_all are used instead of read and write.*

const size_t k_max_msg = 4096;

static int32_t one_request(int connfd) {
    // 4 bytes header
    char rbuf[4 + k_max_msg];
    errno = 0;
    int32_t err = read_full(connfd, rbuf, 4);
    if (err) {
        msg(errno == 0 ? "EOF" : "read() error");
        return err;
    }
    uint32_t len = 0;
    memcpy(&len, rbuf, 4);  // assume little endian
    if (len > k_max_msg) {
        msg("too long");
        return -1;
    }
    // request body
    err = read_full(connfd, &rbuf[4], len);
    if (err) {
        msg("read() error");
        return err;
    }
    // do something
    printf("client says: %.*s\n", len, &rbuf[4]);
    // reply using the same protocol
    const char reply[] = "world";
    char wbuf[4 + sizeof(reply)];
    len = (uint32_t)strlen(reply);
    memcpy(wbuf, &len, 4);
    memcpy(&wbuf[4], reply, len);
    return write_all(connfd, wbuf, 4 + len);
}

errno is set to the error code if the syscal failed. However, errno is NOT set to 0 if the syscall succeeded; it simply keeps the previous value. That's why the above code sets errno = 0 before read_full() to distinguish th eEOF case.

You can read errno if and only if the call faield. But some libv fucntions have no way to tell if the call failed other than by clearing errno first. *Not Used in the Linux Kernel*


#### Understanding read/write

**TCP Socket vs Disk File**

There are differences between reading disk files and reading sockets despite of sharing the same read/write API. When reading a disk file and it returns less than requested, it means ether EOF or an error. But a socket can return less data even under normal conditions. This can be explained by pull-based IO and push-based IO.

Data over a network is pushed by the remote peer. The remote does not need the read call before sending data. The kernel will allocate a receive buffer to store the received data. Read just copies whatever is available from the receive buffer to the userspace buffer, since it's unknown if there is more inflight data.

Data from a local file is pulled from disk. The data is always considered "ready" and the file size is known. There is no reason to return less than requested unless it's EOF.

**Interrupted Syscalls**

Normally, write just appends data to a kernel-side buffer, however the actual network transfer is deferred to the OS. The buffer size is limited, The buffer size is limited, so when the buffer is full, the caller must wait for it to drain before copying the remaining data. During the wait, the syscall may be interrupted by a signal, causing write to return partial written data.

read can also be interrupted by a signal because it must wait if the buffer is empty. In this case, 0 bytes are read, but the return value is -1 an derrno is EINTR. This is not an error. An exercise for the reader: handle this case in read_full. (IDK how to do that)

#### More on Protols

**Text vs Binary**

Why not use something simpler like HTTP and JSON? Plain text seems simple because it's human readable. But they aren't very machine-readable due to the implementation complexity

A human-readable protocol deals with strings, strings are variable leength, so you are constantly checking the length of things, which is tedious and error-prone. While a binary protocol avoids unnecessary strings,, nothing is simpler than memcpy-ing a struct.

**Length prefixes vs delimiters**

Patterns this chapter:
- Start with a fixed-size part
- Variable-length data follows, with the length indicated by the fixed-size part

When parsing a protocol like this, you always know how much data to read

The other pattern is to use delimiters to indicate the end of the variable-length thing. To parse a delimiter protocol, keep reading until the delimiter is found. But what if the payload contains the delimiter? You now need escape sequences, which adds even more complexity. 

**Case Study: Real World Protocols**

HTTP headers are stirngs delimited by \r\n, each header is a KV pair delimited by colon. The URL may contain \r\n, so the URL in the request line must be escaped/encoded. You might forget that \r\n is not allowed in header values, which has caused some security vulnerabilities

EX:

GET /index.html HTTP/1.1 '\r\n\' --> end this line start a new one
Host: example.com
Foo: bar


## Concurrent IO Models

### Thread Based Concurrency

A connnection-oriented request-response protocol can be used for any number of request-response pairs, and the client can hold the connection as long as it wants. So there is a need to handle multiple connections simultaneously, because while the server is waiting on one client, it cannot do anything with the other clients. THis is solved by multi-threading

```python
fd = socket()
bind(fd, address)
listen(fd)
while True:
    conn_fd = accpet(fd);
    new_thread(do_something_with, conn_fd);

def do_something_with(conn_fd):
    while not_quitting(conn_fd):
        req = read_request(conn_fd);
        res = process(req);
        write_response(conn_fd, res);
    
    close(conn_fd);
```

Most modern server apps use event loops to handle concurrent IO without creating new threads.

Drawbacks of thread-based IO?
- Memory Usage: Many threads means many stacks. Stacks are used for local variables and function calls, memory usage per thread is hard to control
- Overhead: Stateless clients like PHP apps will create many short-lived connections, adding overhead to both latency and CPU usage

Forking new processes costs even more.

Multithreading and multiprocessing are still used when ther eis no need to scale to large numbers of connections, and they have a big advantage over event loops: they are easier and less error-prone.

### Event Based Concurrency

Concurrent IO is possible without threading. Let's start by examining the read() sysccall. The Linux TCP stack handles sending and receivin IP packets transparently, placing incoming data in a per-socket kernel-side buffer. read() merely copies data from the kernel-side buffer, and when the buffer is empty, read() suspends the calling thread until more data is ready.

Similarly, write() does not interact directly with the network; it merely put the data into to kernel-side buffer for the TCP stack to consume, and when the buffer is full, write() suspends the calling thread until there is room.

The need for multi-threading comes from the need to wait for each socket to become ready (to read or write). If there is a way to wait for multiple sockets at once, and then read/write whichever ones are ready, only a signle thread is needed.

Ex:
while running:
    want_read = [...] # socket fds
    want_write = [...] # socket fds
    can_read, can_write = wait_for_readiness(want_read, want_write) # blocks
    for fd in can_read:
        data = read_nb(fd) # non-blocking, only consume from buffer 
        handle_data(fd, data) # application logic without IO
    for fd in can_write:
        data = pending_data(fd) # produced by the application
        n = write_nb(fd, data) # non-blocking, only append to the buffer
        data_written(fd, n) # n <= len(data), limited by the available space

This involves 3 operating system mechanisms:
- Readiness notification: Wait for multiple sockets, reutrn when one or more are ready. "Ready"
- Non-blocking read: Assuming the read buffer is not empty, consume from it
- Non-blocking write: Assuming the write buffer is not full, put some data into it

This is called event loop. Each loop iteration waits for any readiness events, the reacts to events without blocking, so that all sockets are processed without delay.


Callbacks are common in JS. To read from something in JS, first register a callback on some event, then the data is delivered to the callback. This is what we will do next. Except in JS, the event loop is hidden, while in this project, the event loop is coded. 

### Non-blocking IO

If the read buffer is not empty, both blocking and non blocking reads will return the data immediately. Otherwise, a non-blocking read will return with errno = EAGAIN, while a blocking read will wait for more data. Non-blocking reads can be called repeatedly to fully drain the read buffer.

If the write buffer is not full, both blocking and non-blocking writes will fill the write buffrer and return immediately. Otherwise, a non-blocking write will return with errno = EAGAIN, while a blocking write will wait or more room. Non-blocking writes can be called repeatedly to fully fill the write buffer. If the data is larget than the available write buffer, a non-blocking write will do a partial wite, while a blocking write may block.

accept() is similar to read() in that it just consumes an item from a queue, so it has a non blocking mode and can provide readiness notifications

ex: 
for fd in can\_read:
    if fd is a listening socket:
        conn\_fd = accept\_nb(fd)
        handle-new-conn(conn\_fd)

    else:
        
        data = read-nb(fd)
        handle-data(fd, data)


Non blocking reads and writes us ethe same syscalls as blocking reads and writes. The 0-NoneBlock flag puts a socket in non-blocking mode

ex:
static void fd-set-nonblock(int fd){ 
    int flags = fcntl(fd, F-GetFL, 0); // Get Flags
    flags |= 0-NOnblock; // Modify Flags
    fcntl(fd, f-setfl, flags); // Set Flags
    // Handle errno
}

### Readiness API

Waiting for IO readiness is platform specific, and there are several ones on Linux

ex: can-read, can-write = wait-for-readiness(want-read, want-write)

simplest one == poll()

int poll(struct pollfd \* fds, nfds\_t nfds, int timeout);

struct pollfd {
    int fd;
    short events;   // request: want to read, write, or both?
    short revents;  // returned: can read? can write?
}

poll() takes an array of fds, each with an input flag and an output flag:
- The events flag indicates whether you want to read (POLLIN), write (POLLOUT), or both (POLLIN|POLLOUT)
- The revents flag returned from the syscall indicates the readiness

The timeout arg is used to implement timers later

#### Other Readiness APIs
- select() -- can only used 1024 fds, not suggested
- epoll\_wait() -- Linux specific, fd not pass in args, but stored in kernel (used to modify fd list) -- more scalable than poll() -- Default choice on linux
- kqueue() is BSD specific. like epoll() but requires fewer syscalls because it can batch update the list

All readiness APIs can only be used with sockets, pipes, and some special stuff like signalfd. THey cannot be used with disk files because when a socket is ready to read, it means that the data is in the read buffer, so the read is guaranteed not to block, but for a disk file, no such buffer exists in the kernel, so the readiness for a disk file is undefined.

These API;s will always report a disk file as ready, but the IO will block. So file IO must be done outside thevent loop, in a thread pool.

### Summary Of Concurrent IO Techniques

Socket -- Thread per connection -- pthread -- low scalability
Socket -- Process per connection -- fork() -- low scalability
Socket -- Event loop -- poll(),epoll() -- high scalability
File -- Thread pool -- pthread
Any -- Event loop -- io\_uring -- high scalability


## Event Loop

Rewrite the echo server from chapter 4 into an event loop

```python

while running:
    want_read = [...]
    want_write = [...]
    can_read, can_write = wait_for_readiness(want_read, want_write) # blocks
    for fd in can_read:
        data = read_nb(fd) # non-blocking, only consume from the buffer
        handle_data(fd, data) # application logic without IO
    
    for fd in can_write:
        data = pending_data(fd) # produced by the application
        n = write_nb(fd, data) # non-blocking, only append to the buffer
        data_written(fd, n) # n <= len(data), limited by the available space
```

**Application code vs event loop code**

Some libraries can abstract away the event loop: the even loop code interacts with the application code via callbacks, and the application code interacts with the event loop via a well-defined API. We are not writing a library, but there is still an implicit boundary between the event loop code and the application code.

### Per-connection state

With an event loop, an application task can span multiple loop iterations, so the stat emust be explicity stored somewhere. Here is our per connection state:

```python

struct Conn {
    int fd = -1;
    //application's intention, for the event loop
    
    bool want_read = false;
    bool want_write = false;
    bool want_close = false;

    // buffered input and output
    std::vector<uint8_t> incoming; // data to be parsed by the application
    std::vector<uint8_t> outgoing; // responses genereated by the application
};
```
- Conn::want_read & Con::want_write represents the fd list for the readiness API
- Conn::want_close tells the event loop to destroy the connection
- Conn::incoming buffers data from the socket for the protocol parser to work on
- Conn::outgoing buffers generated responses that are written to the socket

**The need for input buffers**

Since reads are now non-blocking, we cannot just wait for n bytes while parsin the protocol; the read_full() function is now irrelevant.

At each loop iteration, if the socket is ready to read:

- Do a non blocking read
- Add new data to the Conn::incoming buffer
- Try to parse the accumulated buffer
    - If there is not enough data, do nothing in that iteration
- Process the parsed message
- Remove the message from Conn::incoming

**Why buffer output data?**

Since writes are now non-blocking, we cannot write to sockets at will; data is written if the socket is ready to write. A large response may take multiple loop iterations to complete. SO the response data must be stored in a buffer (Conn::outgoing).

### The Event loop code

**Map from fd to connection state**

poll() returns a fd list. We need to map each fd to the Conn object.

```python

// a Map of all client connections, keyed by fd
std::vector<Conn *> fd2conn;
```

On Unix, an fd is allocated as the smallest available non-negative integer, so the mapping from fd to Conn can be a flat array indexed by fd, and the array will be densely packed. Nothing can be more effecient. Sometimes simple arrays can replace complex data structures like hashtables.

**The poll() syscall**

The readiness API takes a list of fds that the program wants to do IO on, then returns a list of fds ready for IO. There are 2 types of readiness: read and write

```python
can_read, can_write = wait_for_readiness(want_read, want_write)
```

We'll use poll(), it uses the same fd list for both input and output

```python
int poll(struct pollfd *fds, nfds_t nfds, int timeout);

struct pollfd {
    int fd;
    short events; // request: want to read, write or both?
    short revents; // returned: can read? can write?
};
```

- The nfds argument is the size of the fds array
- The timeout argument is set to -1, which means no timeout
- pollfd::events is a combination of POLLIN, POLLOUT, POLLERR:
    - POLLIN & POLLOUT correspond to the want\_read & want\_write fd list.
    - POLLERR indicates a socket error that we always want to be notified about
- pollfd::revents is returned by poll(). Uses the same set of flags to indicate wheter the fd is in the can\_read or can\_write list

**Step 1: Construct the fd list for `poll()`**

The application code decides the type of readiness notifications. It communicates with the even loop via the want\_read & want\_write flags in Conn, the fds argument is then constructed from these flags:

```python
    // a map of all client connections, keyed by fd
    std::vector<Conn *> fd2conn;
    // the event loop
    std::vector<struct pollfd> poll_args;
    while (true) {
        // prepare the arguments of the poll()
        poll_args.clear();
        // put the listening sockets in the first position
        struct pollfd pfd = {fd, POLLIN, 0};
        poll_args.push_back(pfd);
        // the rest are connection sockets
        for (Conn *conn : fd2conn) {
            if (!conn) {
                continue;
            }
            struct pollfd pfd = {conn->fd, POLLERR, 0};
            // poll() flags from the application's intent
            if (conn->want_read) {
                pfd.events |= POLLIN;
            }
            if (conn->want_write) {
                pfd.events |= POLLOUT;
            }
            poll_args.push_back(pfd);
        }

        // more ...
    }
```
**Step 2: call `poll()`**
```python

    while (true) {
        // prepare the arguments of the poll()
        // ...

        //wait for readiness
        int rv = poll(poll_args.data(), (ndfs_t) poll_args.size(), -1);
        if (rv < 0 && errno == EINTR) {
            continue; // not an error
        }
        if (rv < 0) {
            die("poll");
        }

        // ...

    }
```
poll() is the only blocking syscall in the entire program. Normally it returns when at least one of the fds is ready. However, it may occasionally return with errno = EINTR even if nothing is ready.

if a process receives a Unix signal during a blocking syscall, the syscall is immediately return with EINTR to give the process a chance to handle the signal. EINTR is not expected for non-blocking syscalls.

EINTR is not an error, the syscall should be retried. Even if you do not use signals, you sould still handle EINTR, because there may be unexpected source of signals.

**Step 3: Accept new connections

We put the listening socket at position 0 on the fd list.

```python
    while (true) {
    // prepare the arguments of the poll()
        poll_args.clear();
        //put the listening sockets in the first position
        struct pollfd pfd = {fd, POLLIN, 0};
        poll_args.push_back(pfd);
        // ...

        //... 
        int rv = poll(poll_args.data(), (nfds_t)poll_args.size(), -1);
        // ..

        // handle the listenig socket
        if (poll_args[0].revents) {
            if (Conn *conn = handle_accept(fd)) {
                // put it into the map 
                if (fd2conn.size() <= (size_t)conn->fd) {
                    fd2conn.resize(conn->fd + 1);
                }
                fd2conn[conn->fd] = conn;
            }
        }

        // ...
    } // event loop

```

accept() is treated as read() in readiness notifications, so it uses POLLIN. After poll() returns, check the 1st fd to see if we can accept().

handle\_accept() creates the Conn object for the new connection. We''ll code this later

**Step 4: Invoke application callbacks**

The rest of the fd list is for connection sockets. Call the application code if they are ready for IO.

```python

    while (true) {
        // ...
        // wait for readiness
        int rv = poll(poll_args.data(), (nfds_t)poll_args.size(), -1);
        // ...

        // handle connection sockets
        for (size_t i = 1; i < poll_args.size(); ++i) { // note: skip the 1st
            uint32_t ready = poll_args[i].revents;
            Conn *conn = fd2conn[poll_args[i].fd];
            if (read & POLLIN) {
                handle_read(conn); // application logic
            }
            if (ready & POLLOUT) {
                handle_write(conn); // application logic
            }
        }
    }

```

**Step 5: Terminate connections**

We always poll() for POLLERR on connection sockets, so we can destroy connection on error. Application code can also set Conn::want\_close to request the event loop to destroy the connection.

```python
    
    // handle connection sockets
    for (size_t i = 1; i < poll_args.size(); ++i) {
        uint32_t ready = poll_args[i].revents;
        Conn *conn = fd2conn[poll_args[i].fd];
        // read & write ...
        
        // close the socket from socket error or application logic 
        if ((ready & POLLERR) || conn-> want_close) {
            (void)close(conn->fd);
            fd2conn[conn->fd] = NULL;
            delete conn;

```

You can add a callback handle\_err() to let the application code handle the error, but there is nothing to do in our application, so we just close the socket here.


### Application code with non-blocking IO

**Non-blocking `accept()`**

Before entering the event loop, make the listening socket non-blocking with fcntl.

```python
    static void fd_set_nb(int fd) {
        fcntl(fd, F_SETFL, fcntl(fd, F_GETFL, 0) | 0_NONBLOCK);
    }
```

Then the event loop calls back the application code to do accept()

```python
static Conn *handle_accept(int fd) {
    //accept
    struct sockaddr_in client_addr = {};
    socklen_t addrlen= sizeof(client_addr);
    int connfd = accept(fd, (struct sockaddr *)&client_addr, &addrlen);
    if (conn < 0) {
        return NULL;
    }
    
    // set the new connection fd to nonblocking mode
    fd_set_nb(connfd);
    // create a `struct Conn`
    Conn *conn = new Conn();
    conn->fd = connfd;
    conn->want_read = true; // read the 1st request
    return conn;
}    


```

The connection socket is also made non-blocking, waiting for its 1st read

**Protocol parser with non-blocking read**

```python
static void handle_read(Conn *conn) {
    // 1. Do a non-blocking read
    uint8_t buf[64 * 1024];
    ssize_t rv = read(conn->fd, buf, sizeof(buf));
    if (rv <= 0) {  // handle IO error (rv < 0) or EOF (rv == 0)
        conn->want_close = true;
        return;
    }
    // 2. Add new data to the `Conn::incoming` buffer.
    buf_append(conn->incoming, buf, (size_t)rv);
    // 3. Try to parse the accumulated buffer.
    // 4. Process the parsed message.
    // 5. Remove the message from `Conn::incoming`.
    try_one_request(conn)
    // ...
    }
}
```

The handling is split into try\_one\_request(). If there is not enough data, it will do nothing until a future loop iteration with more data.

```python
// process 1 request if there is enough data
static bool try_one_request(Conn *conn) {
    // 3. Try to parse the accumulated buffer.
    // Protocol: message header
    if (conn->incoming.size() < 4) {
        return false;   // want read
    }
    uint32_t len = 0;
    memcpy(&len, conn->incoming.data(), 4);
    if (len > k_max_msg) {  // protocol error
        conn->want_close = true;
        return false;   // want close
    }
    // Protocol: message body
    if (4 + len > conn->incoming.size()) {
        return false;   // want read
    }
    const uint8_t *request = &conn->incoming[4];
    // 4. Process the parsed message.
    // ...
    // generate the response (echo)
    buf_append(conn->outgoing, (const uint8_t *)&len, 4);
    buf_append(conn->outgoing, request, len);
    // 5. Remove the message from `Conn::incoming`.
    buf_consume(conn->incoming, 4 + len);
    return true;        // success
}

```

We use std::vector as the buffer type, which is just a dynamic array.

```python
static void 
buf_append(std::vector<uint8_t> &buf, const uint8_t *data, size_t len) {
    buf.insert(buf.end(), data, data + len);
}

// remove from the front
static void buf_consume(std::vector<uint8_t> &buf, size_t n) {
    buf.erase(buf.begin(), buf.begin() + n);
}
```

**Non-blocking write**

There is no application logic, just write smoe dat and remove it from the buffer.write() can reutrn less bytes and that's ok because the event loop will call it again.

```python

static void handle_write(Conn *conn) {
    assert(conn->outgoing.size() > 0);
    ssize_t rv = write(conn->fd, conn->outgoing.data(), conn->outgoing.size());
    if (rv < 0) {
        conn->want_close = true; // err handling
        return;
    }
    
    // remove written data from `outgoing`
    buf_consume(conn->outgoing, (size_t)rv);
}
```
**State transitions between request and response**

In a request-response protocol, the program is either reading a request or writing a response. At the end of handle\_read() and handle\_write(), we need to switch between the 2 states

```python
static void handle\_read(Conn *conn) {
    //...
    // update the readiness intention
    if (conn->outgoing.size() > 0) { // has a response
        conn->want_read = false;
        conn->want_write = true;
    } // else: want read
}
```

```python

static void handle_write(Conn *conn) {
    //...
    if (conn->outgoing.size() == 0) { // all data written
        conn->want_read = true;
        conn->want_write = false;
    } // else: want write
}

```

This is not universally true. For example, some proxies and messaging protocols are not request-response and can read and write simulteaneously

## Event Loop (Part 2)

### Event Loop Basics

- Callback-based programming: Reacting to events instead of blocking the thread
- Handling input with non-blocking IO: Keep data in a buffer until there is enough to proceed
- The boundary between the event loop and application code. A minimal event loop is still non-trivial, separate generic event handling from application logic
- Persist application state between loop iterations. For request-response protocols, alternate between 2 states (read and write).

***Bare Minimum***


### Pipelined Requests

**Batching requests without changing the protocol**

A typical Redis server can handle 20K-200K requests/second for simle operations like get, set, del. Many other req-res applications are in the same range. This is limited by the number of IO events a single thread can handle if the application logic (processing requests) is insignificant.

The server is bottlenecked by single-threaded IO, so it's desirable to increase the amount of work per IO by batching multiple requests per read.

This can be done by pipelining requests. Normally a client sends 1 request, then waits for 1 response, a pipelined client sends n requests, then waits for n responses. The server still handles each request in order, the only difference is that it can get multiple requests in 1 read, effectively reducing the number of IOs.

Requests pipelining also reduces latency on the client side because it can get multipe responses in 1 RTT (round trip time). This is a documented use case for Redis.

**Problems caused by pipelined requests**

Pipelining doese not change the protocol or server-side handling; requests are sent in order, the server handles them in order, then sends responses in order. So it should just work? Not so simple.

Remember that TCP is just a stream of ordered bytes; a server simply consuming messages from it one at a time cannot tell the difference. but implementations often make extra assumptions. For example:

```python
print("go mess around with the server + client files before continuing so yuou understand better")
```
