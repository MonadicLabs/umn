
#include "nntx.h"

#include <nn.h>
#include <bus.h>

#include <iostream>
using namespace std;

umn::NNTx::NNTx(const std::__cxx11::string &remoteUrl)
{
    if ((_remotesock = nn_socket(AF_SP, NN_BUS)) < 0) {
       // fatal("nn_socket");
        cerr << "nope." << endl;
    }

    if (nn_connect(_remotesock, remoteUrl.c_str()) < 0) {
        // fatal("nn_connect");
        cerr << "nope again." << endl;
    }

}

umn::NNTx::~NNTx()
{

}

int umn::NNTx::fd()
{
    return -1;
}

int umn::NNTx::write(uint8_t *buffer, size_t len)
{
    int r = nn_send( _remotesock, buffer, len, NN_DONTWAIT );
    return r;
}
