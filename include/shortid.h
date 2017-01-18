#pragma once

#include <iostream>
#include <chrono>
#include <cstring>
#include <random>

using namespace std;

namespace umn
{
class ShortId
{
    static const size_t ShortIdLength = 16;

public:
    ShortId( const std::string& fromString = "" )
    {
        // cout << "cons" << endl;
        _data = new char[ ShortIdLength ];
        if( fromString.size() <= 0 )
            generate();
        else
            copyFromString( fromString );
    }

    virtual ~ShortId(){
        delete[] _data;
    }

    ShortId( const ShortId& other )
    {
        // cout << "copycons from " << other.toString() << endl;
        _data = new char[ ShortIdLength ];
        // cout << "_data=" << (unsigned long)_data << " other._data=" << (unsigned long)other._data << endl;
        *this = other;
    }

    ShortId& operator =( const ShortId& other )
    {
        memcpy( (void*)_data, (void*)other._data, ShortIdLength );
        return *this;
    }

    bool operator !=( const ShortId& other )
    {
        bool ret = false;
        for( size_t k = 0; k < ShortIdLength; ++k )
        {
            if( _data[k] != other._data[k] )
            {
                ret = true;
                break;
            }
        }
        return ret;
    }

    bool operator ==(const ShortId& other )
    {
        return !(*this != other);
    }

    bool operator <( const ShortId& other ) const
    {
        return std::lexicographical_compare( _data, _data + ShortIdLength, other._data, other._data + ShortIdLength );
    }

    std::string toString() const
    {
        string ret = "";
        for( size_t k = 0; k < 16; ++k )
        {
            if( k > 0 && k % 4 == 0 )
                ret += "-";
            ret += _data[k];
        }
        return ret;
    }

private:
    char * _data;

    void generate(){
        std::tm tm;
        strptime("1986-09-26 11:11", "%Y-%m-%d %H:%M", &tm);
        auto tp = std::chrono::high_resolution_clock::from_time_t(std::mktime(&tm));
        auto end_time = chrono::high_resolution_clock::now();
        unsigned long long tt = chrono::duration_cast<chrono::microseconds>(end_time - tp).count();
        std::random_device rd;
        std::default_random_engine e1(rd());
        std::uniform_int_distribution<int> uniform_dist(1, 46656);
        std::string sts = toBase36( tt );
        std::string ssh = toBase36( 135 );
        std::string svs = toBase36( 1 );
        std::string srd = toBase36( uniform_dist(e1) );

        for( size_t k = 0; k < sts.size(); ++k )
            _data[ k ] = sts[k];

        for( size_t k = 0; k < ssh.size(); ++k )
            _data[ 10 + k ] = ssh[k];

        for( size_t k = 0; k < svs.size(); ++k )
            _data[ 12 + k ] = svs[k];

        for( size_t k = 0; k < srd.size(); ++k )
            _data[ 13 + k ] = srd[k];
    }

    std::string toBase36( unsigned long long n )
    {
        // std::cout << "n=" << n << std::endl;
        char ch[]={'0','1','2','3','4','5','6','7','8','9',
                   'A','B','C','D','E','F','G','H','I','J',
                   'K','L','M','N','O','P','Q','R','S','T',
                   'U','V','W','X','Y','Z'
                  };

        std::string ret = "";
        unsigned long long  r, i = 0;
        while( n > 0)
        {
            r = n % 36;
            ret += ch[ r ];
            // std::cout << "r=" << r << endl;
            i++;
            n = n / 36;
        }

        // std::cout << "base36:" << ret << std::endl;
        return ret;
    }

    void copyFromString( const std::string& fromString )
    {
        if( fromString.size() != (ShortIdLength + 3) )
        {
            throw std::exception();
            return;
        }

        for( size_t k = 0, i = 0; k < fromString.size(); ++k )
        {
            if( fromString[k] != '-' )
            {
                _data[i] = fromString[k];
                ++i;
            }
        }
    }

protected:
};
}
