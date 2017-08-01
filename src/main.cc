#include <iostream>

#include <uv.h>

#include "ipinterface.h"

using namespace std;

int main( int argc, char** argv )
{
    IPInterface popo(0,"wlan0");

    uv_run(uv_default_loop(), UV_RUN_DEFAULT );
    return 0;
}
