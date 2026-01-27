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


