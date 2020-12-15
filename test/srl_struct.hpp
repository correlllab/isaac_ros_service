#ifndef SRL_STRUCT_H
#define SRL_STRUCT_H

#include <iostream>

#include <string>
#include <vector>

#include <boost/asio.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>

using std::string;
using std::vector;

using std::cout;
using std::endl;
using std::flush;

namespace b_asio = boost::asio;
namespace b_ip   = b_asio::ip;

using b_ip::tcp;


/*************** Datagram ********************************************************************************************/

struct Datagram{ 
    friend class boost::serialization::access; 

    static size_t  N;
    size_t /*---*/ seq;
    vector<double> data;

    Datagram(){
        Datagram::N++;
        seq = Datagram::N;
    }

    template <class Archive>
    void serialize( Archive &ar ) {
        ar &seq &data;  // Simply serialize the data members
    }    
};

/*************** Connection ******************************************************************************************/

struct Connection{

/***** Members *****/

/// The underlying socket.
boost::asio::ip::tcp::socket socket_;

/// The size of a fixed length header.
enum { header_length = 8 };

/// Data
vector<char> inbound_data_; /// Holds the inbound data.
string /*-*/ outbound_data_; /// Holds the outbound data.

/***** Methods *****/

Connection(boost::asio::io_service& io_service) : socket_(io_service){} /// Constructor.

boost::asio::ip::tcp::socket& socket(){  return socket_;  }  /// Get the underlying socket.

template <typename T>
bool write_one_object( const T& obj ){
    try{
        std::ostringstream /*------*/ archive_stream;
        boost::archive::text_oarchive archive( archive_stream );
        archive << obj;
        outbound_data_ = archive_stream.str();
        b_asio::write( socket_ , outbound_data_ );
        return 1;
    }catch ( std::exception& e ){

    }
}



};

#endif