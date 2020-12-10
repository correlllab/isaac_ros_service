#ifndef TSTSVR1_H
#define TSTSVR1_H

/* ### NOTES ###

*/

#include <cstdlib>
#include <iostream>
#include <thread>
#include <utility>
#include <boost/asio.hpp>
#include <queue>
#include <string>
#include <vector>
#include <memory>
#include <set>
#include <algorithm>

#include <boost/array.hpp>

#include "../helpers/helper.hpp"
#include "../src/Connection.hpp"

using std::queue;
using std::cout;
using std::endl;
using std::string;
using std::vector;
using std::set;
using std::max;

const int /*---*/ max_length = 1024;
extern const bool _DEBUG;

/*************** Utilities *******************************************************************************************/

template<size_t N> inline
boost::array<double,N> bad_boost_arr(){
    boost::array<double,N> rtnArr;
    for( size_t i = 0 ; i < N ; i++){  rtnArr[i] = nan("");  }
    return rtnArr;
}

template<typename T , size_t N> inline
bool is_nan( const boost::array<T,N>& arr ){
    if( N == 0 )  
        return true;
    else if(  is_nan( arr[0] )  )  
        return true;
    else
        return false;   
}

template<typename T>
void erase( vector<std::unique_ptr<T>>& vec ){
    // Erase a vector of unique pointers
    for( auto& pointer : vec ){
        if( pointer ){
            delete pointer;
            pointer = nullptr;
        }
    }
    vec.erase( 
        std::remove( vec.begin() , vec.end() , nullptr ) , 
        vec.end() 
    );
};


/*************** class ServiceBridge_Server Declaration **************************************************************/

/***** ServiceBridge_Server *****/

class ServiceBridge_Server{

/***** Public *****/ public:

/*** Functions ***/

/* Setup */
bool /*-----*/ parse_service_file( string fullPath );
size_t /*---*/ how_many_services();
vector<string> get_served_ROS_topic_names();
size_t /*---*/ init_server( u_short port_ );

/* Send / Receive */
bool serve_loop( double spinHz = 100.0 );
bool accept_all();

/* Create / Destroy */
ServiceBridge_Server();
ServiceBridge_Server( string fullPath , size_t nThreadP = 2 );
~ServiceBridge_Server();


/*** Vars ***/

/* Networking */ 
set<string> /*--------------------------*/ serviceNames;
status_t /*-----------------------------*/ status;
string /*-------------------------------*/ ip;
u_short /*------------------------------*/ port;
vector<ServiceQueue<shared_ptr<double[]>>> services;
vector<Connection> /*-------------------*/ connections;
shared_ptr<tcp::socket> /*--------------*/ sock_ptr;
shared_ptr<tcp::acceptor> /*------------*/ porter_ptr;

/* Threading */ 
size_t /*-------------------------*/ NthreadsPerTopic ,
                                     MtotalThreads    ,
                                     numServices      ; 
boost::thread_group /*------------*/ threadPool;
shared_ptr<b_asio::io_service> /*-*/ scheduler_ptr;
shared_ptr<b_asio::io_service::work> dispatcher_ptr;



/*** END ***/ }; 

#endif