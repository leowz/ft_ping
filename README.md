# ft_ping

Re-code ping command. Reference ping implementation from inetutils-2.0

[inetuils reference](https://ftp.gnu.org/gnu/inetutils/)

## Definition:

ping is a computer network administration software utility used to test the reachability of a host on an Internet Protocol (IP) network. It is available for virtually all operating systems that have networking capability, including most embedded network administration software.

Ping measures the round-trip time(RTT) for messages sent from the originating host to a destination computer that are echoed back to the source. The name comes from active sonar terminology that sends a pulse of sound and listens for the echo to detect objects under water.

Ping operates by means of Internet Control Message Protocol (ICMP) packets. Pinging involves sending an ICMP echo request to the target host and waiting for an ICMP echo reply. The program reports errors, packet loss, and a statistical summary of the results, typically including the minimum, maximum, the mean round-trip times, and standard deviation of the mean.

The command-line options of the ping utility and its output vary between the numerous implementations. Options may include the size of the payload, count of tests, limits for the number of network hops (TTL) that probes traverse, interval between the requests and time to wait for a response. Many systems provide a companion utility ping6, for testing on Internet Protocol version 6 (IPv6) networks, which implement ICMPv6.

ICMP is implemented within the operating system's networking stack and is often handled by the kernel of the operating system. ICMP messages are generated, received, and processed by the networking software running on routers, computers, and other network devices. There isn't a separate service or daemon specifically for ICMP; it's an integral part of the networking infrastructure.

## Functions allowed:

- inet_ntop / inet_pton: convert binary net address to text form address or vice versa in #include <arpa/inet.h>
- ntohs / htons: convert values between host and network byte order. convert value order. Endian related.
- alarm: send an SIGALRM to calling process in given secondes
- socket: create an endpoint for communication using domain, type and protocal, get a file descriptor as return. in  #include <sys/socket.h>
- setsockopt: set options for the socket.
- sendto /recvmsg: send and receive message on a socket.
- getpid / getuid: get process id and get user id from the calling process.
- getaddrinfo: returns one or more addrinfo structures,
each of which contains an Internet address that can be specified
in a call to bind or connect
- getnameinfo: it converts a socket address to a corresponding host and service, in
a protocol-independent manner

## Socket

A **network socket** is a software structure within a [network node](https://en.wikipedia.org/wiki/Node_(networking)) of a [computer network](https://en.wikipedia.org/wiki/Computer_network) that serves as an endpoint for sending and receiving data across the network. The structure and properties of a socket are defined by an [application programming interface](https://en.wikipedia.org/wiki/Application_programming_interface) (API) for the networking architecture. Sockets are created only during the lifetime of a [process](https://en.wikipedia.org/wiki/Process_(computing)) of an application running in the node.

Because of the [standardization](https://en.wikipedia.org/wiki/Standardization) of the [TCP/IP](https://en.wikipedia.org/wiki/TCP/IP) protocols in the development of the [Internet](https://en.wikipedia.org/wiki/Internet), the term *network socket* is most commonly used in the context of the Internet protocol suite, and is therefore often also referred to as **Internet socket**. In this context, a socket is externally identified to other hosts by its **socket address**, which is the triad of [transport protocol](https://en.wikipedia.org/wiki/Transport_protocol), [IP address](https://en.wikipedia.org/wiki/IP_address), and [port number](https://en.wikipedia.org/wiki/Port_number).

The term *socket* is also used for the software endpoint of node-internal [inter-process communication](https://en.wikipedia.org/wiki/Inter-process_communication) (IPC), which often uses the same API as a network socket.

Types: Datagram socket, Stream socket, Raw socket.

Use socket() to create a socket which returns a file descriptor of this socket created.

## ICMP

The **Internet Control Message Protocol** (**ICMP**) is a supporting [protocol](https://en.wikipedia.org/wiki/Communications_protocol) in the [Internet protocol suite](https://en.wikipedia.org/wiki/Internet_protocol_suite). It is used by [network devices](https://en.wikipedia.org/wiki/Network_device), including [routers](https://en.wikipedia.org/wiki/Router_(computing)), to send error messages and operational information indicating success or failure when communicating with another [IP address](https://en.wikipedia.org/wiki/IP_address), for example, an error is indicated when a requested service is not available or that a [host](https://en.wikipedia.org/wiki/Host_(network)) or router could not be reached.[[2]](https://en.wikipedia.org/wiki/Internet_Control_Message_Protocol#cite_note-Forouzan-2) ICMP differs from [transport protocols](https://en.wikipedia.org/wiki/Transport_protocol) such as [TCP](https://en.wikipedia.org/wiki/Transmission_Control_Protocol) and [UDP](https://en.wikipedia.org/wiki/User_Datagram_Protocol) in that it is not typically used to exchange data between systems, nor is it regularly employed by end-user network applications (with the exception of some diagnostic tools like [ping](https://en.wikipedia.org/wiki/Ping_(networking_utility)) and [traceroute](https://en.wikipedia.org/wiki/Traceroute)).

ICMP for [IPv4](https://en.wikipedia.org/wiki/IPv4) is defined in [RFC](https://en.wikipedia.org/wiki/RFC_(identifier)) [792](https://datatracker.ietf.org/doc/html/rfc792). A separate [ICMPv6](https://en.wikipedia.org/wiki/ICMPv6), defined by RFC 4443, is used with [IPv6](https://en.wikipedia.org/wiki/IPv6).

Need to use ICMP_ECHO and ICMP_ECHOREPLY to send and receive message.

## Select:

Select is necessary for test if the socket is able to receive a message. If select is not used and only use usleep. If you create a blocking socket, then the program cannot be stopped by ctr +c. 

If you create a none blocking then you risk drop the first package or mess up the last package time

```c
int select(int nfds, fd_set *_Nullable restrict readfds,
                  fd_set *_Nullable restrict writefds,
                  fd_set *_Nullable restrict exceptfds,
                  struct timeval *_Nullable restrict timeout);
```

select() allows a program to monitor multiple file descriptors, waiting until one or more of the file descriptors become "ready" for some class of I/O operation (e.g., input possible).  A file descriptor is considered ready if it is possible to perform a corresponding I/O operation without blocking.