#include <errno.h>
#include <fcntl.h>
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

#define CHECK_ERRNO(err)                                                       \
  ({                                                                           \
    int __val = (err);                                                         \
    (__val == -1 ? ({                                                          \
      fprintf(stderr, "ERROR: (%s:%d) -- %s\n", __FILE__, __LINE__,            \
              strerror(errno));                                                \
      exit(-1);                                                                \
      -1;                                                                      \
    })                                                                         \
                 : __val);                                                     \
  })

int openSocket();
void printMac(int socketfd);
void changeMac(int socketfd);

int main(int argc, char **argv) {

  int sfd = openSocket();
  printMac(sfd);
  changeMac(sfd);

  printf("Mac has changed successfully\n");

  return 0;
}

int openSocket() {
  int sfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
  CHECK_ERRNO(sfd);
  return sfd;
}

void printMac(int socketfd) {
  struct ifreq *req = (struct ifreq *)malloc(sizeof(struct ifreq));
  strcpy(req->ifr_ifrn.ifrn_name, "enp0s3");
  req->ifr_ifru.ifru_hwaddr.sa_family = AF_INET;

  int res = ioctl(socketfd, SIOCGIFHWADDR, req);
  CHECK_ERRNO(res);

  printf("MAC Address: %02x:%02x:%02x:%02x:%02x:%02x\n",
         (unsigned char)req->ifr_ifru.ifru_hwaddr.sa_data[0],
         (unsigned char)req->ifr_ifru.ifru_hwaddr.sa_data[1],
         (unsigned char)req->ifr_ifru.ifru_hwaddr.sa_data[2],
         (unsigned char)req->ifr_ifru.ifru_hwaddr.sa_data[3],
         (unsigned char)req->ifr_ifru.ifru_hwaddr.sa_data[4],
         (unsigned char)req->ifr_ifru.ifru_hwaddr.sa_data[5]);
}

void changeMac(int socketfd) {
  struct ifreq *req = (struct ifreq *)malloc(sizeof(struct ifreq));
  strcpy(req->ifr_ifrn.ifrn_name, "enp0s3");
  req->ifr_ifru.ifru_hwaddr.sa_family = ARPHRD_ETHER;

  const char *addrStr = "40:32:58:12:85:12";
  unsigned char addrBytes[6];
  sscanf(addrStr, "%02hhx:%02hhx:%02hhx:%02hhx:%02hhx:%02hhx:", &addrBytes[0],
         &addrBytes[1], &addrBytes[2], &addrBytes[3], &addrBytes[4],
         &addrBytes[5]);

  memcpy(req->ifr_ifru.ifru_hwaddr.sa_data, addrBytes, 6);

  int res = ioctl(socketfd, SIOCSIFHWADDR, req);
  CHECK_ERRNO(res);
}
