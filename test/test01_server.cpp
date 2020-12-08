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

using boost::asio::ip::tcp;
using std::queue;
using std::cout;
using std::endl;
using std::string;
using std::vector;
using std::set;
using std::max;

const int max_length = 1024;

typedef boost::array<double, 8> boostArrDbbl8;
typedef boost::array<double,16> boostArrDbbl16;
typedef boost::array<double,32> boostArrDbbl32; // Use this for now
typedef boost::array<double,64> boostArrDbbl64;

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


/*************** class Service Declaration ***************************************************************************/

/***** Status Codes *****/

enum MsgType{ 
    REQUEST  = 0 ,
    RESPONSE = 1 ,
};
typedef enum MsgType msg_t;

enum StatusType{ 
    VALID  = 0 ,
    ERROR  = 1 ,
    EMPTY  = 2 ,
    OPEN   = 3 ,
    CLOSED = 3 ,
};
typedef enum StatusType status_t;

/***** Service Message *****/

template<typename T>
struct SrvMsg{
    msg_t    type; 
    status_t status; 
    size_t   sequence;
    T        data;
};

/***** Service *****/

template<typename T>
class Service{
    // Container class for running one service
    public:

    /*** Vars ***/
    string   topicName;
    size_t   queueSize , 
             seq_req   ,
             seq_rsp   ;
    queue<T> Q_req;
    queue<T> Q_rsp;

    /*** Functions ***/

    bool   req_push( T  item   ){  if( Q_req.size() < queueSize ){  Q_req.push( item );  return 1;  }else{  return 0;  }  };
    bool   rsp_push( T  item   ){  if( Q_rsp.size() < queueSize ){  Q_rsp.push( item );  return 1;  }else{  return 0;  }  };
    size_t req_size(){  return Q_req.size();  };
    size_t rsp_size(){  return Q_rsp.size();  };

    bool req_pop( T& target ){  
        if( Q_req.size() ){  
            target = Q_req.pop();  
            return 1;  
        }else{  
            target = bad_boost_arr();
            return 0;  
        }  
    };

    bool rsp_pop( T& target ){  
        if( Q_rsp.size() ){  
            target = Q_rsp.pop();  
            return 1;  
        }else{  
            target = bad_boost_arr();
            return 0;  
        }  
    };

    SrvMsg<T> req_pop_msg(){  
        T    data;
        bool result = req_pop( data );
        if( result ){
            return SrvMsg<T>( REQUEST , VALID , seq_req++  , data );
        }else{
            return SrvMsg<T>( REQUEST , EMPTY , _BAD_INDEX , data );
        }
    };

    SrvMsg<T> rsp_pop_msg(){  
        T    data;
        bool result = rsp_pop( data );
        if( result ){
            return SrvMsg<T>( RESPONSE , VALID , seq_req++  , data );
        }else{
            return SrvMsg<T>( RESPONSE , EMPTY , _BAD_INDEX , data );
        }
    };

    Service( string tpcName , size_t qLen ) : topicName{ tpcName } , queueSize{ qLen }{  seq_req = seq_rsp = 0;  };
};


/*************** class ServiceBridge_Server Declaration **************************************************************/

/***** Connection *****/
// https://gist.github.com/wush978/6190443
struct Connection{
    boost::asio::io_service /*--*/ ios;
    // boost::asio::ip::tcp::resolver resolver;
    string /*-------------------*/ ip;
    size_t /*-------------------*/ port;
    size_t /*-------------------*/ bytes_rcvd;
    size_t /*-------------------*/ bytes_sent;
    status_t /*-----------------*/ status;
}

/***** ServiceBridge_Server *****/

class ServiceBridge_Server{

/***** Public *****/ public:

/*** Functions ***/
/* Setup */
bool /*-----*/ parse_service_file( string fullPath );
size_t /*---*/ how_many_services();
vector<string> get_served_ROS_topic_names();
bool /*-----*/ serve(  );
/* Send / Receive */

~ServiceBridge_Server();

/*** Vars ***/
boost::system::error_code netError;
set<string> /*---------*/ serviceNames;

/***** Protected *****/ protected:
vector< Service<boostArrDbbl32> > services;

/*** END ***/ }; 


/*************** class ServiceBridge_Server Definition ***************************************************************/

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
                    services.push_back( Service<boostArrDbbl32>( topicName , qLen ) );
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