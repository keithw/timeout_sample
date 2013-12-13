/* Goal for this program: send a datagram every given number of seconds,
                          AND print a message when a datagram is received */

/* by Minna Song and Keith Winstein */

#include <cstdlib>
#include <iostream>

#include "socket.hh"
#include "poller.hh"

using namespace std;

/* arguments: LOCAL_PORT DEST_ADDRESS DEST_PORT INTERVAL */
int josh( const int argc, const char *argv[] )
{
  /* Truly paranoid check */
  if ( argc <= 0 ) {
    throw Exception( "josh", "Missing argv[ 0 ]" );
  }

  /* Check arguments */
  if ( argc != 5 ) {
    throw Exception( argv[ 0 ], "LOCAL_SERVICE DEST_ADDRESS DEST_SERVICE INTERVAL" );
  }

  /* Get the arguments into our own variables */
  const string local_service = argv[ 1 ];
  const string dest_address = argv[ 2 ];
  const string dest_service = argv[ 3 ];
  //  const int num_seconds = myatoi( argv[ 4 ] );

  /* Make socket */
  Socket datagram_socket( UDP );

  /* Bind socket */
  datagram_socket.bind( Address( "0", local_service, UDP ) );

  /* Set up the events that we care about */
  Poller poller;

  

  return EXIT_SUCCESS;
}

int main( const int argc, const char *argv[] )
{
  try {
    return josh( argc, argv );
  } catch ( const Exception & e ) {
    e.perror();
    return EXIT_FAILURE;
  }
}
