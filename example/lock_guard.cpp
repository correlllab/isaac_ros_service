// #include "stdafx.h"
#include <iostream>
#include <boost/thread.hpp>

void funct1();
void funct2();

boost::mutex zzzz;

int main()  // sample program using lock_guard
{	
    boost::thread xxxx(funct1);
    boost::thread yyyy(funct2);
    xxxx.join();
    yyyy.join();
    return 0;
}

void funct1()
{
    {  // artificially create a scope to contain the lock
    
       // the constructor for the wwww object
       // locks the mutex called zzzz
       boost::lock_guard<boost::mutex> wwww(zzzz);
       for (int i = 0; i < 50; i++) std::cout << '?';
       std::cout << std::endl;
    }  // the lock called wwww goes out of scope at
       //this point and the destructor unlocks the mutex zzzz
}   
    

void funct2()
{
    {  // artificially create a scope to contain the lock
    
       // the constructor for the vvvv object
       // locks the mutex called zzzz
       boost::lock_guard<boost::mutex> vvvv(zzzz);
       for (int i = 0; i < 50; i++) std::cout << '!';
       std::cout << std::endl;
    }  // the lock called vvvv goes out of scope at
       // this point and the destructor unlocks the mutex zzzz
}