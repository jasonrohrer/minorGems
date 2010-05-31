/***************************************************************************************/
/*   Example of Asyncronous Serial Comms on a PIC16F877                                */
/*                                                                                     */
/*   By: J.Winpenny                                                                    */
/*                                                                                     */
/*-------------------------------------------------------------------------------------*/
/*   Date:      |      Revision History                                                */
/*-------------------------------------------------------------------------------------*/
/*  16/11/1999  | Original Coding                                                      */
/*              |                                                                      */
/*              |                                                                      */
/*  25/1/2000   | Revised for C2C 3.28                                                 */
/*              |                                                                      */
/*              |                                                                      */
/*              |                                                                      */
/*              |                                                                      */
/*-------------------------------------------------------------------------------------*/
/* Target:  PIC16F877                                                                  */
/*                                                                                     */
/*       Baud: 9600                                                                    */
/*       Bits: 8                                                                       */
/*     Parity: none                                                                    */
/* Stop bits : 1                                                                       */
/*                                                                                     */
/* Warning: Clock frequency dependant                                                  */
/*                                                                                     */
/*                                                                                     */
/* This example sends a message then                                                   */
/* echos lines typed on a terminal.                                                    */
/*                                                                                     */
/***************************************************************************************/


#pragma CLOCK_FREQ 10000000  // Processor clock frequency in Hz. 
                            // Default value for PIC is 4000000 (4MHz) 
                            // and for SX 50000000 (50MHz).
                            
                            // This will not affect the comms speed !
                            // if you use a different frequency crystal then change SPBRG                                                                 



// My PORT Configuration    

#define PortAConfig 0x00
#define PortBConfig 0xf0
#define PortCConfig 0x98    /* SCL & SDA as Inputs */
#define PortDConfig 0x00
#define PortEConfig 0x00


// USART Register bits
#define CSCR 7
#define TX9  6
#define TXEN 5
#define SYNC 4
#define BRGH 2
#define TRMT 1
#define TX9D 0
#define SPEN 7
#define RX9  6
#define SREN 5
#define CREN 4
#define ADDEN 3
#define FERR 2
#define OERR 1
#define RX9D 0  
  
#define TRMT_MASK 2

// Masks for PIR1

#define PSPIF_MASK 0x80
#define ADIF_MASK  0x40
#define RCIF_MASK  0x20  
#define TXIF_MASK  0x10


// Registers for I2C

char SSPSTAT@0x94;  // Bank 1
char SSPCON@0x14;   // Bank 0
char SSPCON2@0x91;  // Bank 1
char SSPBUF@0x13;   // I2C Buffer
char SSPADD@0x93;   // I2C Slave Address register


// Bits of SSPSTAT

#define SMP 7
#define CKE 6
#define D_A 5
#define P   4
#define S   3
#define R_W 2
#define R_W_MASK 0x04
#define UA  1
#define BF  0


// Bits of SSPCON2 

#define GCEN    7
#define ACKSTAT 6
#define ACKDT   5
#define ACKEN   4
#define RCEN    3
#define PEN     2
#define RSEN    1
#define SEN     0


// Bits of PIR1

#define PSPIF  7
#define ADIF   6
#define RCIF   5 
#define TXIF   4
#define SSPIF  3
#define SSPIF_MASK 0x08
#define CCP1IF 2
#define TMR2IF 1
#define TMR1IF 0  

// Bits of SSPCON 

#define WCOL  7
#define SSPOV 6
#define SSPEN 5
#define CKP   4
#define SSPM3 3
#define SSPM2 2
#define SSPM1 1
#define SSPM0 0


// Port addresses
char PORTC@0x07;
char PORTD@0x08;
char PORTE@0x09;
 
// USART Registers
char TXREG@0x19;
char RCREG@0x1a;
char TXSTA@0x98;
char RCSTA@0x18;
char SPBRG@0x99;
                  
// Extra Ports on PIC16F877
char TRISC@0x87;
char TRISD@0x88;
char TRISE@0x89;

// Other regs

char PIE1@0x8c;
char PIE2@0x8d;
char PIR1@0x0c;
char PIR2@0x0d;
char PCON@0x8e;

// ADC bits  
char ADCON0@0x1f;
char ADCON1@0x9f;


// Function Declarations  

void Setup(void);                  // Setup the PIC
void ConfigureComms(void);         // Configure the comms
void SendChar(char);               // Send a character

char RxChars(void);                // Receive a characters when RX Interrupt occurs

void SendString( const char *ptr );// Send a const string


char MyFlags;                      // Status flags

char BufferIndex;                  // Yep ! an index to the buffer

char RxFifo[20];                   // Receive data buffer.

#define RX_BUFFER_SIZE 20

#define BufferReady 0              // Bit 0 of MyFlags
#define BufferReadyMask 0x01                      


const char *Msg1 ="PIC16F877 RS232 Test";  


//-----------------------------------------------------------------------------------
//                                    Start of MAIN
//-----------------------------------------------------------------------------------                  

void main(void)
{

  
  Setup();                           /* Setup the PIC */
  
  BufferIndex = 0; 
  
  clear_bit( MyFlags, BufferReady ); /* Clear the buffer ready flag */
  
  
  SendString( Msg1 );                /* Send a message to the terminal */


   
     while( 1 )
     {

            
      if ( MyFlags & BufferReadyMask )
         {
            /* ECHO the buffer back to the terminal */ 
            /* After the enter key was pressed      */
            
            while( RxFifo[ BufferIndex ] != 0  )
                  {
                     SendChar( RxFifo[ BufferIndex++ ] );
                  }
                  
            BufferIndex = 0;
            
            clear_bit( MyFlags, BufferReady ); /* Clear the buffer ready flag */
         }  
                                                                                
                                                                                                                                               
      clear_wdt();
      
      }// end while 1
   
     
         
} // end of Main()                 


void interrupt(void)
{
        
 if ( ( PIR1 & RCIF_MASK ) != 0 ) // If USART RX Interrupt
    {
       RxChars();                // Process the received character
       clear_wdt();              
       clear_bit( PIR1, RCIF );  // Clear flag  
    }

// Return from Interrupt
}




/*****************************************************/ 
/* Send a const string                               */
/* ( Null terminated )                               */
/*****************************************************/
         
void SendString(const char *ptr)
{
 
 char i;

  i = 0;
                // Check for end of string

    while( ptr[i] !=  0 )
         {
            SendChar( ptr[i++] );
         }  

}         
         

         
/*****************************************************/ 
/* setup PIC16F877 options,ports,interrupts          */
/*****************************************************/
void Setup(void)
{
  
  
  INTCON = 0x00;
  set_bit( INTCON, GIE );            // Enable Global Interrupts
  set_bit( INTCON, PEIE );           // Enable all Peripheral Interrupts
  
  set_bit( STATUS, RP0 );            // Register page 1
                                     // Compiler doesn't detect ram bank switch here !
                                     // with OPTION_REG
                                     
  OPTION_REG = 0x0C;                 // Set Option register
                                     // Prescaler = WDT
                                     // WDT rate := 1:16
  
  TRISD = PortDConfig;

  ADCON1 = 0x7f;                     // Disable ADC
  
  
  TRISA = PortAConfig;
  TRISB = PortBConfig;
  TRISC = PortCConfig;    
  TRISE = PortEConfig;
 
  clear_bit( STATUS, RP0 );          // Register page 0
  
  PIR1 = 0;
  
  clear_wdt();
  
  ConfigureComms();                  /* Configure USART for Asyncronous Comms */

}
         

 
/*******************************************************/
/* Configure USART for communications                  */
/*                                                     */
/* Asynchronous mode                                   */
/* 19,200 Baud  ( With 3.579545 Mhz Clock )            */
/* 8 data bits  ( For other rates see PIC16F8XX Data ) */
/* 2 stop bits                                         */
/* No Parity                                           */
/*                                                     */
/*******************************************************/
void ConfigureComms(void)
{
    set_bit( RCSTA, SPEN );    // Enable Serial port
    clear_bit( RCSTA, RX9 );   // 8 bit receive mode
        
    set_bit( STATUS, RP0 );    // *** Register page 1 ***
    
    clear_bit( TXSTA, TX9 );   // 8 bit transmit mode    
  
   // SPBRG = 0;               // SPBRG = 1  ( Set Baud rate 115,200 ) 
   // SPBRG = 5;               // SPBRG = 5  ( Set Baud rate 38,400 )
   // SPBRG = 22;              // SPBRG = 22 ( Set Baud rate 9,600 )
   // SPBRG = 11;              // SPBRG = 11 ( Set Baud rate 19,200 )
   
    //SPBRG = 22;                // SPBRG = 22 ( Set Baud rate 9,600 )             
    SPBRG = 64;
                               //------------------------------------------
                               //  For this value at a given clock rate 
                               //  see the microchip document           
                               //------------------------------------------                                
   
    set_bit( TXSTA, BRGH );    // RRGH = 1   ( High speed mode )
    clear_bit( TXSTA, SYNC );  // Asyncronous mode;
    
    set_bit( TXSTA, TXEN );    // Enable Transmitter
    
    set_bit( PIE1, RCIE );     // Enable Receive Interrupt
    
    clear_bit( STATUS, RP0 );  // *** Register page 0 ***
    
    set_bit( RCSTA, CREN );    // Enable continuous receive
    clear_bit( PIR1, RCIF );   // Clear Receive Interrupt flag

    set_bit( INTCON, PEIE );   // Enable all Peripheral Interrupts
    set_bit( INTCON, GIE );    // Enable Global Interrupts    
 
}


/*****************************************************/
/* Send a character over the RS232 Port              */
/*                                                   */
/*                                                   */
/*****************************************************/
void SendChar(char ch)
{

char TxEmpty;
  
    asm bsf STATUS, RP0;          // *** Register page 1 ***
    asm movf TXSTA, W;            // Save TXSTA value 
    asm bcf STATUS, RP0;          // *** Register page 0 ***
    asm movwf _TxEmpty_SendChar;  // Restore TXSTA value
     
       
  while ( ( TxEmpty & TRMT_MASK ) == 0 ) // Wait for TX Empty
   {
      asm bsf STATUS, RP0;         // *** Register page 1 ***
      asm movf TXSTA, W;           // Save TXSTA value 
      asm bcf STATUS, RP0;         // *** Register page 0 ***
      asm movwf _TxEmpty_SendChar; // Restore TXSTA value
   }

   asm bcf STATUS, RP0;         // *** Register page 0 ***
  
   
   TXREG = ch;                     // Load the TXREG
   
   clear_wdt();
}



/*****************************************************/
/* Receive a character over the RS232 Port           */
/*                                                   */
/* Called from Interrupt service routine             */
/*                                                   */
/* Returns the char received                         */
/* and saves it in the buffer                        */
/*                                                   */
/*****************************************************/
char RxChars(void)
{


      if ( ( RCSTA & 6 ) == 0 )     // Then if no errors              
         {                          // Process received character
     
                 // If terminated by a carrage return
                 // or Buffer end reached
             
                               
              if ( ( RCREG == 13 ) || ( BufferIndex == RX_BUFFER_SIZE ) ) 
                 {
                    set_bit( MyFlags, BufferReady ); /* Set a flag to indicate line received */
                    RxFifo[ BufferIndex ] = 0;       /* NULL Terminate the buffer */
                 }  
              else  
                 {          
                   RxFifo[ BufferIndex++] = RCREG; // Save the data        
                 }  
                                  
                                                                                                                    
              set_bit( RCSTA, CREN ); // Enable receiver.
         }
      else
         {
           
//          process any errors here
//          Beware, we are in the Interrupt routine.
           
//          ...

           
            clear_bit( RCSTA, CREN ); // Clear any errors   
            set_bit( RCSTA, CREN );   // Enable receiver.
         }  
        

  return RCREG;    

}

