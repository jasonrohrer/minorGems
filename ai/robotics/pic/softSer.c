/*
 * Modification History
 *
 * 2001-April-15   Jason Rohrer
 * Created from code found at:
 * http://www.iptel-now.de/HOWTO/PIC/pic.html
 */

//RS232 settings
#pragma RS232_TXPORT PORTA
#pragma RS232_RXPORT PORTA
#pragma RS232_TXPIN  1
#pragma RS232_RXPIN  4
#pragma RS232_BAUD 9600
#pragma TRUE_RS232 1

//Timing settings
#pragma CLOCK_FREQ 10000000

main()
{
    char i;
    char j;

    //Hardware Initialization
    disable_interrupt( GIE );
    set_bit( STATUS, RP0 );
    set_tris_a( 0x10 );
    set_tris_b( 0 );
    clear_bit( STATUS, RP0 );
    output_port_a( 0 );
    output_port_b( 0 );

    while(1) {
        for (i=0; i <= 255; i++)
        {
            output_port_b(i);
            putchar(i);

            delay_ms(100);
        }
    }
}

