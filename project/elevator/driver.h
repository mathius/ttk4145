// C++11    (c) 2014 Vladimír Štill <xstill@fi.muni.cz>

/** the middle level API for elevator */

#include <tuple>
#include <cstdint>
#include <elevator/io.h>

#ifndef SRC_DRIVER_H
#define SRC_DRIVER_H

namespace elevator {

enum class ButtonType {
    CallUp = 0,
    CallDown = 1,
    TargetFloor = 2
};

struct Button {
    using Tuple = std::tuple< int, int >;
    Button() = default;
    Button( ButtonType type, int floor ) : _type( type ), _floor( floor ) { }
    Button( Tuple tuple ) : _type( ButtonType( std::get< 0 >( tuple ) ) ), _floor( std::get< 1 >( tuple ) ) { }

    Tuple tuple() const { return std::make_tuple( int( _type ), _floor ); }

    ButtonType type() { return _type; }
    int floor() { return _floor; }

  private:
    ButtonType _type;
    int _floor;
};

enum class Direction { None, Up, Down };

struct Driver {

    Driver();
    ~Driver();

    /* initialize the elevator -- disable all lights and run to lowest floor */
    void init();
    void shutdown();

    void setButtonLamp( Button button, bool state );
    void setStopLamp( bool state );
    void setDoorOpenLamp( bool state );
    void setFloorIndicator( int floor );

    bool getButtonLamp( Button button );
    bool getStopLamp();
    bool getDoorOpenLamp();
    int getFloorIndicator();
    bool getButtonSignal( Button button );

    void stopElevator();
    int getFloor();
    bool getStop();
    bool getObstruction();

    /* blocking move */
    void goToFloor( int );
    void goUpToFloor( int );
    void goDownToFloor( int );
    void goToBottom();
    void goToTop();

    int minFloor() const { return _minFloor; }
    int maxFloor() const { return _maxFloor; }
    Direction lastDirection() const { return _lastDirection; }
    int lastFloor() const { return _lastFloor; }
    bool moving() const { return _moving; }

    void setMotorSpeed( Direction, int );
    void movingOnFloor( int );

    bool alive();

  private:
    void _goTo( Direction, int );

    const int _minFloor;
    const int _maxFloor;
    Direction _lastDirection;
    int _lastFloor;
    bool _moving;
    lowlevel::IO _lio;
};

}

#endif // SRC_DRIVER_H