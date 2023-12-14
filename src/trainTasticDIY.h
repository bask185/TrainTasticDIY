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

// OPCODES


const int OPC_HEARTBEAT     = 0x00 ;  // response/request

const int OPC_GET_INFO      = 0xF0 ;  // request
const int OPC_SEND_INFO     = 0xFF ;  // response 

const int OPC_GET_FEATURES  = 0xE0 ;  // request
const int OPC_SEND_FEATURES = 0xE4 ;  // response

const int OPC_GET_INPUT     = 0x12 ;  // request  bask185 NOTE: inputs are fun and all, but there should be a destinction in point/signal/accessory commands, occupancy broadcasts, 'just' switches and RAILCOM data
const int OPC_SEND_INPUT    = 0x13 ;  // response

const int OPC_GET_OUTPUT    = 0x22 ;  // request
const int OPC_SET_OUTPUT    = 0x23 ;  // response

const int OPC_SET_SPEED     = 0x37 ;  // response/request: Set locomotive decoder speed and/or direction.
const int OPC_SET_FUNCTION  = 0x35 ;  // response/request: Enable/disable locomotive decoder function.
const int OPC_UNSUBSCRIBE   = 0x34 ;  // send:

// bask185 future ideas: 

// Tokens for throttles

// The server may send a token to a throttle. The #1 goal is to display the token on a display of the receiving device
// Optionally a token should be mandatory for a throttle in order to take control over a train.
// goal #2 is to achieve interlocking. So the receiving device need to send a response to confirm token reception.
// this must work both ways. When the throttle returns the token, the server must acknowledge this
// The ACK respons should perhaps contain the throttle's ID. TT can track which throttle is in posession of which token.

// Optionally. #3 goal. A token exchange should happen at so called: token exchange points. One such a point has an occupancy sensor, preferebly railcom.
// The train should be physically on the expected occupancy sensor before the throttle may return the token.
// TT should know which occopancy sensor that is. 

// The token message from the central should also contains the loco's DCC address. After ACK signal, the throttle may control that address.

// The token themselfes can be linked to virtual occupancy sensors. If a token for a section is dispatched to a throttle, the entire section should be reported as occupied.
const int OPC_SEND_TOKEN = 0x4F ; // message: sends a token 
// format <OPC>, <SIZE>, <THROTTLE_ID_HB>, <THROTTLE_ID_LB>, <LOCO_ADDRESS_HB>, <LOCO_ADDRESS_LB>, <"TOKEN NAME">
const int OPC_REQ_TOKEN = 0x40 ; // response ACK token
// or  const int OPC_REQUEST_TOKEN = 0x42 ; // respons ACK + throttle ID

// DISPATCH AN ADDRESS
// server must prepare an address for dispatching. 
// when a throttle requests an address, the server respnds with the address. The throttle may only control this address untill the server says otherwise.
// The throttle needs to send his ID. In the event 2 throttles request an address, the address can only be dispatched to one of the two throttles.
// if server sends a token, no throttle ID must be sent. So 0x50 as OPC can be used
// if throttle sends a token, throttle ID must be filled

const int OPC_REQ_ADDR = 0x52 ;      // This throttle wants an address, it sends it's ID with it
// <OPC> <THROTTLE_ID_HB> <THROTTLE_ID_LB>
const int OPC_DISPATCH_ADDR = 0x54 ; // respons of server with throttle ID and Loco address.
// <OPC> <THROTTLE_ID_HB> <THROTTLE_ID_LB> <LOCO_ADDRESS_HB>, <LOCO_ADDRESS_LB> <XOR>

// RAILCOM MESSAGES.
// Special Occupancy message, with 2 byte sensor address, 2 byte address and 1 byte loco's speed 
const int OPC_SEND_RC_DATA = 0x65 ;
// <OPC> <SENSOR ADDR HB> <SENSOR ADDR LB> <LOCO_ADDRESS_HB>, <LOCO_ADDRESS_LB> <LOCO_SPEED> <XOR>

// TIME/DATE BROADCAST
// server may issue the time and or date once a second or somethign..idk. This can be used for station clocks, throttle clocks (timetable?)
// and perhaps
const int OPC_SEND_TIME_AND_DATE = 0x77 ;
//<OPC> <seconds> <minutes> <hours> <day> <month> <year_HB> <year_LB> <XOR>


// FLAGS
// INFO flags
const int HAS_INPUTS        = 0b0001 ;
const int HAS_OUTPUTS       = 0b0010 ;
const int IS_THROTTLE       = 0b0100 ;
// N.B. const int  IS_CENTRAL = 0b1000 ;? 

// INPUT flags
const int IS_RAILCOM   = 0b0001 ;
const int IS_OCCUPANCY = 0b0010 ;
const int IS_SWITCH    = 0b0100 ; 
const int IS_ACCESSORY = 0b1000 ;

// OUTPUT flags
const int DCC_CONVENTIONAL = 0b01 ; // DCC standard accessory
const int DCC_EXT          = 0b10 ; // DCC extended command
                                        

class TrainTasticDIY
{
public:
    TrainTasticDIY() ; // constructor
    void        begin(uint8_t VAL, char *description ) ;
    uint8_t     getMessage() ;
    void        sendByte( uint8_t b  ) ; // special method to issue 'external' commands from source code, can be removed in the future.

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
    uint8_t     getDirection();
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