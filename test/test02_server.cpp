#include "test01_server.hpp"


/*************** class ServiceBridge_Server Definition ***************************************************************/

const static size_t  DFLT_THREADS_PER_TOPIC =    2;
const static u_short DFLT_PORT /*--------*/ = 8000;

ServiceBridge_Server::ServiceBridge_Server() : 
/* Init Functions */ dispatcher( scheduler ) , receiver( scheduler , b_ip::tcp::endpoint( b_ip::tcp::v4() , DFLT_PORT ) ) , sock( scheduler ) ,  
/* Assign Vars */ NthreadsPerTopic{ DFLT_THREADS_PER_TOPIC } , port{ DFLT_PORT } {
    cout << "NOTE: " << "Default number of threads per topic: " << DFLT_THREADS_PER_TOPIC << endl
         << "NOTE: " << "Default port to serve on: __________ " << DFLT_PORT              << endl;
}

ServiceBridge_Server::ServiceBridge_Server( string fullPath , size_t nThreadP , u_short port_ ) : 
/* Init Functions */ dispatcher( scheduler ) , receiver( scheduler , b_ip::tcp::endpoint( b_ip::tcp::v4() , port_ ) ) , sock( scheduler ) ,  
/* Assign Vars */ NthreadsPerTopic{ nThreadP } , port{ port_ } {

    // Set up server vars and parse the service file
    parse_service_file( fullPath );
}

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
                    services.emplace_back( 
                        boost::make_shared<ServiceQueue<xfer_type>>(
                            topicName , qLen
                        )
                    );
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

size_t /*---*/ ServiceBridge_Server::how_many_services(){  return services.size();  }
vector<string> ServiceBridge_Server::get_served_ROS_topic_names(){  return convert_set_to_vector( serviceNames );  }

size_t ServiceBridge_Server::init_server(){
    // Get the server ready to serve connections

    if( _DEBUG ){  cout << "Enter init_server ... " << flush;  }
    
    // 2. Set up the network protocol
    ip  = get_local_ip_addr();
    if( _DEBUG ){  cout << "Fetched IP " << ip << " ..." << endl;  }

    // 3. Create threads
    numServices    = serviceNames.size();
    MtotalThreads  = NthreadsPerTopic * numServices;
    size_t CPU_lim = boost::thread::hardware_concurrency() * 2;      
    if( CPU_lim > 0 ){  MtotalThreads = min( CPU_lim , MtotalThreads );  }
    NthreadsPerTopic = (size_t) MtotalThreads / numServices;
    NthreadsPerTopic += (numServices > 1 ? MtotalThreads % numServices : 0);
    MtotalThreads = NthreadsPerTopic * numServices;
    if( _DEBUG ){  cout << "There will be " << MtotalThreads << " threads spread across " 
                        << numServices << " services with " << NthreadsPerTopic << " threads each." << endl;  }
    for( unsigned i = 0 ; i < MtotalThreads ; ++i ){
        if( _DEBUG ){  cout << "\tCreate thread " << i+1 << " of " << MtotalThreads << " ... " << flush;  }
		// NOTE: Threads are created 
        boost::shared_ptr<boost::thread> thread(
            new boost::thread(
                boost::bind(
                    &boost::asio::io_service::run , 
                    &scheduler
                )
            )
        );
        threads.push_back( thread );
        if( _DEBUG ){  cout << "Created!" << endl;  }
	}
    if( _DEBUG ){  
        cout << "Created " << threads.size() << " threads!" << endl;
        cout << "EXIT init_server!" << endl;  
    }
    return 0;
}


// FIXME: START HERE
/*
    [Y] Create threads
    [ ] Accept connection and set status
    [ ] session: Serve one connection
    [ ] run gracefully: How to `spin_once`?  Sleep, loop, and assume the other threads are running?
    [ ] exit gracefully: How to make the server close its connections
*/

bool ServiceBridge_Server::accept_all(){
    
    vector<string> topics = get_served_ROS_topic_names();

    // 1. For every topic
    for( size_t i = 0 ; i < numServices ; i++ ){
        // 2. For every thread serving that topic
        for( size_t j = 0 ; j < NthreadsPerTopic ; j++ ){
            // 3. 

        }
    }

    return 1;
}


bool ServiceBridge_Server::serve_loop( double spinHz ){
    status = RUNNING;
    while( status == RUNNING ){
        // 1. 
    }
    return 1;
}

/*************** Main ************************************************************************************************/



int main( int argc , char* argv[] ){
    ServiceBridge_Server server( "/home/jwatson/isaac_ros_service/test/test01.srv" );
    cout << "There are " << server.how_many_services() << " services." << endl; 
    cout << "Will poll ROS at the following topics: " << server.get_served_ROS_topic_names() << endl;
    server.init_server();
    cout << "Ready to connect at " << server.ip << ":" << server.port << endl;
    size_t len = server.threads.size();
    cout << "There are still " << len << " threads!" << endl;

    u_short dead = 0 , 
            i    = 1 ;
    for( auto& thread : server.threads ){
        cout << "\tThread " << i << " of " << len << " is " << flush;
        if( thread.get()->joinable() ){
            
            if( thread.get()->try_join_for( boost::chrono::nanoseconds(1) ) ){
                dead++;
                cout << "DEAD!" << endl;
            }else{
                cout << "ALIVE!" << endl;
            }
        }
        i++;
    }
    cout << "There are " << dead << " dead threads!" << endl;

    server.scheduler.stop();

    dead = 0 ; 
    i    = 1 ;
    for( auto& thread : server.threads ){
        cout << "\tThread " << i << " of " << len << " is " << flush;
        if( thread.get()->joinable() ){

            // thread.get()->terminate(); // DNE
            
            if( thread.get()->try_join_for( boost::chrono::nanoseconds(1) ) ){
                dead++;
                cout << "DEAD!" << endl;
            }else{
                cout << "ALIVE!" << endl;
            }
        }
        i++;
    }
    cout << "There are " << dead << " dead threads!" << endl;

    return 0;
}