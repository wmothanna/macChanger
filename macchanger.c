#include "macchanger.h"

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

#define CHECK_ERRNO_FREE(err, toFree)                                          \
  ({                                                                           \
    int __val = (err);                                                         \
    void *__toFree = (toFree);                                                 \
    (__val == -1 ? ({                                                          \
      fprintf(stderr, "ERROR: (%s:%d) -- %s\n", __FILE__, __LINE__,            \
              strerror(errno));                                                \
      __toFree ? free(__toFree) : exit(-1);                                    \
      exit(-1);                                                                \
      -1;                                                                      \
    })                                                                         \
                 : __val);                                                     \
  })

int openSocket() {
  int sfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
  CHECK_ERRNO(sfd);
  return sfd;
}

void printMac(int socketfd, char *interface) {
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

void changeMac(int socketfd, char *interface, char *address) {
  strToLower(interface);
  if (!isValidInterface(interface)) {
    printf("No such interface with the name: \"%s\"\n", interface);
    exit(0);
  }

  strToLower(address);
  if (!isValidAddress(address)) {
    printf("\"%s\" invalid address\n", address);
    exit(0);
  }

  struct ifreq *req = (struct ifreq *)malloc(sizeof(struct ifreq));
  if (!req) {
    fprintf(stderr, "ERROR: (%s:%d) -- %s\n", __FILE__, __LINE__,
            strerror(errno));
    exit(-1);
  }

  strcpy(req->ifr_ifrn.ifrn_name, interface);
  req->ifr_ifru.ifru_hwaddr.sa_family = ARPHRD_ETHER;

  const char *addrStr = address;
  unsigned char addrBytes[6];
  sscanf(addrStr, "%02hhx:%02hhx:%02hhx:%02hhx:%02hhx:%02hhx:", &addrBytes[0],
         &addrBytes[1], &addrBytes[2], &addrBytes[3], &addrBytes[4],
         &addrBytes[5]);

  memcpy(req->ifr_ifru.ifru_hwaddr.sa_data, addrBytes, 6);

  int res = ioctl(socketfd, SIOCSIFHWADDR, req);
  CHECK_ERRNO_FREE(res, req);
  free(req);
}

bool isValidInterface(char *interface) {
  struct ifaddrs *addrs, *tmp;

  getifaddrs(&addrs);
  tmp = addrs;

  while (tmp) {
    if (tmp->ifa_addr && tmp->ifa_addr->sa_family == AF_PACKET)
      if (strcmp(tmp->ifa_name, interface) == 0)
        return true;

    tmp = tmp->ifa_next;
  }

  freeifaddrs(addrs);
  return false;
}

bool isValidAddress(char *address) {
  // TODO:
  // [ ] Check First Byte
  // [ ] Check regex

  if (strlen(address) != 17 || strcmp(address, "00:00:00:00:00:00") == 0 ||
      strcmp(address, "ff:ff:ff:ff:ff:ff") == 0) {
    return false;
  }
  return true;
}
