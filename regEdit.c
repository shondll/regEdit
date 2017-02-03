#include <stdio.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <getopt.h>

/**
  print usage information
*/
static void usage(FILE* fp, int argc, char** argv)
{
  fprintf (fp,
    "Usage: %s [options]\n\n"
    "Options:\n"
    "-d | --device name   Video device name [/dev/mem]\n"
    "-h | --help          Print this message\n"
    "-w | --write         write register with value\n"
    "-r | --read          read register\n"
    "-b | --base          base address\n"
    "-f | --offset        offset from base address\n"
    "",
    argv[0]);
}

static const char short_options [] = "d:hw:rb:f:";

static const struct option
long_options [] = {
        { "device",     required_argument,      NULL,           'd' },
        { "help",       no_argument,            NULL,           'h' },
        { "write",      required_argument,      NULL,           'w' },
        { "read",       no_argument,            NULL,           'r' },
        { "base",       required_argument,      NULL,           'b' },
        { "offset",     required_argument,      NULL,           'f' },
        { 0, 0, 0, 0 }
};

static char* deviceName = "/dev/mem";
static off_t baseAddr = 0;
static off_t addrOffset = 0;
static char reading = 0;
static char writing = 0;

int main(int argc, char **argv)
{
        int mem;
        int flags;
        unsigned int value;
        unsigned int toWrite;
        void *ptr;

//        usage (stdout, argc, argv);
//        exit(EXIT_FAILURE);

        for (;;) {
          int index, c = 0;

          c = getopt_long(argc, argv, short_options, long_options, &index);

          if (-1 == c)
            break;

          switch (c) {
            case 0: /* getopt_long() flag */
              break;

            case 'd':
              deviceName = optarg;
              break;

            case 'h':
              // print help
              usage (stdout, argc, argv);
              exit(EXIT_SUCCESS);

            case 'w':
              // writing mode
              writing = 1;
              sscanf(optarg, "0x%X", &toWrite);
              break;

            case 'r':
              // reading mode
              reading = 1;
              break;

            case 'b':
              // set base addr
              sscanf(optarg, "0x%X", (unsigned int*)&baseAddr);
              break;

            case 'f':
              // set offset
              sscanf(optarg, "0x%X", (unsigned int*)&addrOffset);
              break;

            default:
              usage(stderr, argc, argv);
              exit(EXIT_FAILURE);
          }
        }

        /* Open /dev/mem */
        flags = O_SYNC;
        flags |= writing ? O_RDWR : O_RDONLY;
        if ((mem = open (deviceName, flags)) == -1)
                fprintf(stderr, "Cannot open %s\n", deviceName), exit(1);

        flags = PROT_READ;
        flags |= writing ? PROT_WRITE : 0;

//                printf("base: 0x%X\n", (unsigned int)baseAddr);
//                printf("offset: 0x%X\n", (unsigned int)addrOffset);

        ptr = mmap (0, (size_t)addrOffset + 1, flags, MAP_SHARED, mem, baseAddr);
        if(ptr == (void *) -1) {
                printf("Memory map failed (probably base address should be page boundary). error %i\n", (unsigned int)ptr);
                perror("mmap");
                exit(EXIT_FAILURE);
        }

        if(reading) {
                value = *((unsigned int*)(ptr + addrOffset));
                printf("0x%08X\n", value);
        }

        if(writing) {
        	value = toWrite;
        	*((unsigned int*)(ptr + addrOffset)) = value;
        }

        munmap(ptr, (size_t)addrOffset + 1);

        if (close(mem) == -1)
                fprintf(stderr, "Cannot close %s\n", deviceName), exit(1);

        return 0;

//        /* Open /dev/mem */
//        printf("Opening %s\n", deviceName);
//        printf("base: 0x%X\n", (unsigned int)baseAddr);
//        printf("offset: 0x%X\n", (unsigned int)addrOffset);
//
//
//        if(reading) {
//                value = 0x55555559;
//                printf("0x%X", value);
//        }
//
//        if(writing) {
//        	value = toWrite;
//        }
//
//        return 0;
}
