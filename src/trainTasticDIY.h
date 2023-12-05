#include <Arduino.h>

enum modes
{
    getOpcode,
    getMessageSize,
    receivingMessage,
} ;

enum throttleStates
{
    FREE,
    IN_USE,
} ;

const int SET_SPEED = 0b10000000 ;
const int SET_DIR   = 0b01000000 ;

// opcodes


const int OPC_HEARTBEAT     = 0x00 ;  // response/request

const int OPC_GET_INFO      = 0xF0 ;  // request
const int OPC_SEND_INFO     = 0xFF ;  // response 

const int OPC_GET_FEATURES  = 0xE0 ;  // request
const int OPC_SEND_FEATURES = 0xE4 ;  // response

const int OPC_GET_INPUT     = 0x12 ;  // request
const int OPC_SEND_INPUT    = 0x13 ;  // response

const int OPC_GET_OUTPUT    = 0x22 ;  // request
const int OPC_SET_OUTPUT    = 0x23 ;  // response

const int OPC_SET_SPEED     = 0x37 ;  // response/request: Set locomotive decoder speed and/or direction.
const int OPC_SET_FUNCTION  = 0x35 ;  // response/request: Enable/disable locomotive decoder function.
const int OPC_UNSUBSCRIBE   = 0x34 ;  // send: 

const int HAS_INPUTS        = 0b001 ;
const int HAS_OUTPUTS       = 0b010 ;
const int IS_THROTTLE       = 0b100 ; 
                                        // N.B. IS_DCC_CENTRAL? 

class TrainTasticDIY
{
public:
    TrainTasticDIY() ; // constructor
    void        begin(uint8_t VAL, char *description ) ;
    uint8_t     getMessage() ;
    void        sendByte( uint8_t b  ) ;

    void        transmitt( uint8_t* ) ;
    void        processMessage( ) ;

private:
    uint8_t     getChecksum() ;

    uint8_t     buf[16]; // 16 enough bytes for a message?
    uint8_t     mode = getOpcode ;
    uint8_t     byteCounter ;
    uint8_t     serialByte ;
    uint8_t     features ;
    uint8_t     messageLength ;
    bool        newByte ;
    uint8_t     attributes ;    // tells if I have inputs, ouputs or if if I am throttle.
    uint8_t     *ptr ;
} ;

extern TrainTasticDIY TrainTastic ;


class TrainTasticDIYThrottle
{
public:
    TrainTasticDIYThrottle() ;

    void        setSpeed(uint8_t);
    uint8_t     getSpeed();
    void        setDirection(uint8_t);
    uint8_t     getDirection(void);
    void        setFunction(uint8_t,uint8_t);
    uint8_t     getFunction();
    void        setAddress( uint16_t ) ;


private:
    uint8_t     direction ;
    uint8_t     speed ;
    uint16_t    id ;
    uint16_t    address ;
    uint8_t     throttleState ;
} ;


extern void notifyGetInput()    __attribute__ ((weak)) ;
extern void notifyGetOutput()   __attribute__ ((weak)) ;
extern void notifySetSpeed()    __attribute__ ((weak)) ;