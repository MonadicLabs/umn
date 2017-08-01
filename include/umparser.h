#pragma once

class UMParser
{
public:
    typedef enum
    {
        UMPARSER_WAIT_SYNC_0 = 0,
        UMPARSER_WAIT_SYNC_1,
        UMPARSER_WAIT_PTYPE
    } State;

    UMParser()
        :_curState(UMPARSER_WAIT_SYNC_0)
    {}

    virtual ~UMParser()
    {}

private:
    UMParser::State _curState;

};
