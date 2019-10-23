#ifndef COMMON_H_INCLUDED
#define COMMON_H_INCLUDED

#define _GNU_SOURCE

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include "error.h"


// Порт сокета на сервере
#define SOCKET_PORT	55123

// Размер буфера данных
#define BUF_SIZE	0xFFFF

// Порт источника для режима RAW сокетов
#define SOURCE_PORT 55999

// Размер заголовка UDP
#define UDP_H_SIZE	sizeof(struct udphdr)

// Размер заголовка IP
#define IP_H_SIZE	20


#endif // COMMON_H_INCLUDED
