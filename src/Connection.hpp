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
#include <boost/lexical_cast.hpp>

#include "../helpers/helper.hpp"
#include "IP.hpp"

/*************** Types & Names ***************************************************************************************/

namespace b_asio = boost::asio;
namespace b_ip   = b_asio::ip;

using b_ip::tcp;


// typedef boost::shared_ptr<double[]> xfer_shr_ptr;
// typedef boost::array<double,32> xfer_type;
typedef vector<double> xfer_type; // NOTE: *Copied* structures!


typedef boost::packaged_task<int>   task_t;
typedef boost::shared_ptr<task_t>   ptask_t;


/*************** Utilities *******************************************************************************************/

string get_local_ip_addr( b_ip::tcp::socket& sock );
string get_local_ip_addr();


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
    msg_t     type; 
    status_t  status; 
    size_t    sequence;
    xfer_type data;
    size_t    payload_size;
    /*** Functions ***/
    SrvMsg( msg_t typ , status_t stts , size_t seq , size_t N ) : type{typ} , status{stts} , sequence{seq} {
        payload_size = sizeof( double ) * N;
        data /*---*/ = xfer_type();
    };
};


/*************** concurrent_queue ************************************************************************************/

template<typename T>
class concurrent_queue{
// a simple thread-safe multiple producer, multiple consumer queue (LOCKS)
// https://www.justsoftwaresolutions.co.uk/threading/implementing-a-thread-safe-queue-using-condition-variables.html

/***** Protected *****/ protected:

std::queue<T> /*-------*/ _queue; // - Underlying queue
mutable boost::mutex /*--------*/ _mutex; // - Lock
boost::condition_variable cond_var; // Semaphore

/***** Public *****/ public:

// concurrent_queue( boost::mutex& mtx , boost::condition_variable& cnd ) : _mutex{ mtc } , cond_var{cnd} {}

void push( T const& data ){
    // Lock the queue, push data, unlock, notify
    boost::mutex::scoped_lock lock(_mutex);
    _queue.push( data );
    lock.unlock();
    cond_var.notify_one();
}

bool empty() const{
    // Lock the queue, then return `true` if the queue is empty
    boost::mutex::scoped_lock lock(_mutex);
    return _queue.empty();
}

bool try_pop( T& popped_value ){
    /* returns true if there was a value to retrieve (in which case it retrieves it), or false to indicate that the queue was empty. */ 
    boost::mutex::scoped_lock lock( _mutex );
    if( _queue.empty() ){  return false;  }
    popped_value = _queue.front();
    _queue.pop();
    return true;
}

void wait_and_pop( T& popped_value ){
    // If multiple threads are popping entries from a full queue, then they just get serialized inside wait_and_pop, and everything works fine.
    boost::mutex::scoped_lock lock(_mutex);
    while( _queue.empty() ){  cond_var.wait( lock );  }
    popped_value=_queue.front();
    _queue.pop();
}

void clear(){
    T popped_value;
    while( !empty() ){  try_pop( popped_value );  }
}

};


/*************** Service *********************************************************************************************/

/***** ServiceQueue *****/

template<typename T>
struct ServiceQueue{
// Container class for running one service
/***** Public *****/ public:

/*** Vars ***/
string /*--------*/ topicName;
size_t /*--------*/ queueSize, 
                    seq_req  ,
                    seq_rsp  ;
concurrent_queue<T> Q_req;
concurrent_queue<T> Q_rsp;

/*** Functions ***/

SrvMsg req_pop_msg(){
    // Pop from requests and create message
    T    data;
    bool result = Q_req.try_pop( data );
    if( result ){
        return SrvMsg( REQUEST , VALID , seq_req++  , data );
    }else{
        return SrvMsg( REQUEST , EMPTY , _BAD_INDEX , data );
    }
};

SrvMsg rsp_pop_msg(){  
    // Pop from responses and create message
    T    data;
    bool result = Q_rsp.try_pop( data );
    if( result ){
        return SrvMsg( RESPONSE , VALID , seq_req++  , data );
    }else{
        return SrvMsg( RESPONSE , EMPTY , _BAD_INDEX , data );
    }
};

ServiceQueue( string tpcName , size_t qLen ) : topicName{ tpcName } , queueSize{ qLen }{  seq_req = seq_rsp = 0;  };
~ServiceQueue(){  Q_req.clear();  Q_rsp.clear();  }
};

/*************** Connection ******************************************************************************************/
// `Connection` handles messages for one client, and holds data related to that 

// https://gist.github.com/wush978/6190443
class Connection{

/***** Public *****/ public:

Connection(); // Begin with a closed connection and no bytes transferred

/*** Vars ***/
status_t /*------------*/ status;
size_t /*--------------*/ bytes_rcvd;
size_t /*--------------*/ bytes_sent;
boost::system::error_code netError;

};

#endif