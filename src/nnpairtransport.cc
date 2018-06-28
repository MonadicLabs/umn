
#include "nnpairtransport.h"

#include <nn.h>
#include <pair.h>

void
fatal(const char *func)
{
    fprintf(stderr, "%s: %s\n", func, nn_strerror(nn_errno()));
    exit(1);
}

umn::NNPairTransport::NNPairTransport(const std::string &url)
{
    if ((_sock = nn_socket(AF_SP, NN_PAIR)) < 0) {
        fatal("nn_socket");
    }
    if (nn_bind(_sock, url.c_str()) < 0) {
        fatal("nn_bind");
    }
}

umn::NNPairTransport::~NNPairTransport()
{

}

int umn::NNPairTransport::fd()
{
    int fd;
    size_t sz = sizeof (fd);
    nn_getsockopt (_sock, NN_SOL_SOCKET, NN_RCVFD, &fd, &sz);
    return fd;
}

int umn::NNPairTransport::read(uint8_t *buffer, size_t len){ return -1; }

int umn::NNPairTransport::write(uint8_t *buffer, size_t len){ return -1; }
