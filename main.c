#include "consts.c"
#include "macchanger.c"

int main(int argc, char **argv) {

  char *interface = NULL;
  char *address = NULL;
  bool random = false;

  int opt;

  while ((opt = getopt(argc, argv, "i:a:rh")) != -1) {
    switch (opt) {
    case 'i':
      interface = optarg;
      break;
    case 'a':
      address = optarg;
      break;
    case 'r':
      random = true;
      break;
    case 'h':
      printf("%s\n", help);
      exit(0);
      break;
    case '?':
      printf("%s\n", help);
      exit(0);
      break;
    }
  }

  if (!interface) {
    printf("%s\n", "Interface is required");
    exit(0);
  }
  if (!address && !random) {
    printf("%s\n",
           "Address is required or -r can be specified to generate randomone");
    exit(0);
  }

  int sfd = openSocket();
  printMac(sfd, interface);

  if (random) {
    printf("%s\n", "random mac address will be supported soon");
    exit(0);
  }

  changeMac(sfd, interface, address);

  printf("Mac has changed successfully\n");

  return 0;
}
