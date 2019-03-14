
#include "ipcrxstream.h"
#include "utils.h"

#include <assert.h>
#include <nanomsg/nn.h>
#include <nanomsg/pipeline.h>

umn::IPCRxStream::IPCRxStream(const std::string &endpoint)
{
    _sock = nn_socket (AF_SP, NN_PULL);
    assert (_sock >= 0);
    assert (nn_bind (_sock, endpoint.c_str()) >= 0);
}

umn::IPCRxStream::~IPCRxStream()
{
    nn_shutdown (_sock, 0);
}

size_t umn::IPCRxStream::read(uint8_t *buffer, size_t buffer_size)
{
    int bytes = nn_recv (_sock, buffer, buffer_size, NN_DONTWAIT );
    UMN_DEBUG_PRINT( "bytes=%d\n", bytes );

    if( bytes <= 0 )
        bytes = 0;
    return bytes;
}
