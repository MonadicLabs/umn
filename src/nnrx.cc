
#include "nnrx.h"

#include <nn.h>
#include <bus.h>

void
fatal(const char *func)
{
    fprintf(stderr, "%s: %s\n", func, nn_strerror(nn_errno()));
    exit(1);
}

umn::NNRx::NNRx(const std::string &localUrl)
    :_url(localUrl)
{
    if ((_localsock = nn_socket(AF_SP, NN_BUS)) < 0) {
        fatal("nn_socket");
    }

    if (nn_bind(_localsock, localUrl.c_str()) < 0) {
        fatal("nn_bind");
    }
    size_t sz = sizeof (_pollable);
    nn_getsockopt (_localsock, NN_SOL_SOCKET, NN_RCVFD, &_pollable, &sz);
}

umn::NNRx::~NNRx()
{

}

int umn::NNRx::fd()
{
    return _pollable;
}

int umn::NNRx::read(uint8_t *buffer, size_t len)
{
    int r = nn_recv( _localsock, buffer, len, NN_DONTWAIT );
    return r;
}
