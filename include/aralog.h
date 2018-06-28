#pragma once

#include <string>
#include <iostream>

namespace umn
{

class ARARouter;
class ARALog : public std::ostream, std::streambuf
{
public:
    ARALog(ARARouter* ara = 0) : std::streambuf(), std::ostream(this), _parent(ara) { }
    std::streambuf::int_type overflow(std::streambuf::int_type c)
    {
        std::string id = getNodeId();
        std::cout << "ARA_" << id << " - " << c << std::endl;;
        // My custom operations
        return 0;
    }

    ARALog& operator = (const ARALog& other )
    {
        _parent = other._parent;
        return *this;
    }

    std::string getNodeId();

    ARARouter * _parent;

};



}

class println_buf : public std::streambuf
{
    std::string buffer;
public:
    println_buf()
    {

    }

    println_buf(const println_buf &) = delete;
    println_buf(println_buf &&);
    println_buf& operator=(const println_buf &) = delete;
    println_buf& operator=(println_buf &&);

    virtual std::streamsize xsputn(const char* s, std::streamsize n) override;
    virtual int overflow(int c) override;
    virtual int sync() override;
};
