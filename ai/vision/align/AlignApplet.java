/*
 * Modification History
 *
 * 2001-April-10   Jason Rohrer
 * Created.
 *
 * 2001-April-11   Jason Rohrer
 * Does not work as expected because of applet security restrictions.
 * Has been replaced by AlignApp.java. 
 */

import java.applet.Applet;
import java.awt.*;
import java.awt.event.*;
import java.net.*;


/**
 * Applet to assist in Left-Right webcam pair alignment.
 *
 * @author Jason Rohrer.
 */
public class AlignApplet extends Applet {

	int mWidth = 500;
	int mHeight = 400;

	
	public void init() {

		SettingsFrame connectFrame = new SettingsFrame();

		boolean malformed = true;

		URL urls[] = new URL[2];

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
				malformed = true;
				settingsFrameTitle = "malformed URL:  " +
					settingsFrameTitle;
				}
			}
		
		ImageFrame displayFrame = new ImageFrame();

		while( true ) {
			Image left = getImage( urls[0] );
			Image right = getImage( urls[1] );

			displayFrame.setImage( left );
			}
		
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
		
		public SettingsFrame() {
			setLayout( new FlowLayout( FlowLayout.LEFT ) );
			setSize( 300, 200 );
			
			add( mServerLLabel );
			add( mServerLField );
			add( mServerRLabel );
			add( mServerRField );

			add( mConnectButton );

			mConnectButton.addActionListener( this );

			mServerLField.setText(
				"http://www.google.com/images/title_homepage4.gif" );
			mServerRField.setText(
				"http://www.google.com/images/title_homepage4.gif" );
			
			// setVisible( true );
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
				wait();
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
			notify();
			}
	
		}

	
	/**
	 * Frame that displays an image.
	 */
	private class ImageFrame extends Frame {

		Image mImage;
		
		public ImageFrame() {
			super( "camera alignment utility" );

			setSize( mWidth, mHeight );
			setVisible( true );
			}

		
		public void setImage( Image inImage ) {
			mImage = inImage;
			repaint();
			}


		
		public void paint( Graphics inGraphics ) {
			inGraphics.drawImage( mImage, 0, 0, this );
			}
		
		}
	


	}








