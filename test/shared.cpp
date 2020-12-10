// Demonstrate the proper use of shared pointers to avoid memory leaks

#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/move/move.hpp>
#include <iostream>
#include <unistd.h>
#include <boost/asio.hpp>

#include "../helpers/helper.hpp"

/*************** Types & Names ***************************************************************************************/

#define _DATA_LEN 32 
typedef boost::packaged_task<int> task_t;
typedef boost::shared_ptr<task_t> ptask_t;


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
    msg_t /*-----------------*/ type; 
    status_t /*--------------*/ status; 
    size_t /*----------------*/ sequence;
    boost::shared_ptr<double[]> data;
    size_t /*----------------*/ payload_size;
    /*** Functions ***/
    SrvMsg( msg_t typ , status_t stts , size_t seq , size_t N ) : type{typ} , status{stts} , sequence{seq} {
        payload_size = sizeof( double ) * N;
        data /*---*/ = shared_ptr<double[]>( new double[N] , array_deleter<double>() );
    };
};

SrvMsg get_msg( size_t N ){
    SrvMsg foo( REQUEST , EMPTY , 0 , N );
    for( size_t i = 0 ; i < N ; i++ ){  foo.data[i] = 5.0;  }
    return foo;
}


/*************** Main ************************************************************************************/

int main( int argc , char* argv[] ){

    size_t /*---------------*/ N   = 5;
    shared_ptr<double> /*---*/ foo( new double[N] , array_deleter<double>() );
    vector<shared_ptr<double>> bar;
    vector<SrvMsg> /*-------*/ baz;

    for( size_t i = 0 ; i < N ; i++ ){
        // cout << i << ": " << foo.get()[i] << ", " << *(foo.get())[i] << endl;
        cout << i << ": " << foo.get()+i << ", " << foo.get()[i] << endl;
        foo.get()[i] = 1.0 / (i+1);
        // bar.push_back( boost::move( foo ) ); // Causes leaks
        bar.push_back( foo );
        baz.push_back( get_msg( N ) );
        cout << "baz[" << i << "]:" << endl;
        for( size_t j = 0 ; j < N ; j++ ){
            cout << '\t' << baz[i].data[j] << endl;
        }
    }

    cout << endl 
         << "`foo` has " << foo.use_count() << " references and " << sizeof( foo.get() ) << "bytes" << endl
         << endl;

    for( size_t i = 0 ; i < N ; i++ ){
        // cout << i << ": " << foo.get()[i] << ", " << *(foo.get())[i] << endl;
        cout << i << ": " << foo.get()+i << ", " << foo.get()[i] << endl;
    }

    bar.clear();

    cout << endl 
         << "`foo` has " << foo.use_count() << " references." << endl
         << "`baz` has " << baz.size() << " elements." << endl
         << endl;

    foo.reset();
    baz.clear();

    cout << endl 
         << "`foo` has " << foo.use_count() << " references." << endl
         << endl;

    disconnect_stdout();
    return 0;
}