#include "core.c"
#include <errno.h>
#include <fcntl.h>
#include <ifaddrs.h>
#include <linux/in.h>
#include <net/if.h>
#include <net/if_arp.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <unistd.h>

int openSocket();
void printMac(int socketfd, char *interface);
void changeMac(int socketfd, char *interface, char *address);
bool isValidInterface(char *interface);
bool isValidAddress(char *address);
