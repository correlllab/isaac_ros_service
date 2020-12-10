// https://www.tutorialspoint.com/how-to-get-the-ip-address-of-local-computer-using-c-cplusplus

#ifndef IP_H
#define IP_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>

void check_host_name( int hostname ); // -------------- This function returns host name for local computer
void check_host_entry( struct hostent * hostentry ); // Find host info from host name
void IP_formatter( char *IPbuffer ); // --------------- Convert IP string to dotted decimal format
std::string get_host_address(); // -------------------- Get the address of this host

#endif