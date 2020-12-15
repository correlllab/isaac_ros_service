#include "Connection.hpp"


string get_local_ip_addr( b_ip::tcp::socket& sock ){
    // Get the local IP address of a socket 
    return boost::lexical_cast<std::string>(sock.remote_endpoint());
}

string get_local_ip_addr(){  return get_host_address();  }


/*************** Connection ******************************************************************************************/

Connection::Connection( b_asio::io_service& io_service_ ) : 
/*Var Init*/ status{ CLOSED } , bytes_rcvd{0} , bytes_sent{0} , 
/*Obj Init*/ socket( io_service_ ) {}

bool Connection::accept( b_asio::io_service& io_service_ , tcp::acceptor& acceptor_  ){
    // 0. Create a server endpoint.
    b_ip::tcp::resolver /*--*/ resolver( io_service_ );
    b_ip::tcp::resolver::query query( ip , port );
    b_ip::tcp::endpoint /*--*/ endpoint = *resolver.resolve( query );
    // 1. Open the connection and set options
    acceptor_.open( endpoint.protocol() );
    acceptor_.set_option( b_ip::tcp::acceptor::reuse_address( true ) );
    // 2. Bind the connnection and listen for a client
    acceptor_.bind( endpoint );
    acceptor_.listen();
    status = WAITING;
    // 3. Block until there is data in the buffer
    try{ 
        acceptor_.accept( socket ); 
        status = OPEN;
        return 1;
    }catch(...){  
        status = CLOSED;
        return 0;  
    }
}

bool Connection::is_closed(){
    bool reachedEOF = netError == boost::asio::error::eof;
    if( reachedEOF ){  status = CLOSED;  }
    return reachedEOF;
}

bool Connection::try_read_bytes( size_t Nbytes , b_asio::mutable_buffer& outBuffer ){
    if( socket.by ) // FIXME: START HERE
}

void Connection::read( b_asio::mutable_buffer& outBuffer ){
    size_t amtRead = socket.read_some( buffer , netError );
    b_asio::buffer_copy( buffer , outBuffer );
}


void Connection::write( b_asio::mutable_buffer& inBuffer ){
    b_asio::buffer_copy( inBuffer , buffer );
    size_t amtRead = socket.write_some( buffer , netError );
}