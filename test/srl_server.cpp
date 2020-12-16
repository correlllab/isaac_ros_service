// #include <boost/asio.hpp>
// #include <boost/bind.hpp>
// #include <iostream>
// #include <vector>

#include <string>

#include "srl_struct.hpp" // Must come before boost/serialization headers.
// #include <boost/serialization/vector.hpp>


/*************** SerializationServer *********************************************************************************/

class SerializationServer{ public:


/*** Vars ***/

/* ASIO */ 
b_asio::io_service /*-*/ scheduler;
b_asio::io_service::work dispatcher;
tcp::acceptor /*------*/ receiver;
Connection /*---------*/ connection;
u_short /*------------*/ port = 8000;
string /*-------------*/ ip   = "127.0.0.1";


/*** Methods ***/

SerializationServer( u_short port_ ) : 
/* Init Functions */ dispatcher( scheduler ) , receiver( scheduler , b_ip::tcp::endpoint( b_ip::tcp::v4() , port_ ) ) , connection( scheduler ) {}

bool accept(){
    // 0. Create a server endpoint.
    b_ip::tcp::endpoint /*--*/ endpoint = fetch_endpoint( scheduler , ip , to_string( port ) );
    // 1. Open the connection and set options
    receiver.open( endpoint.protocol() , connection.err );
    receiver.set_option( b_ip::tcp::acceptor::reuse_address( true ) );
    // 2. Bind the connnection and listen for a client
    receiver.listen();
    try{
        receiver.accept( connection.socket_ , endpoint , connection.err ); // This will block until the connection is accepted
        cout << "Connection Result: " << connection.err.message() << endl;
    }catch( std::exception& e ){
        cout << "Encountered an error while accepting connection ...\n" << e.what() << endl;
    }
}

};


/*************** MAIN ************************************************************************************************/

int main(){

    return 0;
}