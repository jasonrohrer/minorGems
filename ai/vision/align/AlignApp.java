/*
 * Modification History
 *
 * 2001-April-11   Jason Rohrer
 * Created.  
 * Has been tested and debugged.
 */


import java.awt.*;
import java.awt.event.*;
import java.net.*;
import java.awt.image.*;
import java.awt.geom.*;


/**
 * Applet to assist in Left-Right webcam stereo pair alignment.
 *
 * @author Jason Rohrer.
 */
public class AlignApp {

	int mWidth = 500;
	int mHeight = 400;

	String mLeftURLString;
	String mRightURLString;

	Component mObserver;
	
	
	public static void main( String inArgs[] ) {
		if( inArgs.length == 2 ) {
			// two URLS passed in
			new AlignApp( inArgs[0], inArgs[1] );
			}
		else if( inArgs.length == 0 ) {
			// no URLS passed in
			new AlignApp();
			}
		else {
			// incorrect usage
			usage( "AlignApp" );
			}
		}



	/**
	 * Prints usage information for this application.
	 *
	 * @param inAppName the name of the application.
	 */
	private static void usage( String inAppName ) {
		System.out.println( "Usage:" );
		System.out.println( "   " + inAppName + " [left_url right_url]" );
		System.out.println( "Examples:" );
		System.out.println( "   " + inAppName + 
			" http://mysite.com	http://mysite2.com" );
		System.out.println( "   " + inAppName );
		System.exit( -1 );	
		}



	/**
	 * Constructs an AlignApp with default URLs.
	 */
	public AlignApp() {
		// use default URLS
		this(
			"http://us.a1.yimg.com/us.yimg.com/i/ww/m5v2.gif",
			"http://www.google.com/images/title_homepage4.gif" );
		}
		
	
	
	/**
	 * Constructs an AlignApp..
	 *
	 * @param inLeftURLString the left image URL.
	 * @param inRightURLString the righ image URL.
	 */	
	public AlignApp( String inLeftURLString, String inRightURLString ) {
		
		mLeftURLString = inLeftURLString;
		mRightURLString = inRightURLString;
		
		SettingsFrame connectFrame = new SettingsFrame();
		mObserver = connectFrame;
		
		boolean malformed = true;

		URL urls[] = new URL[2];
		
		String baseSettingsFrameTitle = "connection settings";		
		String settingsFrameTitle = "connection settings";
		
		// loop until proper URLs entered
		while( malformed ) {
			String urlStrings[] =
				connectFrame.getURLS( settingsFrameTitle );

			try {
				urls[0] = new URL( urlStrings[0] );
				urls[1] = new URL( urlStrings[1] );
				malformed = false;
				}
			catch( MalformedURLException inE ) {
				System.out.println( "Malformed URL." );
				malformed = true;
				settingsFrameTitle = "malformed URL:  " +
					baseSettingsFrameTitle;
				}
			
			// try loading the images
			// use createImage instead of getImage to force a reload
			Image left = Toolkit.getDefaultToolkit().createImage( urls[0] );
			Image right = Toolkit.getDefaultToolkit().createImage( urls[1] );
			
			MediaTracker tracker = new MediaTracker( connectFrame );
			tracker.addImage( left, 0 );
			tracker.addImage( right, 1 );
			try{
				System.out.println( "Testing left image load." );
				tracker.waitForID(0);
				if( tracker.isErrorID(0) ) {
					System.out.println( "  Error loading left image." );
					malformed = true;
					settingsFrameTitle = "loading failed:  " +
						baseSettingsFrameTitle;
					}
				else {
					System.out.println( "  Load successful." );
					}
				System.out.println( "Testing right image load." );	
				tracker.waitForID(1);
				if( tracker.isErrorID(1) ) {
					System.out.println( "  Error loading right image." );
					malformed = true;
					settingsFrameTitle = "loading failed:  " +
						baseSettingsFrameTitle;
					}
				else {
					System.out.println( "  Load successful." );
					}	
				}
			catch(InterruptedException e) {
				}	
				
			}
		
		ImageFrame displayFrame = new ImageFrame();
		mObserver = displayFrame;
		
		while( true ) {
			// use createImage instead of getImage to force a reload
			Image left = Toolkit.getDefaultToolkit().createImage( urls[0] );
			Image right = Toolkit.getDefaultToolkit().createImage( urls[1] );
			
			MediaTracker tracker = new MediaTracker( displayFrame );
			tracker.addImage( left, 0 );
			tracker.addImage( right, 1 );
			try{
				tracker.waitForID(0);
				if( tracker.isErrorID(0) ) {
					System.out.println( "Error loading left image." );
					}
				tracker.waitForID(1);
				if( tracker.isErrorID(1) ) {
					System.out.println( "Error loading right image." );
					}
				}
			catch(InterruptedException e) {
				}
			
			displayFrame.setImage( blendImages( left, right ) );
			}
		}


	
	/**
	 * Alpha blends two images.
	 *
	 * @return a new image containing a blend of the two images.
	 */
	private Image blendImages( Image inImageA, Image inImageB ) {
		
		// find max width and height
		int w = inImageA.getWidth( mObserver );
		if( inImageB.getWidth( mObserver ) > w ) {
			w = inImageB.getWidth( mObserver );
			}
		int h = inImageA.getHeight( mObserver );
		if( inImageB.getHeight( mObserver ) > h ) {
			h = inImageB.getHeight( mObserver );
			}
		
		BufferedImage returnImage = 
			new BufferedImage( w, h, BufferedImage.TYPE_INT_ARGB );
			
		Graphics2D graphics = returnImage.createGraphics();
		
		// draw A with no compositing
		graphics.drawImage( inImageA, new AffineTransform(), mObserver );
		
		// draw B with alpha compositing
		AlphaComposite myAlpha = 
			AlphaComposite.getInstance( AlphaComposite.SRC_OVER, 0.5f );
		graphics.setComposite( myAlpha );
		graphics.drawImage( inImageB, new AffineTransform(), mObserver );			
		
		return returnImage;
		}

	
	
	/**
	 * Dialog box frame for connection settings.
	 */
	private class SettingsFrame
				extends Frame
				implements ActionListener {
		
		private TextField mServerLField = new TextField( 40 );
		private TextField mServerRField  = new TextField( 40 );;
		private Label mServerLLabel = new Label( "left server image URL: " );
		private Label mServerRLabel = new Label( "right server image URL: " );

		private Button mConnectButton = new Button( "connect" );
		
		
		
		/**
		 * Constructs a settings frame.  
		 * Will not be displayed until getURLS is called.
		 */
		public SettingsFrame() {
			setLayout( new FlowLayout( FlowLayout.LEFT ) );
			setSize( 300, 200 );
			
			add( mServerLLabel );
			add( mServerLField );
			add( mServerRLabel );
			add( mServerRField );

			add( mConnectButton );

			mConnectButton.addActionListener( this );

			mServerLField.setText( mLeftURLString );
			mServerRField.setText( mRightURLString );
			}



		/**
		 * Gets URLs for two servers by displaying this frame.
		 *
		 * Does not return until user clicks the "connect" button.
		 * Do not call this method from the even dispatch thread.
		 *
		 * @return an array of strings, one for each of two URLs.
		 */
		public synchronized String[] getURLS( String inTitle ) {

			setTitle( inTitle );
			setVisible( true );

			// wait for an action
			try {
				this.wait();
				}
			catch( InterruptedException inException ) {
				}

			String returnArray[] = new String[2];

			returnArray[0] = mServerLField.getText();
			returnArray[1] = mServerRField.getText();

			setVisible( false );

			return returnArray;
			}



		// implements the action listener interface
		public synchronized void actionPerformed( ActionEvent inEvent ) {
			// signal the waiting thread
			this.notify();
			}
	
		}

	
	
	/**
	 * Frame that displays an image.
	 */
	private class ImageFrame extends Frame {

		Image mImage;
		
		
		
		/**
		 * Constructs and displays an image frame.
		 */
		public ImageFrame() {
			super( "camera alignment utility" );

			setSize( mWidth, mHeight );
			setVisible( true );
			}


		
		/**
		 * Sets the image to be displayed.
		 * 
		 * @param inImage the image to be displayed.
		 */
		public void setImage( Image inImage ) {
			mImage = inImage;
			repaint();
			}



		// overrides the Frame update method to prevent flicker
		public void update( Graphics inGraphics ) {
			paint( inGraphics );
			}
		

		
		// overrides the Frame paint method
		public void paint( Graphics inGraphics ) {
			if( mImage != null ) {
				inGraphics.drawImage( mImage, 30, 30, this );
				}
			}
		
		}
	


	}
