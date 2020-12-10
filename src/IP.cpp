// https://www.tutorialspoint.com/how-to-get-the-ip-address-of-local-computer-using-c-cplusplus

#include "IP.hpp"

void check_host_name( int hostname ){ // This function returns host name for local computer
   if( hostname == -1 ){
      perror( "gethostname" );
      exit(1);
   }
}

void check_host_entry( struct hostent * hostentry ){ // find host info from host name
   if( hostentry == NULL ){
      perror( "gethostbyname" );
      exit(1);
   }
}

void IP_formatter( char *IPbuffer ){ // convert IP string to dotted decimal format
   if( NULL == IPbuffer ){
      perror( "inet_ntoa" );
      exit(1);
   }
}

std::string get_host_address(){ // Get the address of this host
    struct hostent* host_entry;
    char /*------*/ host[256];
    int /*-------*/ hostname;
    hostname = gethostname( host, sizeof( host ) ); //find the host name
    check_host_name( hostname );
    host_entry = gethostbyname( host ); //find host information
    check_host_entry( host_entry );
    return std::string( // Convert into IP string
        inet_ntoa(   
            *(  (struct in_addr*) host_entry->h_addr_list[0]  )   
        ) 
    ); 
}
