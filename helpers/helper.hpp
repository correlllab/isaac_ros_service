#ifndef HELPER_H
#define HELPER_H

#include <string>
#include <iostream>
#include <vector>
#include <algorithm> 
#include <sys/stat.h> // File status
#include <fstream> // -- File I/O
#include <filesystem>
#include <limits>

using std::vector;
using std::string;
using std::cout;
using std::endl;
using std::ifstream;
using std::getline;

const std::string _WHITESPACE = " \n\r\t\f\v";
const size_t      _BAD_INDEX  =  std::numeric_limits<size_t>::max();

string ltrim( const string& s ); // Return a version of the string with LEADING _WHITESPACE removed
string rtrim( const string& s ); // Return a version of the string with TRAILING _WHITESPACE removed
string  trim( const string& s ); // Return a version of the string with LEADING AND TRAILING _WHITESPACE removed

vector<string> split_CSV_line( const string& line , bool stripWS = 1 ); // Return a vector of strings separated by commas

vector<string> readlines( string path ); // Return all the lines of text file as a string vector

template<typename Container, typename T>
bool container_has( Container container , T element ){  return container.find( element ) != container.end();  } 

#endif
