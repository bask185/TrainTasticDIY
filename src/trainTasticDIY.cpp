#include "TrainTasticDIY.h"

// https://traintastic.org/en-us/manual/master#tdiyp

// #define DEBUG

TrainTasticDIY::TrainTasticDIY()  // constructor
{
}

void TrainTasticDIY::begin( uint8_t VAL, char *description  ) // not needed I believe
{
    int length = 0;
    while (description[length] != '\0') { length ++ ; }// calculate length of description
    length ++ ;

    uint8_t message[length+1] = {OPC_SEND_INFO, length} ;

    for( int i = 2 ; i < length+3 ; i ++ )   
    {
        message[i] = *description ++ ;
    }

    sendMessage( message) ;
}

void TrainTasticDIY::sendByte( uint8_t b  )
{
    serialByte = b ;
    newByte = true ;
} 


void TrainTasticDIY::sendMessage( uint8_t* message )
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
    if( newByte == false ) return 0 ;
    newByte = false ;

    // #ifdef DEBUG
    // Serial.println(serialByte) ;
    // #endif

    if( mode == getOpcode )
    {
        // #ifdef DEBUG
        // Serial.println(F( "receiving message")) ;
        // #endif
        
        if( serialByte & 0x0F == 0x0F ) // if true message length will be in following byte
        {
            mode = getMessageSize ;
        }
        else
        {
            messageLength = serialByte & 0x0F ; // store length of message (may need a +1?
            if( messageLength == 0 )
            {
                // #ifdef DEBUG
                // Serial.println(F( "message received (just opcode)")) ;
                // #endif
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
            // #ifdef DEBUG
            // Serial.println(F( "message received")) ;
            // #endif
            
            mode = getOpcode ;
            return 1 ;
        }
    }

    return 0 ;
}

void TrainTasticDIY::processMessage()
{
    uint8_t opcode = buf[0] ;

    Serial.println(opcode) ;
    
    switch( opcode )
    {
        case OPC_HEARTBEAT:
        {
            uint8_t message[] = { OPC_HEARTBEAT, 0 } ;
            sendMessage( message ) ;
            // make notify?
        } break ;

        case OPC_GET_INFO:
        {
            uint8_t message[] = { OPC_SEND_INFO,0 /*length param1 .. param255*/ } ;
            // TODO fill in parameters message[1] = ... etc
            sendMessage( message ) ;
            // make notify?
        } break ;

        case OPC_GET_FEATURES:
        {
            uint8_t message[] = {OPC_SEND_FEATURES,0 /*param1, param2, param3 param4*/ } ;
            // TODO fill in parameters message[1] = ... etc
            sendMessage( message ) ;
            // make notify?
        } break ;


        case OPC_GET_INPUT:
        {
            uint8_t message[] = {OPC_SEND_INPUT,0 /*param1, param2, param3*/ } ;
            // TODO fill in parameters message[1] = ... etc
            sendMessage( message ) ;
            // make notify?

        } break ;

        case OPC_GET_OUTPUT:
        {
            uint8_t message[] = {OPC_SET_OUTPUT,0 /*param1, param2, param3*/ } ;
            // TODO fill in parameters message[1] = ... etc
            sendMessage( message ) ;
            // make notify?
        } break ;

        case OPC_SET_SPEED:
        {
        }
    }
}



void TrainTasticDIY::setSpeed( uint8_t speed )
{ // 0x37 <TH> <TL> <AH> <AL> <SP> <SM> <FL> <checksum>
    uint8_t message[] = { 
        OPC_SET_SPEED, 
        highByte(id),       lowByte(id),
        highByte(address),  lowByte(address),
        speed,               //
        128,                // // SM max speed 
        0b10000000          // FL
    } ; // set speed only

    sendMessage( message ) ;
}

uint8_t TrainTasticDIY::getSpeed()
{
    return speed ;
}

void TrainTasticDIY::setDirection(uint8_t)
{
}

uint8_t TrainTasticDIY::getDirection(void)
{
}

//OPC_SET_FUNCTION <TH> <TL> <AH> <AL> <FN> <checksum>
void TrainTasticDIY::setFunction( uint8_t Fx, uint8_t state )
{
    uint8_t message[] = 
    {
        OPC_SET_FUNCTION,
        highByte(id),       lowByte(id),
        highByte(address),  lowByte(address),
        Fx | (state<<7)
    } ;

    sendMessage( message ) ;
}

uint8_t TrainTasticDIY::getFunction()
{
}