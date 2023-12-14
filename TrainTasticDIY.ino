#include "src/io.h"
#include "src/date.h"
#include "src/version.h"
#include "src/macros.h"
#include "src/trainTasticDIY.h"


TrainTasticDIYThrottle throttle ;

uint8_t speed = 2 ;

void setup()
{
    initIO() ;
    Serial.begin( 19200 ) ;
    // Serial.println( version ) ;
    // Serial.println( date ) ;

    //TrainTastic.begin( IS_THROTTLE, "ThrottleX ds V1.0" ) ; // give parameters
    throttle.setAddress( 8 ) ; 

    pinMode(13, OUTPUT);
}

void loop()
{
    REPEAT_MS( 1000 )
    {
        static uint8_t state = 0 ;

        if( state == 0 ) throttle.setFunction( 1, 1 ) ;         // turn on F1
        if( state == 1 ) throttle.setFunction( 1, 0 ) ;         // turn off F1
        if( state == 2 ) throttle.setSpeed( speed ++ ) ;
        // if( state == 3 ) TrainTastic.sendByte( OPC_HEARTBEAT ) ;
        // if( state == 4 ) TrainTastic.sendByte( OPC_GET_INFO ) ;
        // if( state == 5 ) TrainTastic.sendByte( OPC_GET_FEATURES ) ;
        //if( state == 3 ) TrainTastic.begin( IS_THROTTLE, "ThrottleX ds V1.0" ) ; // give parameters

        if( ++state == 6 ) {state = 0 ; PORTB ^= (1<<5); }
        //Serial.println(state) ;
        
    } END_REPEAT
        

    if( TrainTastic.getMessage() )  // buffers a message, returns true when message is received
    {
        TrainTastic.processMessage() ; // call this N.B. Might as well make this private method and call it from 'getMessage()' directly. Perhaps rename to update or let it be processMessage
    }

}

void notifySetSpeed()
{
    PORTB ^= (1<<5);
}
/*



*/