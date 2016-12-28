#pragma once

#include "databroker.h"

#include <runnable.h>
#include <unistd.h>

namespace novadem
{
class DataComm : public novadem::Runnable
{
    friend class DataBroker;
public:
    DataComm()
        :_tickInterval(500)
    {}

    virtual ~DataComm(){}

    virtual void run()
    {
        while( !hasToStop() )
        {
            send();
            usleep( _tickInterval * 1000 );
        }
        setState(Runnable::STOPPED);
    }

    virtual void send() = 0;

private:
    unsigned long _tickInterval; // in millisec
    void setBroker( std::shared_ptr<DataBroker> broker )
    {
        _broker = broker;
    }

protected:
    std::shared_ptr< DataBroker > _broker;
};
}
