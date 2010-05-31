/*
 * Modification History
 *
 * 2001-April-11   Jason Rohrer
 * Created.
 */

import java.io.*;
import java.util.*;
import javax.comm.*;


/**
 * Sonar unit test class.
 *
 * @author Jason Rohrer
 */
public class Sonar {

	InputStream mInputStream;
	OutputStream mOutputStream;
	SerialPort mSerialPort;
	DisplayFrame mFrame;


	
	public static void main( String inArguments[] ) {
		new Sonar();
		}


	/**
	 * Constrcts a sonar tester and runs it.
	 */
	public Sonar() {
		mFrame = new DisplayFrame();
		
		Enumeration portList =
			CommPortIdentifier.getPortIdentifiers();
		CommPortIdentifier portId = null;
		CommPortIdentifier tempPortId;
		
		while( portList.hasMoreElements() ) {
			tempPortId = (CommPortIdentifier)( portList.nextElement() );
			if( tempPortId.getPortType() == CommPortIdentifier.PORT_SERIAL ) {
				if( tempPortId.getName().equals( "COM1" ) ) {
				//if (portId.getName().equals("/dev/term/a")) {
					portId = tempPortId;
					}
				}
			}	
		
		System.out.println( "Serial port found." );
		// now we have our port ID
		
		try {
			mSerialPort = (SerialPort)( portId.open( "Sonar", 2000 ) );
			} 
		catch( PortInUseException e ) {
			System.out.println( "Port in use." );
			System.exit( -1 );
			}
		try {
			mInputStream = mSerialPort.getInputStream();
			mOutputStream = mSerialPort.getOutputStream();
			} 
		catch( IOException e ) {
			System.out.println( "Opening streams to port failed." );
			System.exit( -1 );
			}
		
		System.out.println( "Serial port opened." );
		/*
		try {
			mSerialPort.addEventListener( this );
			}
		catch( TooManyListenersException e ) {
			System.out.println( "too many listeners on port" );
			System.exit( -1 );
			}
		*/
		try {
			mSerialPort.setSerialPortParams( 9600,
				SerialPort.DATABITS_8,
				SerialPort.STOPBITS_1,
				SerialPort.PARITY_NONE );
			} 
		catch( UnsupportedCommOperationException e ) {
			System.out.println( "Unable to set serial port parameters." );
			System.exit( -1 );
			}
		
		byte [] readByte = new byte[1];
		
		// write a CR LF to get a prompt
		byte [] returnBytes =  new byte[2];
		
		returnBytes[0] = (byte)13;
		returnBytes[1] = (byte)10;
		try{
			mOutputStream.write( returnBytes );
			}
		catch( IOException inE ) {
			System.out.println( "Exception while writing byte" );
			}
		
		
		readByte[0] = 0;
		
		try {
		
			waitForPrompt();
			
			// turn power on
			System.out.println( "Turning power on." );
			String powerOn = "CP 0F";
			
			mOutputStream.write( powerOn.getBytes() );
			mOutputStream.write( returnBytes );
			
			waitForPrompt();
			mOutputStream.write( powerOn.getBytes() );
			mOutputStream.write( returnBytes );
			
			waitForPrompt();
			
			
			
			while( true ) {
				int distance[] = new int[3];
				for( int i=1; i<4; i++ ) {
					//try {
						//Thread.sleep( 500 );
					//	}
					//catch( Exception e ) {
					//	}
					String request = "RT 000" + i;
					mOutputStream.write( request.getBytes() );
					mOutputStream.write( returnBytes );
					
					// ignore the echo
					byte [] echoBuffer = new byte[9];
					waitForReady( 9 );
					mInputStream.read( echoBuffer );
					
					//System.out.println( "echo:  "+ new String( echoBuffer ) );
					
					
					// read the value
					byte [] readBuffer = new byte[4];
					
					waitForReady( 4 );
					mInputStream.read( readBuffer );
					
					//System.out.println( "read:  "+ new String( readBuffer ) );
					
					String hexValue = new String( readBuffer );
					
					//String hexValue = bufferString.substring( 0, 4 );
					
					distance[i-1] = 
						( Integer.decode( "#" + hexValue ) ).intValue();
				
					
					waitForPrompt();	
					}
				mFrame.setValues( distance[0], distance[1], distance[2] );			
				}
			}
		catch( IOException inE ) {
			System.out.println( "Exception while reading byte" );
			}
		
		 
		}

	

	/**
	 * Waits for a prompt (*).
	 *
	 * @throws IOException if reading from the stream fails.
	 */
	private void waitForPrompt() throws IOException {
		byte [] readByte = new byte[1];
		while( readByte[0] != '*' ) {

			mInputStream.read( readByte );

			}
		//System.out.println( "Got a prompt!" );
		}


	
	/**
	 * Waits until a specified number of stream bytes are ready.
	 *
	 * @param inNumBytes the number of bytes to wait for.
	 *
	 * @throws IOException if the stream operation fails.
	 */
	private void waitForReady( int inNumBytes ) throws IOException {
		try {
			while( mInputStream.available() < inNumBytes ) {
				Thread.sleep( 10 );
				}
			}
		catch( InterruptedException inE ) {
			
			}
		}
	
	/*
	public void serialEvent( SerialPortEvent inEvent ) {
		switch( inEvent.getEventType() ) {
        	case SerialPortEvent.DATA_AVAILABLE:
        		byte [] readByte = new byte[1];
		
				readByte[0] = 0;
				
				while( readByte[0] != '*' ) {
					try{
						mInputStream.read( readByte );
						System.out.print( new String( readByte ) );
						}
					catch( IOException inE ) {
						System.out.println( "Exception while reading byte" );
						}
					}
				System.out.println( "Got a prompt!" );
        		break;
        	default:
        		break;
        	}
		}
	*/

	/**
	 * Frame that displays a bar graph.
	 */
	private class DisplayFrame extends java.awt.Frame {
		int mA, mB, mC;
	
		DisplayFrame() {
			super( "Sonar Readings" );
			
			mA = 0;
			mC = 0;
			mB = 0;
			
			setSize( 400, 400 );
			
			setVisible( true );
			}



		// overrides Frame.paint
		public void paint( java.awt.Graphics inGraphics ) {
			
			inGraphics.setColor( java.awt.Color.white );
			inGraphics.fillRect( 0, 0, 400, 80 );
			
			inGraphics.setColor( java.awt.Color.black );
			
			inGraphics.fillRect( 0, 30, mA / 10, 10 );
			
			inGraphics.fillRect( 0, 50, mB / 10, 10 );
			inGraphics.fillRect( 0, 70, mC / 10, 10 );
			
			}
		

		/**
		 * Sets the values to display in the graph.
		 *
		 * @param inA the value for the first bar.
		 * @param inB the value for the second bar.
		 * @param inC the value for the third bar.
		 */
		public void setValues( int inA, int inB, int inC ) {
			mA = inA;
			mB = inB;
			mC = inC;
			
			//System.out.println( "values = " + mA + ", " + mB + ", " + mC );
			
			repaint();
			}
		
		}
	
	}
