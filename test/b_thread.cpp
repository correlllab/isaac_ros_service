// https://dens.website/tutorials/cpp-asio/multithreading
/*
To scale your Boost.Asio application on multiple threads you should do the following:
* Create one io_context object.
* Run io_context::run member function of that object on multiple threads.
* Realize which control flow branches operate on a shared data and therefore need to be synchronized, and which can go in parallel.
* Create io_context::strand object for every control flow branch which requires execution serialization.
* Wrap your completion handlers into corresponding strand objects with boost::asio::bind_executor function.
* If you need to execute some regular code within a some specific strand, post that code into the strand with boost::asio::post function.
*/

#include <boost/thread/thread.hpp>
#include <boost/lockfree/queue.hpp>
#include <boost/asio.hpp>
#include <iostream>
// #include <boost/atomic.hpp>

#include "../helpers/helper.hpp"

namespace b_asio = boost::asio;
namespace b_ip   = b_asio::ip;

using error_code = boost::system::error_code;


template<typename T>
class concurrent_queue{
// a simple thread-safe multiple producer, multiple consumer queue (LOCKS)
// https://www.justsoftwaresolutions.co.uk/threading/implementing-a-thread-safe-queue-using-condition-variables.html

/***** Protected *****/ protected:

std::queue<T> /*-------*/ _queue; // - Underlying queue
mutable boost::mutex /**/ _mutex; // - Lock
boost::condition_variable cond_var; // Semaphore

/***** Public *****/ public:

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

};

struct Payload{
    boost::shared_ptr<double[]> data;
    size_t /*----------------*/ dataLen;
};

// Prepare things
b_asio::io_service /*--------------*/ io_service;
std::vector<boost::thread> /*------*/ threads;
concurrent_queue<Payload> input;
concurrent_queue<Payload> output;
size_t /*--------------------------*/ count  = boost::thread::hardware_concurrency() * 2;
size_t /*--------------------------*/ active = 0;
size_t /*--------------------------*/ N      = 0;

const size_t DATA_LEN = 5;

/*  Shortly, there is no reason to pass by value, 
unless the goal is to share ownership of an object (eg. between different data structures, or between different threads). */



void populate_arr( Payload& elem , double val , size_t n = DATA_LEN ){
    for( u_char i = 0 ; i < DATA_LEN ; i++ ){  elem.data.get()[i] = val;  }
}

void report_arr( const Payload& elem , double val , size_t n = DATA_LEN ){
    cout << "[ ";
    for( u_char i = 0 ; i < DATA_LEN ; i++ ){  cout << elem.data.get()[i] << ", ";  }
    cout << "]";
}

Payload mult_arr( const Payload& elem , double factor , size_t n = DATA_LEN ){
    Payload rtnArr{ shared_ptr<double[]>( new double[DATA_LEN] , array_deleter<double>() ) , DATA_LEN };
    for( u_char i = 0 ; i < DATA_LEN ; i++ ){  rtnArr.data.get()[i] = elem.data.get()[i] * factor;  }
    return rtnArr;
}

struct Worker{
    // Read and process data
    
    /* Init */ 
    concurrent_queue<Payload>& in;
    concurrent_queue<Payload>& out;
    double factor;
    size_t N;
    Worker( double fctr , concurrent_queue<Payload>& in_ , concurrent_queue<Payload>& out_ ) : 
        factor{ fctr } , in{ in_ } , out{ out_ } {  N = 0;  };

    /* Work */ 

    void process_one(){
        Payload inVal;
        Payload outVal;

        if( !in.empty() ){
            in.wait_and_pop( inVal );
            outVal = mult_arr( inVal , 2.0 );
            out.push( outVal );
        }
    };

    void store_one(){
        // If there is one to send, then send it
    };
};

struct Sender{
    // Write and send data
};


int main(){

    N = count - active;
    for( size_t i = 0 ; i < N ; i++ ){
        active++;
        threads.emplace_back(
            [&]{  io_service.run();  }
        );
    }

    for( auto& thread : threads ){
        if( thread.joinable() ){
            active--;
            thread.join();
        }
    }

    return 0;
}