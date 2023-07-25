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
    delay(5000);
}

void loop()
{
    REPEAT_MS( 1000 )
    {
        static byte state = 0 ;

        if( state == 0 ) throttle.sendByte(OPC_HEARTBEAT) ;
        if( state == 1 ) throttle.sendByte(OPC_GET_INFO) ;
        if( state == 2 ) throttle.sendByte(OPC_GET_FEATURES) ;
        if( state == 3 )
        {
            throttle.sendByte(OPC_SET_SPEED) ;
            throttle.sendByte(0) ; throttle.sendByte(100) ; // throttle id
            throttle.sendByte(1) ; throttle.sendByte(10) ; // address
            throttle.sendByte(1) ;  // speed
            throttle.sendByte(10) ; 
            throttle.sendByte(128) ; // max speed 
            
        }

        if( ++state == 4 ) state = 0 ;
        //Serial.println(state) ;
        
    } END_REPEAT
        

    if( throttle.getMessage() )
    {
        throttle.processMessage() ;
    }

}