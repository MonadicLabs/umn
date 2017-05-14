#pragma once

#include <vector>
#include <string>
#include <map>
#include <thread>
#include <iostream>

#include "comminterface.h"
#include "frame.h"

#ifdef USE_UV
#include <uv.h>
#else
#include "udpreceiver.h"
#include "udptransmitter.h"
#endif

#define IP_COMM_PORT 49800

namespace umn
{
    namespace ip
    {
        typedef struct
        {
            int _ttl;
            std::string _nodeIp;
            ShortId _nodeId;
            UDPTransmitter* _udpSend;
        } IpNodeReference;

        class IpInterface: public CommInterface
        {
        public:
            IpInterface( const std::string& ifName )
                :_ifName(ifName)
            {
                init();
            }

            virtual ~IpInterface()
            {

            }

            virtual void send(Frame frame)
            {

            }

            virtual bool receive( Frame& frame )
            {
                std::cout << "received frame..." << std::endl;
                return false;
            }

            virtual void init()
            {
#ifdef USE_UV
                _uvLoop = uv_default_loop();
#else
                _multicastRecv = new UDPReceiver( IP_COMM_PORT, _ifName, "225.0.0.37" );
                _multicastSend = new UDPTransmitter( IP_COMM_PORT, "225.0.0.37", _ifName );
#endif
            }

            virtual void destroy()
            {

            }

            virtual void run();

        private:
            std::string _ifName;
#ifdef USE_UV
            uv_loop_t* _uvLoop;
#else
            UDPReceiver *       _multicastRecv;
            UDPTransmitter *    _multicastSend;
#endif

        };
    }
}
