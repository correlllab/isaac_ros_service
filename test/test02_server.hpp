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

using boost::shared_ptr;
using boost::make_shared;
// using boost::connection;

const int /*---*/ max_length = 1024;
extern const bool _DEBUG;


/*************** class ServiceBridge_Server Declaration **************************************************************/

/***** ServiceBridge_Server *****/

class ServiceBridge_Server{

/***** Public *****/ public:

/*** Functions ***/

/* Setup */
bool /*-----*/ parse_service_file( string fullPath );
size_t /*---*/ how_many_services();
vector<string> get_served_ROS_topic_names();
size_t /*---*/ init_server();

/* Send / Receive */
bool serve_loop( double spinHz = 100.0 );
bool accept_all();

/* Create / Destroy */
ServiceBridge_Server();
ServiceBridge_Server( string fullPath , size_t nThreadP = 2 , u_short port_ = 8000 );
~ServiceBridge_Server();


/*** Vars ***/

/* ASIO */ 
b_asio::io_service /*-*/ scheduler;
b_asio::io_service::work dispatcher;
tcp::acceptor /*------*/ receiver;
tcp::socket /*--------*/ sock;

/* Threading */ 
size_t /*-------------------------*/ NthreadsPerTopic ,
                                     MtotalThreads    ,
                                     numServices      ; 
// boost::thread_group /*------------*/ threadPool;
vector<boost::shared_ptr<boost::thread>> threads;

/* Networking */ 
set<string> /*---------------------------*/ serviceNames;
status_t /*------------------------------*/ status;
string /*--------------------------------*/ ip;
u_short /*-------------------------------*/ port;
vector<shared_ptr<ServiceQueue<xfer_type>>> services;
vector<Connection> /*--------------------*/ connections;




/*** END ***/ }; 

#endif