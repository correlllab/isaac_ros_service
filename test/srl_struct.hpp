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
using std::ostream;

using std::cout;
using std::endl;
using std::flush;

using std::to_string;

namespace b_asio = boost::asio;
namespace b_ip   = b_asio::ip;

using b_ip::tcp;


/*************** Utilities *******************************************************************************************/

tcp::endpoint fetch_endpoint( boost::asio::io_service& io_service_ , string ip , string port ){
    tcp::resolver /*--*/ resolver( io_service_ );
    tcp::resolver::query query( ip , port );
    return *resolver.resolve( query );
}

tcp::resolver::iterator get_endpoint_iterator( boost::asio::io_service& io_service_ , string ip , string port ){
    tcp::resolver /*--*/ resolver( io_service_ );
    tcp::resolver::query query( ip , port );
    return resolver.resolve( query );
}

template<typename T> 
ostream& operator<<( ostream& os , const vector<T>& vec ){// ostream '<<' operator for vectors
    // Print a generic `std::vector`
    // NOTE: This function assumes that the ostream '<<' operator for T has already been defined
    os << "[ ";
    for( size_t i = 0 ; i < vec.size() ; i++ ){
        os << vec[i];
        if( i+1 < vec.size() ){ os << ", "; }
    }
    os << " ]";
    return os; // You must return a reference to the stream!
}


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

/// Networking
b_ip::tcp::socket /*---*/ socket_;
boost::system::error_code err;

/// Data
vector<char> inbound_data_; /// Holds the inbound data.
string /*-*/ outbound_data_; /// Holds the outbound data.

/***** Methods *****/

Connection( b_asio::io_service& io_service_ ) : socket_( io_service_ ){} /// Constructor.

template <typename T>
bool write_one_object( const T& obj ){
    try{
        std::ostringstream /*------*/ archive_stream;
        boost::archive::text_oarchive archive( archive_stream );
        archive << obj;
        outbound_data_ = archive_stream.str();
        b_asio::write( socket_ , outbound_data_ );
        return 1;
    }catch( std::exception& e ){
        cout << "Failed to send!" << endl 
             << "Message: '" << outbound_data_ << "'" << endl;
        return 0;
    }
}

template <typename T>
bool read_one_object( T& target ){
    try{
        if( socket_.available() >= sizeof( T ) ){
            b_asio::read( socket_ , inbound_data_ );
            boost::archive::text_iarchive archive( inbound_data_ );
            archive >> target;
            return 1;
        }else{  return 0;  }
        
    }catch( std::exception& e ){
        return 0;
    }
}

template <typename T>
bool read_all_objects( vector<T>& target ){
    T /**/ temp;
    size_t count = 0;
    if( socket_.available() >= sizeof( T ) ){
        b_asio::read( socket_ , inbound_data_ );
        boost::archive::text_iarchive archive( inbound_data_ );
        while(1){
            try{
                archive >> temp;
                target.push_back( temp );
                count++;
            }catch( boost::archive::archive_exception const& e ){  break;  }
        }
        if( count > 0 ){  return 1;  }else{  return 0;  }
    }else{  return 0;  }
}

};

#endif