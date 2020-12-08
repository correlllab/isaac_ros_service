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

using boost::asio::ip::tcp;
using std::queue;
using std::cout;
using std::endl;
using std::string;
using std::vector;
using std::set;
using std::max;

const int max_length = 1024;

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
}


/*************** class ServiceBridge_Server Declaration **************************************************************/

/***** ServiceBridge_Server *****/

class ServiceBridge_Server{

/***** Public *****/ public:

/*** Functions ***/
/* Setup */
bool /*-----*/ parse_service_file( string fullPath );
size_t /*---*/ how_many_services();
vector<string> get_served_ROS_topic_names();
bool /*-----*/ start();
bool /*-----*/ serve_loop();
/* Send / Receive */

~ServiceBridge_Server();
ServiceBridge_Server( string fullPath , size_t nThreadP = 2 );

/*** Vars ***/
set<string> /*---------*/ serviceNames;
size_t /*--------------*/ NthreadsPerTopic;
string /*--------------*/ ip;
size_t /*--------------*/ port;

/***** Protected *****/ protected:
vector<Service<XferType>> services;
vector<Connection> /*--*/ connections;

/*** END ***/ }; 


/*************** class ServiceBridge_Server Definition ***************************************************************/

ServiceBridge_Server::ServiceBridge_Server( string fullPath , size_t nThreadP ) : NthreadsPerTopic{ nThreadP } {
    // Set up server vars and parse the service file
    parse_service_file( fullPath );
}

ServiceBridge_Server::~ServiceBridge_Server(){
    // Free all pointers
}

bool ServiceBridge_Server::parse_service_file( string fullPath ){
    // Parse a service file, it should have 1 per service
    /*<string topic_name>,<size_t queue_size>,<size_t request_length>,<size_t response_length>*/
    const size_t minLines =  1;
    const size_t maxElems = 32;
    const size_t minAttrs =  4;

    vector<string> lines = readlines( fullPath );
    vector<string> servSpec;

    // 1. Read lines and split
    size_t fLen   = lines.size() ,
           qLen   = 0 ,
           reqLen = 0 ,
           rspLen = 0 ,
           maxLen = 0 ;
    string topicName;
    // 2. Parse the service, each should have 1 line
    /* <string topic_name>,<size_t queue_size>,<size_t request_length>,<size_t response_length> */
    if( fLen >= minLines ){
        for( size_t i = 0 ; i < fLen ; i++ ){
            
            servSpec = split_CSV_line( lines[i] );

            if( servSpec.size() < minAttrs ){
                cout << "ServiceBridge_Server::parse_service_file: Min number of attributes per topic is " << minAttrs 
                         << ", got: " << servSpec << " elements for " << topicName << endl;
                continue;
            }

            topicName = servSpec[0];
            qLen      = std::stoull( servSpec[1] );
            reqLen    = std::stoull( servSpec[2] );
            rspLen    = std::stoull( servSpec[3] );
            maxLen    = max( reqLen , rspLen  );

            if( !container_has( serviceNames , topicName ) ){  
                if( maxLen <= 32 ){
                    services.push_back( Service<XferType>( topicName , qLen ) );
                    serviceNames.insert( topicName );
                }else{
                    cout << "ServiceBridge_Server::parse_service_file: Max number of elements is " << maxElems 
                         << ", got: " << std::to_string( maxLen ) << " elements for " << topicName << endl;
                }
            }else{
                cout << "ServiceBridge_Server::parse_service_file: A service with name " << topicName << " already exists!";
            }
        }
    }else{
        cout << "ServiceBridge_Server::parse_service_file: File " << fullPath << " must have " << minLines << " or more lines!" << endl;
        return 0;
    }
    return 1;
}

size_t /*---*/ ServiceBridge_Server::how_many_services(){  return services.size();  }
vector<string> ServiceBridge_Server::get_served_ROS_topic_names(){  return convert_set_to_vector( serviceNames );  }


// FIXME: START HERE
/*
    [ ] Create threads
    [ ] Accept connection and set status
    [ ] session: Serve one connection
    [ ] run gracefully: How to `spin_once`?  Sleep, loop, and assume the other threads are running?
    [ ] exit gracefully: How to make the server close its connections
*/


// string getAddress( std::shared_ptr<boost::asio::io_service> ioService , boost::system::error_code& err ){
//     boost::asio::ip::tcp::resolver resolver( *ioService );
//     return resolver.resolve( boost::asio::ip::host_name() , err ) ->endpoint().address().to_string();
// }

// bool ServiceBridge_Server::start(){
//     // Start serving connections
    

//     // 1. For every connection, start the designated number of connections
//     for( auto srv_i : services ){
//         // 2. For each connection
//         for( size_t i = 0 ; i < NthreadsPerTopic ; i++ ){
//             srv_i.topicName;
//             std::shared_ptr<boost::asio::io_service> srv_(  new boost::asio::io_service()  );
//             string /*-----------------------------*/ ip_ = getAddress( srv_ , netError );
//             connections.push_back(
//                 // srv = std::make_shared<boost::asio::io_service>
//                 Connection(  
//                     srv_ ,
//                     ip_  ,
//                 )
//             )
//         }
//     }
// }

/*************** Main ************************************************************************************************/

int main( int argc , char* argv[] ){
    ServiceBridge_Server server( "/home/jwatson/isaac_ros_service/test/test01.srv" );
    cout << "There are " << server.how_many_services() << " services." << endl; 
    cout << "Will poll ROS at the following topics: " << server.get_served_ROS_topic_names() << endl;
    return 0;
}