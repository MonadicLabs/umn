#pragma once

#include <vector>
#include <string>
#include <map>
#include <thread>

#include "comminterface.h"
#include "node.h"
#include "shortid.h"
#include "frame.h"

#include <uv.h>

namespace umn
{
    namespace ip
    {
        typedef struct
        {
            int _ttl;
            std::string _nodeId;
            uv_udp_t _udpRecv;
            uv_udp_t _udpSend;
        } IpNodeReference;

        class IpInterface: public CommInterface
        {
        public:
            IpInterface( const std::string& ifName )
                :_ifName(ifName)
            {
                init();

                // Look for all network interfaces
                if( !checkNetworkInterfaceExists(_ifName) )
                {
                    // ERROR
                }
            }

            virtual ~IpInterface()
            {

            }

            virtual void send(Frame frame)
            {

            }

            virtual bool receive( Frame& frame )
            {
                return false;
            }

            virtual void init()
            {
                _uvLoop = uv_default_loop();
            }

            virtual void destroy()
            {

            }

            virtual void run();

            void emitBeacon();
            void receiveBeacon();

        private:
            std::string _ifName;
            std::string _ownIpAddr;

            // maps ( node-IP / node-ID + ttl )
            std::map< std::string, IpNodeReference* > _nodeRefs;

            bool checkNetworkInterfaceExists(const std::string& ifName);
            void startEmittingBeacon()
            {
                _beaconEmissionThread = std::thread( &IpInterface::emitBeacon, this );
            }

            void startReceivingBeacon()
            {
                _beaconReceptionThread = std::thread( &IpInterface::receiveBeacon, this );
            }

            std::thread _beaconEmissionThread;
            std::thread _beaconReceptionThread;

            uv_loop_t* _uvLoop;
            static void node_recv_cb( uv_udp_t* handle, ssize_t nread, uv_buf_t, struct sockaddr* addr, unsigned flags );

        protected:

        };
    }
}
