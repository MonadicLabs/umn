
#include "ipctxstream.h"

#include <assert.h>
#include <nanomsg/nn.h>
#include <nanomsg/pipeline.h>

umn::IPCTxStream::IPCTxStream(const std::string &endpoint )
{
    _sock = nn_socket (AF_SP, NN_PUSH);
    assert (_sock >= 0);
    assert (nn_connect (_sock, endpoint.c_str()) >= 0);
}

umn::IPCTxStream::~IPCTxStream()
{
    nn_shutdown (_sock, 0);
}

size_t umn::IPCTxStream::write(uint8_t *buffer, size_t buffer_size)
{
    int bytes = nn_send (_sock, buffer, buffer_size, NN_DONTWAIT);
    return bytes;
}
