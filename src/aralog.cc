
#include "aralog.h"
#include "ararouter.h"
#include "node.h"

#include <sstream>
using namespace std;

std::string umn::ARALog::getNodeId()
{
    std::stringstream sstr;
    sstr << std::dec << _parent->node()->address().asInteger();
    return sstr.str();
}

println_buf::println_buf(println_buf &&o) : buffer(std::move(o.buffer))
{

}

println_buf& println_buf::operator=(println_buf &&o)
{
    buffer = std::move(o.buffer);
    return *this;
}

streamsize println_buf::xsputn(const char* s, streamsize n)
{
    streamsize start = 0;
    for (std::streamsize i = 0; i < n; i++)
    {
        if (s[i] == '\n')
        {
            buffer.append(s + start, s + i);
            sync();
            start = i + 1;
        }
    }
    buffer.append(s + start, s + n);
    return n;
}

int println_buf::overflow(int c)
{
    char ch = traits_type::to_char_type(c);
    if (ch == '\n')
    {
        sync();
    } else {
        buffer.append(1, c);
    }
    return c;
}

int println_buf::sync()
{
    cout << "pol: " << buffer;
    buffer.clear();
    return 0;
}
