#pragma once

namespace umn
{

class Stream
{
public:
    Stream()
    {

    }

    virtual ~Stream()
    {

    }

    virtual size_t available()
    {
        return 0;
    }

    virtual size_t read( uint8_t* buffer, size_t buffer_size )
    {
        return 0;
    }

    virtual size_t write( uint8_t* buffer, size_t buffer_size )
    {
        return 0;
    }

    virtual bool is_pollable()
    {
        return false;
    }

    void * _ctx;

private:

protected:

};

}
