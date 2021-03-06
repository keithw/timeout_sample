/* Goal for this program: send a datagram every given number of seconds,
                          AND print a message when a datagram is received */

/* by Minna Song and Keith Winstein */

#include <cstdlib>
#include <iostream>
#include <memory>
#include <limits>

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
  if ( (argc != 3) and (argc != 5) ) {
    throw Exception( argv[ 0 ], "INTERVAL_MS LOCAL_SERVICE [DEST_ADDRESS DEST_SERVICE]" );
  }

  bool automatic_target_mode = (argc == 3 );

  /* Get the arguments into our own variables */
  const int interval_ms = myatoi( argv[ 1 ] );
  const string local_service = argv[ 2 ];

  /* We have an optional destination address -- may not learn until we get our first packet */
  Address destination;

  if ( not automatic_target_mode ) {
    /* Look up destination hostname */
    destination = Address( argv[ 3 ], argv[ 4 ], UDP );
  }

  /* Make socket */
  Socket datagram_socket( UDP );

  /* Bind socket */
  datagram_socket.bind( Address( "0", local_service, UDP ) );

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

				       /* retarget the server to send packets to the most recent client
					  who sent it a packet */
				       if ( automatic_target_mode ) {
					 destination = the_packet.first;
				       }
				       return ResultType::Continue;
				     } ) );

  /* Loop */
  while ( true ) {
    uint64_t timeout = numeric_limits<int>::max();

    /* Question: Do we have a destination to send to? */
    if ( not( destination == Address() ) ) {
      /* Step 1: Are we due to send an outgoing packet right now? */
      const uint64_t now = timestamp();
      uint64_t next_packet_is_due = last_datagram_sent_ms + interval_ms;

      if ( now >= next_packet_is_due ) {
	/* Send a datagram */
	datagram_socket.sendto( destination, "Hello from Minna and Josh." );
	last_datagram_sent_ms = now;
	next_packet_is_due = last_datagram_sent_ms + interval_ms;
	cout << "Sent datagram at time " << now << " to " << destination.str() << endl;
      }

      /* Wait for an incoming packet, but make sure to
	 wake up before our next outgoing packet is due */
      timeout = next_packet_is_due - now;
    }

    /* Wait for an event, and run callback if one comes */
    auto poll_result = poller.poll( timeout );

    if ( poll_result.result == Poller::Result::Type::Exit ) {
      /* An action wanted to quit or they all stopped being interested in their events */
      cout << "Quitting after a file descriptor received an error." << endl;
      return poll_result.exit_status;
    }
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
