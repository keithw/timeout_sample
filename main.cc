/* Goal for this program: send a datagram every given number of seconds,
                          AND print a message when a datagram is received */

/* by Minna Song and Keith Winstein */

#include <cstdlib>
#include <iostream>

#include "socket.hh"
#include "poller.hh"
#include "timestamp.hh"

using namespace std;
using namespace PollerShortNames;

/* arguments: LOCAL_PORT DEST_ADDRESS DEST_PORT INTERVAL */
int josh( const int argc, const char *argv[] )
{
  /* Truly paranoid check */
  if ( argc <= 0 ) {
    throw Exception( "josh", "Missing argv[ 0 ]" );
  }

  /* Check arguments */
  if ( argc != 5 ) {
    throw Exception( argv[ 0 ], "LOCAL_SERVICE DEST_ADDRESS DEST_SERVICE INTERVAL_MS" );
  }

  /* Get the arguments into our own variables */
  const string local_service = argv[ 1 ];
  const string dest_address = argv[ 2 ];
  const string dest_service = argv[ 3 ];
  const int interval_ms = myatoi( argv[ 4 ] );

  /* Make socket */
  Socket datagram_socket( UDP );

  /* Bind socket */
  datagram_socket.bind( Address( "0", local_service, UDP ) );

  /* Connect socket to destination */
  datagram_socket.connect( Address( dest_address, dest_service, UDP ) );

  /* Set up the events that we care about */
  Poller poller;

  /* Keep track of last time we sent an outgoing datagram */
  uint64_t last_datagram_sent_ms = timestamp();

  /* Things we care about, #1: getting a datagram */
  poller.add_action( Poller::Action( datagram_socket.fd(),
				     Direction::In,
				     [&] () {
				       const auto the_packet = datagram_socket.recvfrom();
				       cout << "Received datagram at time " << timestamp()
					    << " from " << the_packet.first.str() << endl;
				       return ResultType::Continue;
				     } ) );

  /* Loop */
  while ( true ) {
    /* Step 1: Are we due to send an outgoing packet right now? */
    const uint64_t now = timestamp();
    uint64_t next_packet_is_due = last_datagram_sent_ms + interval_ms;

    if ( now >= next_packet_is_due ) {
      /* Send a datagram */
      datagram_socket.write( "Hello from Minna and Josh." );
      last_datagram_sent_ms = now;
      next_packet_is_due = last_datagram_sent_ms + interval_ms;
      cout << "Sent datagram at time " << now << endl;
    }

    /* Wait for an incoming packet, but make sure to
       wake up before our next outgoing packet is due */
    const uint64_t timeout = next_packet_is_due - now;

    poller.poll( timeout );
  }

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
