#include "helper.hpp"

static bool _DEBUG = 1; // if( _DEBUG ){ cerr << "" << endl; }

string ltrim( const string& s ){
    // Return a version of the string with LEADING _WHITESPACE removed
    // https://www.techiedelight.com/trim-string-cpp-remove-leading-trailing-spaces/
    size_t start = s.find_first_not_of( _WHITESPACE );
    return (start == string::npos) ? "" : s.substr( start );
}
 
string rtrim( const string& s ){
    // Return a version of the string with TRAILING _WHITESPACE removed
    // https://www.techiedelight.com/trim-string-cpp-remove-leading-trailing-spaces/
    size_t end = s.find_last_not_of( _WHITESPACE );
    return ( end == string::npos ) ? "" : s.substr( 0 , end+1 );
}
 
string trim( const string& s ){
    // Return a version of the string with LEADING AND TRAILING _WHITESPACE removed
    // https://www.techiedelight.com/trim-string-cpp-remove-leading-trailing-spaces/
    return rtrim( ltrim( s ) );
}

vector<string> split_CSV_line( const string& line , string delimiter , bool stripWS ){
    // Return a vector of strings separated by commas
    // URL:  https://stackoverflow.com/questions/14265581/parse-split-a-string-in-c-using-string-delimiter-standard-c
    
    if( _DEBUG ){ cerr << "Entered split_CSV_line.." << endl; }

    size_t /*---*/ curr =  0;
    long int /*-*/ last = -1;
    long int /*-*/ len  = line.length();
    string /*---*/ token;
    vector<string> scanVec;
    vector<string> rtrnVec;

    // 1. Split the string into chunks according to the delimiter
    while(  ( curr = line.find( delimiter , last+1 ) )  !=  std::string::npos  ){
        token = line.substr( last+1 , curr-last-1 );
        scanVec.push_back( token );
        if( _DEBUG ){ cerr << "Token,1: " << token << endl; }
        last = curr;
    }
    if( last+1 < len-1 ){   
        token = line.substr( last+1 , len-1 );  
        if( _DEBUG ){ cerr << "Token,2: " << token << endl; }
        scanVec.push_back( token );
    }
    
    // 2. Strip _WHITESPACE and store non-WS strings
    for( auto item : scanVec ){
        token = trim( item );
        if( token.length() ){  rtrnVec.push_back( token );  }
    }

    if( _DEBUG ){ cerr << "Exited split_CSV_line" << endl; }

    // 3. Return the processed vector
    return rtrnVec;
}

bool file_exists( const string& fName ){ 
    // Return true if the file exists , otherwise return false
    struct stat buf; 
    if( stat( fName.c_str() , &buf ) != -1 ){ return true; } else { return false; }
}

vector<string> readlines( string path ){ 
    // Return all the lines of text file as a string vector
    vector<string> rtnVec;
    if( file_exists( path ) ){
        ifstream fin( path ); // Open the list file for reading
        string line; // String to store each line
        while ( getline( fin , line ) ){ // While we are able to fetch a line
            rtnVec.push_back( line ); // Add the file to the list to read
        }
        fin.close();
    } else { cout << "readlines: Could not open the file " << path << endl; }
    return rtnVec;
}

void disconnect_stdout(){
    // Disconnect from stdout to make valgrind happy
    fclose( stdin );
    fclose( stdout );
    fclose( stderr );
};

 
    