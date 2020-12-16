// #include <boost/serialization/vector.hpp>
// #include <boost/archive/text_oarchive.hpp>
// #include <boost/archive/text_iarchive.hpp>

#include "srl_struct.hpp"


/*************** SerializationClient *********************************************************************************/

class SerializationClient{ public:

/*** Vars ***/

/* ASIO */ 
b_asio::io_service /*-*/ scheduler;
b_asio::io_service::work dispatcher;
tcp::acceptor /*------*/ receiver;
Connection /*---------*/ connection;
u_short /*------------*/ port = 8000;
string /*-------------*/ ip   = "127.0.0.1";


/*** Methods ***/

SerializationClient( u_short port_ ) : 
/* Init Functions */ dispatcher( scheduler ) , receiver( scheduler , b_ip::tcp::endpoint( b_ip::tcp::v4() , port_ ) ) , connection( scheduler ) {}

bool connect(){
    // 0. Create a client endpoint.
    tcp::resolver::iterator endpoint_iter = get_endpoint_iterator( scheduler , ip , to_string( port ) );
    tcp::resolver::iterator end;
    connection.err = boost::asio::error::host_not_found;
    // 1. Connect
    while ( connection.err && endpoint_iter != end){
        connection.socket_.close();
        connection.socket_.connect( *endpoint_iter++ , connection.err );
    }
    cout << "Connection Result: " << connection.err.message() << endl;
}

};


/*************** MAIN ************************************************************************************************/

int main(){

    return 0;
}
