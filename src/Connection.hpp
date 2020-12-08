#ifndef CONNECTION_H
#define CONNECTION_H

#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/move/move.hpp>
#include <iostream>
#include <unistd.h>
#include <boost/asio.hpp>

#include "../helpers/helper.hpp"

/*************** Types & Names ***************************************************************************************/

typedef boost::shared_ptr<double[]> xfer_shr_ptr;
typedef boost::packaged_task<int>   task_t;
typedef boost::shared_ptr<task_t>   ptask_t;


/*************** Structs and Enum ************************************************************************************/

/***** ID / Status Codes *****/

enum MsgType{ 
    REQUEST  ,
    RESPONSE ,
};
typedef enum MsgType msg_t;

enum StatusType{ 
    VALID   ,
    ERROR   ,
    EMPTY   ,
    OPEN    ,
    CLOSED  ,
    RUNNING ,
    STOPPED ,
};
typedef enum StatusType status_t;

/***** Service Message *****/

struct SrvMsg{
    /*** Vars ***/
    msg_t /*--*/ type; 
    status_t     status; 
    size_t /*-*/ sequence;
    xfer_shr_ptr data;
    size_t /*-*/ payload_size;
    /*** Functions ***/
    SrvMsg( msg_t typ , status_t stts , size_t seq , size_t N ) : type{typ} , status{stts} , sequence{seq} {
        payload_size = sizeof( double ) * N;
        data /*---*/ = xfer_shr_ptr( new double[N] , array_deleter<double>() );
    };
};


/*************** Service *********************************************************************************************/

/***** ServiceQueue *****/

template<typename T>
class ServiceQueue{
// Container class for running one service
/***** Public *****/ public:

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
    // Pop from requests
    if( Q_req.size() ){  
        target = Q_req.pop();  
        return 1;  
    }else{  
        return 0;  
    }  
};

bool rsp_pop( T& target ){  
    // Pop from responses
    if( Q_rsp.size() ){  
        target = Q_rsp.pop();  
        return 1;  
    }else{  
        return 0;  
    }  
};

SrvMsg<T> req_pop_msg(){
    // Pop from requests and create message
    T    data;
    bool result = req_pop( data );
    if( result ){
        return SrvMsg<T>( REQUEST , VALID , seq_req++  , data );
    }else{
        return SrvMsg<T>( REQUEST , EMPTY , _BAD_INDEX , data );
    }
};

SrvMsg<T> rsp_pop_msg(){  
    // Pop from responses and create message
    T    data;
    bool result = rsp_pop( data );
    if( result ){
        return SrvMsg<T>( RESPONSE , VALID , seq_req++  , data );
    }else{
        return SrvMsg<T>( RESPONSE , EMPTY , _BAD_INDEX , data );
    }
};

ServiceQueue( string tpcName , size_t qLen ) : topicName{ tpcName } , queueSize{ qLen }{  seq_req = seq_rsp = 0;  };
~ServiceQueue(){  clear_queue( Q_req );  clear_queue( Q_rsp );  }
};

/*
1. io_service   : Handles synchronization
2. work         : Class to inform the io_service when it has work to do.
2. thread_group : Handles threads
4. "session"    : User-defined class that does the work of the thread
*/

/*************** Connection ******************************************************************************************/
// `Connection` handles messages for one client, and holds data related to that 

// https://gist.github.com/wush978/6190443
class Connection{

/***** Public *****/ public:

/*** Vars ***/
status_t /*------------------------------*/ status;
size_t /*--------------------------------*/ bytes_rcvd;
size_t /*--------------------------------*/ bytes_sent;
boost::system::error_code /*-------------*/ netError;
std::shared_ptr<ServiceQueue<xfer_shr_ptr>> service;

// TODO: ADD NETWORK INFO VARS

};

#endif