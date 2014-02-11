// C++11 (c) 2014 Vladimír Štill

#include "udptools.h"
#include <wibble/test.h>
#include <thread>
#include <cstring>
#include <unistd.h>

#include <signal.h>

static bool alarmed = false;

struct TestUdp {
    Test basic() {
        udp::Socket sock{};
    }

    Test reuse() {
        udp::Socket sock{ udp::Address{}, true }; // enable socket reuse
    }

    Test send() {
        udp::Address target{ udp::IPv4Address::localhost, udp::Port{ 64123 } };
        udp::Packet packet{ "Test", 5 };
        packet.address() = target;
        udp::Socket sock{};
        bool sent = sock.sendPacket(packet);
        assert( sent );
    }

    Test timeout() {
        udp::Socket sock{ udp::Address{ udp::IPv4Address::localhost, udp::Port{ 64123 } } };
        udp::Packet empty = sock.recvPacketWithTimeout( 300 );
        assert( empty.size() == 0 );
    }

    Test timeoutReset() {
        udp::Socket sock{ udp::Address{ udp::IPv4Address::localhost, udp::Port{ 64123 } } };
        udp::Packet empty = sock.recvPacketWithTimeout( 300 );

        // setup SIGALRM handler
        struct sigaction alarmAct;
        memset( &alarmAct, 0, sizeof( struct sigaction ) );
        alarmed = false;
        alarmAct.sa_handler = []( int sig ) {
            assert_eq( sig, SIGALRM );
            alarmed = true;
        };

        int rc = sigaction( SIGALRM, &alarmAct, nullptr );
        assert_eq( rc, 0 );

        // and setup alarm for 1 second
        alarm( 1 );

        assert( empty.size() == 0 );
        // now test that timeout was actually reset
        sock.recvPacket(); // this shoud block until iterrupted by alarm
        // make sure we were interrupted by alarm (after 1s) and not timeout
        // after 300 ms
        assert( alarmed && "recvPacket should block" );
    }

    Test sendRcv() {
        udp::Address sndAddr{ udp::IPv4Address::localhost, udp::Port{ 64124 } };
        udp::Address target{ udp::IPv4Address::localhost, udp::Port{ 64123 } };

        std::thread sender( [=]() {
                usleep( 400000 );
                udp::Packet packet{"Test",5};
                packet.address() = target;
                udp::Socket sock{ sndAddr };
                sock.sendPacket(packet);

                usleep( 100000 );
                udp::Packet second{ sizeof( long ) + sizeof( int ) };
                second.address() = target;
                second.get< long >() = 0x7700ff770077ff00L;
                second.get< int >( sizeof( long ) ) = 42;
                sock.sendPacket( second );
            } );


        alarm( 4 ); // get killed after 4 seconds -- just in case we deadlock (SIG 14)

        udp::Socket recv{ target };
        udp::Packet pck = recv.recvPacket();
        assert( pck.size() );
        assert_eq( std::strcmp( pck.data(), "Test" ), 0 );
        assert_eq( pck.address(), sndAddr );

        pck = recv.recvPacket();
        assert_eq( pck.size(), int( sizeof( int ) + sizeof( long ) ) );
        assert_eq( pck.get< long >(), 0x7700ff770077ff00L );
        assert_eq( pck.get< int >( sizeof( long ) ), 42 );
        assert_eq( pck.address(), sndAddr );

        sender.join();
    }
};
