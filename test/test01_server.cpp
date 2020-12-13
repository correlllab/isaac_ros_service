#include "test01_server.hpp"


/*************** class ServiceBridge_Server Definition ***************************************************************/

ServiceBridge_Server::ServiceBridge_Server(){}

ServiceBridge_Server::ServiceBridge_Server( string fullPath , size_t nThreadP ) : NthreadsPerTopic{ nThreadP } {
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

size_t ServiceBridge_Server::init_server( u_short port_ ){
    // Get the server ready to serve connections

    if( _DEBUG ){  cout << "Enter init_server ... " << flush;  }

    // 1. Set up synchronization

    scheduler_ptr  = make_shared<b_asio::io_service>();
    if( _DEBUG ){  cout << "Created scheduler ..." << endl;  }

    dispatcher_ptr = make_shared<b_asio::io_service::work>( *scheduler_ptr );
    if( _DEBUG ){  cout << "Created dispatcher ..." << endl;  }

    if( _DEBUG ){  cout << "About to create the acceptor ... " << flush;  }
    porter_ptr     = make_shared<b_ip::tcp::acceptor>( *scheduler_ptr , b_ip::tcp::endpoint( b_ip::tcp::v4() , port ) );
    if( _DEBUG ){  cout << "Created accepter for port " << port << " ..." << endl;  }


    
    
    
    
    
    
    
    sock_ptr       = make_shared<b_ip::tcp::socket>( *scheduler_ptr );
    if( _DEBUG ){  cout << "Created socket ..." << endl;  }
    
    // 2. Set up the network protocol
    boost::system::error_code err = boost::system::error_code();
    if( _DEBUG ){  cout << "Created err ..." << endl;  }
    port = port_;
    // ip   = get_local_ip_addr( *sock_ptr );
    ip  = get_local_ip_addr();
    // if( _DEBUG ){  cout << "Fetched IP ..." << endl;  }

    // 3. Create threads
    numServices    = serviceNames.size();
    MtotalThreads  = NthreadsPerTopic * numServices;
    size_t CPU_lim = boost::thread::hardware_concurrency() * 2;      
    if( CPU_lim > 0 ){  MtotalThreads = min( CPU_lim , MtotalThreads );  }
    NthreadsPerTopic = (size_t) MtotalThreads / numServices;
    NthreadsPerTopic += (numServices > 1 ? MtotalThreads % numServices : 0);
    MtotalThreads = NthreadsPerTopic * numServices;
    if( _DEBUG ){  cout << "There will be " << MtotalThreads << " threads spread across" 
                        << numServices << " services with " << NthreadsPerTopic << "threads each." << endl;  }
    for( unsigned i = 0 ; i < MtotalThreads ; ++i ){
		threadPool.create_thread(
            boost::bind(
                &b_asio::io_service::run ,
			    scheduler_ptr
            )
        );
	}
    if( _DEBUG ){  
        cout << "Created " << threadPool.size() << " threads!" << endl;
        cout << "EXIT init_server !" << endl;  
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


// string getAddress( std::shared_ptr<boost::asio::io_service> ioService , boost::system::error_code& err ){
//     boost::asio::ip::tcp::resolver resolver( *ioService );
//     return resolver.resolve( boost::asio::ip::host_name() , err ) ->endpoint().address().to_string();
// }

// bool ServiceBridge_Server::start(){
//     // Start serving connections
    

//     // 1. For every connection, start the designated number of connections
//     for( auto srv_i : services ){
//         // 2. For each connection
//         for( size_t i = 0 ; i < NthreadsPerTopic ; i++ ){
//             srv_i.topicName;
//             std::shared_ptr<boost::asio::io_service> srv_(  new boost::asio::io_service()  );
//             string /*-----------------------------*/ ip_ = getAddress( srv_ , netError );
//             connections.push_back(
//                 // srv = std::make_shared<boost::asio::io_service>
//                 Connection(  
//                     srv_ ,
//                     ip_  ,
//                 )
//             )
//         }
//     }
// }

/*************** Main ************************************************************************************************/



int main( int argc , char* argv[] ){
    Counter counter = Counter();
    ServiceBridge_Server server( "/home/jwatson/isaac_ros_service/test/test01.srv" );
    cout << "Mark " << counter() << endl;
    cout << "There are " << server.how_many_services() << " services." << endl; 
    cout << "Will poll ROS at the following topics: " << server.get_served_ROS_topic_names() << endl;
    cout << "Mark " << counter() << endl;
    server.init_server( 8000 );
    cout << "Mark " << counter() << endl;
    cout << "Ready to connect at " << server.ip << ":" << server.port << endl;
    cout << "Mark " << counter() << endl;
    return 0;
}