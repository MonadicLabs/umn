#pragma once

#include <vector>
#include <string>
#include <map>
#include <thread>

#include "comminterface.h"
#include "node.h"
#include "shortid.h"
#include "frame.h"

namespace novadem
{
    namespace ip
    {

        typedef struct
        {
            int _ttl;
            std::string _nodeId;
        } IpNodeReference;

        class IpInterface: public CommInterface
        {
        public:
            IpInterface( const std::string& ifName )
                :_ifName(ifName)
            {
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


        protected:

        };
    }
}
