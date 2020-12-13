#ifndef HELPER_H
#define HELPER_H

#include <string>
#include <iostream>
#include <vector>
#include <set>
#include <queue>
#include <algorithm> 
#include <sys/stat.h> // File status
#include <fstream> // -- File I/O
// #include <filesystem>
#include <limits>
#include <cmath>

#include <boost/array.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>


/*************** Names & Namespaces **********************************************************************************/

using std::vector;
using std::set;
using std::queue;
using std::string;
using std::size_t;

using std::cout;
using std::cerr;
using std::endl;
using std::flush;

using std::ifstream;
using std::getline;
using std::ostream;

using std::min;

using boost::shared_ptr;

const  std::string _WHITESPACE = " \n\r\t\f\v";
const  size_t      _BAD_INDEX  =  std::numeric_limits<size_t>::max();
const  bool        _DEBUG      = 1;

/*************** Structs and Functors ********************************************************************************/

struct Counter{
    size_t count;
    Counter(){  count = 0; };
    size_t operator()(){  return count;  };
};


/*************** Utilities , Typed ***********************************************************************************/

string ltrim( const string& s ); // Return a version of the string with LEADING _WHITESPACE removed
string rtrim( const string& s ); // Return a version of the string with TRAILING _WHITESPACE removed
string  trim( const string& s ); // Return a version of the string with LEADING AND TRAILING _WHITESPACE removed

vector<string> split_CSV_line( const string& line , string delimiter = "," , bool stripWS = 1 ); // Return a vector of strings separated by commas

vector<string> readlines( string path ); // Return all the lines of text file as a string vector

template<typename Container, typename T>
bool container_has( Container container , T element ){  return container.find( element ) != container.end();  } 

void disconnect_stdout(); // Disconnect from stdout to make valgrind happy


/*************** Utilities , Templated *******************************************************************************/

template<typename T> 
ostream& operator<<( ostream& os , const vector<T>& vec ){// ostream '<<' operator for vectors
    // Print a generic `std::vector`
    // NOTE: This function assumes that the ostream '<<' operator for T has already been defined
    os << "[ ";
    for( size_t i = 0 ; i < vec.size() ; i++ ){
        os << vec[i];
        if( i+1 < vec.size() ){ os << ", "; }
    }
    os << " ]";
    return os; // You must return a reference to the stream!
}

template<typename T>
std::vector<T> convert_set_to_vector( const set<T>& s ){
    // Return a vector that has all the elements in the set
    if( _DEBUG )  cout << "Convert begin ... ";
    std::vector<T> v( s.begin() , s.end() );
    if( _DEBUG )  cout << "END.  Got" << v.size() << " elements";
    return v;
}

template< typename T >  
struct array_deleter{  void operator ()( T const * p ){  delete[] p;  }  };  // Deletion functor to pass to `boost::shared/unique_ptr`

template< typename T >  
void clear_queue( queue<T>& q ){
    // Erase the queue and all members
    // URL:  https://stackoverflow.com/a/709161
    queue<T> empty;
    std::swap( q , empty );
};

template<size_t N> inline
boost::array<double,N> bad_boost_arr(){
    boost::array<double,N> rtnArr;
    for( size_t i = 0 ; i < N ; i++){  rtnArr[i] = std::nan("");  }
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

#endif
