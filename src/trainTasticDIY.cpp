#include "TrainTasticDIY.h"

// https://traintastic.org/en-us/manual/master#tdiyp

// #define DEBUG



TrainTasticDIY::TrainTasticDIY()  // constructor
{
}


// just sends the description and report our type
void TrainTasticDIY::begin( uint8_t type, char *description  ) // N.B. type is not used yet.
{
    int length = 0;
    while (description[length] != '\0') { length ++ ; }// calculate length of description
    length ++ ;

    uint8_t message[length+1] = {OPC_SEND_INFO, length} ;

    for( int i = 2 ; i < length+3 ; i ++ )   
    {
        message[i] = *description ++ ;
    }

    transmitt( message) ;
}

void TrainTasticDIY::sendByte( uint8_t b  )
{
    serialByte = b ;
    newByte = true ;
} 


void TrainTasticDIY::transmitt( uint8_t* message )
{
    uint8_t XOR = 0x00;                 // declare checksum

    uint8_t length = *message & 0x0F ;  // reading length
    if( length == 0x0F )                // lenth is stored in second byte
    {
        length = *(message+1) ;
    }

    for( uint8_t i = 0 ; i < length+1 ; i++ )
    {
       uint8_t toSend = *message ++ ;
       XOR ^= toSend ;                  // calculate checksum
       
        #ifdef DEBUG
        Serial.print( toSend, HEX ) ; Serial.write(' ') ;
        #else
        Serial.write( toSend ) ;
        #endif
    }
    #ifdef DEBUG
    Serial.println( XOR, HEX ) ;
    #else
    Serial.write( XOR ) ;
    #endif
}

uint8_t TrainTasticDIY::getMessage() 
{
    if( Serial.available() > 0 )
    {
        serialByte = Serial.read() ;
        newByte    = true ;
    }

    if( newByte == false ) return 0 ;
    newByte = false ;

    // #ifdef DEBUG
    // Serial.println(serialByte) ;
    // #endif

    if( mode == getOpcode )
    {
        #ifdef DEBUG
        Serial.println(F( "\r\nreceiving message")) ;
        #endif
        
        if( serialByte & 0x0F == 0x0F ) // if true message length will be in following byte
        {
            mode = getMessageSize ;
        }
        else
        {
            messageLength = serialByte & 0x0F ; // store length of message
            if( messageLength == 0 )
            {
                #ifdef DEBUG
                Serial.println(F( "message received (just opcode)")) ;
                #endif
                ptr = &buf[0] ;
                *ptr = serialByte ;

                return 1 ;
            }
            else
            {
                mode = receivingMessage ;
            }            
        }
        
        ptr = &buf[0] ;
        *ptr = serialByte ;
        ptr ++ ;
    }
    else if( mode == getMessageSize )
    {
        *ptr = serialByte ;
        messageLength = *ptr ;
        ptr ++ ;
        mode = receivingMessage ;
    }

    else if( mode == receivingMessage )
    {
        *ptr = serialByte ;
        ptr ++ ;
        if( ++ byteCounter == messageLength )
        {
            #ifdef DEBUG
            Serial.println(F( "message received, processing")) ;
            #endif
            
            mode = getOpcode ;
            return 1 ;      // flag that complete message is received. May want to use this to distinct messages for throttles or output units.
        }
    }

    return 0 ;
}

void TrainTasticDIY::processMessage()
{
    uint8_t opcode = buf[0] ;
    
    switch( opcode )
    {
        case OPC_HEARTBEAT:
        {
            uint8_t message[] = { OPC_HEARTBEAT, 0 } ;
            #ifdef DEBUG
            Serial.println(F( "\r\nResponding OPC_HEARTBEAT")) ;
            #endif
            transmitt( message ) ;
        } break ;

        case OPC_GET_INFO:
        {
            // uint8_t message[] = { OPC_SEND_INFO,0 /*length param1 .. param255*/ } ;
            // TODO fill in parameters message[1] = ... etc
            #ifdef DEBUG
            Serial.println(F( "\r\nResponding OPC_GET_INFO")) ;
            #endif
            //begin( IS_THROTTLE, "ThrottleX ds V1.0" ) ;
            //transmitt( message ) ;
        } break ;

        case OPC_GET_FEATURES:
        {
            uint8_t message[] = {OPC_SEND_FEATURES, IS_THROTTLE, 0, 0, 0 /*param1, param2, param3 param4*/ } ;
            // TODO fill in parameters message[1] = ... etc
            // make notify?
            #ifdef DEBUG
            Serial.println(F( "Responding OPC_GET_FEATURES")) ;
            #endif
            transmitt( message ) ;
        } break ;


        case OPC_GET_INPUT:
        {
            #ifdef DEBUG
            Serial.println(F( "Responding OPC_GET_INPUT")) ;
            #endif

            if( notifyGetInput ) notifyGetInput() ;
        } break ;

        case OPC_GET_OUTPUT:
        {
            #ifdef DEBUG
            Serial.println(F( "Responding OPC_GET_OUTPUT")) ;
            #endif

            if( notifyGetOutput ) notifyGetOutput() ;
            
        } break ;

        case OPC_SET_SPEED:
        {
            #ifdef DEBUG
            Serial.println(F( "Responding OPC_SET_SPEED")) ;
            #endif

            if( notifySetSpeed ) notifySetSpeed() ; // TODO add parameters 0x37 <TH> <TL> <AH> <AL> <SP> <SM> <FL> <checksum>
        }
    }
}

TrainTasticDIY TrainTastic = TrainTasticDIY() ;

// THROTTLE CLASS
TrainTasticDIYThrottle::TrainTasticDIYThrottle()
{
} 


void TrainTasticDIYThrottle::setAddress( uint16_t newAddress )
{
    address = newAddress ;
}

void TrainTasticDIYThrottle::setSpeed( uint8_t speed )  // N.B. should be in throttle class
{ // 0x37 <TH> <TL> <AH> <AL> <SP> <SM> <FL> <checksum>
    uint8_t message[] = { 
        OPC_SET_SPEED, 
        highByte(id),       lowByte(id),
        highByte(address),  lowByte(address),
        speed,              //
        128,                // // SM max speed 
        SET_SPEED           // FL
    } ; // set speed only

    TrainTastic.transmitt( message ) ;
}

uint8_t TrainTasticDIYThrottle::getSpeed()  // N.B. should be in throttle class
{
    return speed ;
}

void TrainTasticDIYThrottle::setDirection( uint8_t newDir ) // N.B. should be in throttle class
{
    direction = newDir ;
     // 0x37 <TH> <TL> <AH> <AL> <SP> <SM> <FL> <checksum>
    uint8_t message[] = { 
        OPC_SET_SPEED, 
        highByte(id),       lowByte(id),
        highByte(address),  lowByte(address),
        speed,                  //
        128,                    // // SM max speed 
        direction | SET_DIR     // FL
    } ; // set speed only

    TrainTastic.transmitt( message ) ;
}

uint8_t TrainTasticDIYThrottle::getDirection(void)  // N.B. should be in throttle class
{
    return direction ;
}

//OPC_SET_FUNCTION <TH> <TL> <AH> <AL> <FN> <checksum>
void TrainTasticDIYThrottle::setFunction( uint8_t Fx, uint8_t state )  // N.B. should be in throttle class
{
    uint8_t message[] = 
    {
        OPC_SET_FUNCTION,
        highByte(id),       lowByte(id),
        highByte(address),  lowByte(address),
        Fx | (state<<7)
    } ;

    TrainTastic.transmitt( message ) ;
}

uint8_t TrainTasticDIYThrottle::getFunction()  // N.B. should be in throttle class
{
}