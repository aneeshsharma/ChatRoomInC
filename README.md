# ChatRoomInC
A chat room implementation in C using TCP sockets.
> NOTE: This implementation is not encrypted and messages are sent in plain text.

## Prerequisite
To run this program, make sure you have -
- A linux system
- `gcc` compiler
- `make` utility

To install `gcc` and `make` on linux -
### Debian based (Ubuntu, Pop!_OS, etc.)
```
$ sudo apt install gcc make
```

### Arch based (Manjaro, Arco, etc.)
```
$ pacman -S gcc make
```

## Usage
First clone the repository and change to the directory
```
$ git clone https://github.com/aneeshsharma/ChatRoomInC
$ cd ChatRoomInC
```
To run this program, you need to compile the server and client programs.
```
$ make
```

Then, to run the server, execute the `server.o` file (output from `make`).
```
$ ./server.o
```

To run the client, execute the `client.o` file (output from `make`).
```
$ ./client.o
```

> Currently, the server would use the port 5000 and the client would connect to the server using the loopback address (127.0.0.1).