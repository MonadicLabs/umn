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
            std::string _nodeIp;
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
            }

            virtual ~IpInterface()
            {

            }

            virtual void send(Frame frame)
            {
                // Send frame to all IP-network neighbors
            }

            virtual bool receive( Frame& frame )
            {
                std::cout << "received frame..." << std::endl;
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

            void emitBeaconTick();
            void ttlDecayTick();
            void processBeacon( std::string recvIp, const uv_buf_t* buf );

        private:
            std::string _ifName;
            std::string _ownIpAddr;

            // maps ( node-ID / node-IP + ttl )
            std::map< std::string, IpNodeReference* > _nodeRefs;

            bool findInterfaceAddress( const std::string& ifName, std::string& ifAddr );
            void startBeaconEmission();
            void startBeaconReception();
            void startTTLDecay();
            void startFrameReception();

            uv_loop_t* _uvLoop;
            uv_udp_t _beaconRecvSocket;
            uv_udp_t _frameRecvSocket;
            uv_timer_t _beaconEmissionTimer;
            uv_timer_t _ttlDecayTimer;

            static void onReceiveBeacon(uv_udp_t *req, ssize_t nread, const uv_buf_t *buf, const struct sockaddr *addr, unsigned flags);
            static void onSendBeacon(uv_timer_t* handle);
            static void onTTLDecay(uv_timer_t* handle);
            static void onReceiveFrame(uv_udp_t *req, ssize_t nread, const uv_buf_t *buf, const struct sockaddr *addr, unsigned flags);
            std::string bufToString(const uv_buf_t *buf);

            int _frameRecvPort;

        protected:

        };
    }
}
