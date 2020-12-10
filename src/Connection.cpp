#include "Connection.hpp"


string get_local_ip_addr( b_ip::tcp::socket& sock ){
    // Get the local IP address of a socket 
    return boost::lexical_cast<std::string>(sock.remote_endpoint());
}

string get_local_ip_addr(){  return get_host_address();  }