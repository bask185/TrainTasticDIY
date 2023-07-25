#include "src/io.h"
#include "src/date.h"
#include "src/version.h"
#include "src/macros.h"
#include "src/trainTasticDIY.h"

TrainTasticDIY throttle ;

void setup()
{
    initIO() ;
    Serial.begin( 115200 ) ;
    // Serial.println( version ) ;
    // Serial.println( date ) ;

    throttle.begin( IS_THROTTLE, "ThrottleX ds V1.0" ) ; // give parameters
}

void loop()
{
    REPEAT_MS( 1000 )
    {
        static byte state = 0 ;

        // if( state == 0 ) throttle.sendByte(OPC_HEARTBEAT) ;     // 0x00
        if( state == 1 ) throttle.sendByte(OPC_GET_INFO) ;      // 0xF0
        if( state == 2 ) throttle.sendByte(OPC_GET_FEATURES) ;  // 0xE0
        // if( state == 3 )
        // {
        //     throttle.sendByte(OPC_SET_SPEED) ;                  // 0x37
        //     throttle.sendByte(0) ; throttle.sendByte(100) ; // throttle id
        //     throttle.sendByte(1) ; throttle.sendByte(10) ; // address
        //     throttle.sendByte(1) ;  // speed
        //     throttle.sendByte(10) ; 
        //     throttle.sendByte(128) ; // max speed 
        // }
        if( state == 4 ) throttle.setSpeed( 0x37 ) ;

        if( ++state == 6 ) state = 0 ;
        //Serial.println(state) ;
        
    } END_REPEAT
        

    if( throttle.getMessage() )  // buffers a message, returns true when message is received
    {
        throttle.processMessage() ; // call this N.B. Might as well make this private method and call it from 'getMessage()' directly. Perhaps rename to update or let it be processMessage
    }

}