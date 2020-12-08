#ifndef HELPER_H
#define HELPER_H

#include <string>
#include <iostream>
#include <vector>
#include <set>
#include <algorithm> 
#include <sys/stat.h> // File status
#include <fstream> // -- File I/O
#include <filesystem>
#include <limits>

#include <memory>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>


/*************** Names & Namespaces **********************************************************************************/

using std::vector;
using std::set;
using std::string;
using std::cout;
using std::cerr;
using std::endl;
using std::ifstream;
using std::getline;
using std::ostream;
using boost::shared_ptr;
using boost::make_shared;

const std::string _WHITESPACE = " \n\r\t\f\v";
const size_t      _BAD_INDEX  =  std::numeric_limits<size_t>::max();


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
    std::vector<T> v( s.begin() , s.end() );
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
}

#endif