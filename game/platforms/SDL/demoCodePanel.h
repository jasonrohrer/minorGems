

// shows panel until a correct code has been entered
// assumes font TGA in "graphics" folder
void showDemoCodePanel( ScreenGL *inScreen, const char *inFontTGAFileName,
                        int inWidth, int inHeight );

// use this to check if demo checking done
char isDemoCodePanelShowing();


// call if app exits while panel still showing
void freeDemoCodePanel();




#include "minorGems/graphics/openGL/gui/GUIPanelGL.h"
#include "minorGems/graphics/openGL/gui/GUITranslatorGL.h"
#include "minorGems/graphics/openGL/gui/TextGL.h"
#include "minorGems/graphics/openGL/gui/TextFieldGL.h"
#include "minorGems/graphics/openGL/gui/LabelGL.h"
#include "minorGems/graphics/Color.h"
#include "minorGems/util/SimpleVector.h"
#include "minorGems/util/log/AppLog.h"

#include "minorGems/game/game.h"


#include "DemoCodeChecker.h"


int codeCheckerWidth = 320;

int demoCodeLength = 10;

ScreenGL *codeCheckerScreen;

GUIPanelGL *codeCheckerMainPanel;
GUITranslatorGL *codeCheckerGuiTranslator;
TextGL *codeCheckerTextGLFixedWidth;
TextGL *codeCheckerTextGL;

LabelGL *messageLabel;
TextFieldGL *enterDemoCodeField;


DemoCodeChecker *codeChecker = NULL;


// track old code checkers so that we can destroy them after they unblock
SimpleVector<DemoCodeChecker *> oldCodeCheckers;




static void setLabelString( LabelGL *inLabel, 
                            const char *inTranslationString,
                            double inScaleFactor = 1 ) {
    char *labelString =
        (char *)TranslationManager::translate( (char*)inTranslationString );
    
    inLabel->setText( labelString );

    TextGL *text = inLabel->getTextGL();    

    // 1:1 aspect ratio
    // we know font is 8 pixels wide/tall
    double height = inScaleFactor * 8;
    double width = height * strlen( labelString );
    
    double actualDrawWidth =
        height * text->measureTextWidth( labelString );

      
    double centerW = codeCheckerWidth / 2;
    
    double guiY = inLabel->getAnchorY();
    

    double labelX = centerW - 0.5 * actualDrawWidth;
    
    
    inLabel->setPosition( labelX,
                          guiY,
                          width,
                          height );
    }



static LabelGL *createLabel( 
    double inGuiY, 
    const char *inTranslationString,
    TextGL *inText = NULL ) {
    
    if( inText == NULL ) {
        inText = codeCheckerTextGL;
        }

    LabelGL *returnLabel =
        new LabelGL( 0, inGuiY, 0, 0, "", inText );
    
    setLabelString( returnLabel, inTranslationString );

    return returnLabel;
    }




class DemoCodePanelKeyboardHandler : public KeyboardHandlerGL {
    public:
        void keyPressed( unsigned char inKey, int inX, int inY ) {
            
            
            if( inKey == 13 && enterDemoCodeField->isFocused() ) {
                    
                // don't destroy this
                char *enteredCode = enterDemoCodeField->getText();
                
                if( strlen( enteredCode ) > 0 ) {
                    // disable further input
                    enterDemoCodeField->setEnabled( false );
                    enterDemoCodeField->setFocus( false );
                    enterDemoCodeField->lockFocus( false );
                    
                    setLabelString( messageLabel, 
                                    "checkingCode" );
                    
                    // save this for next time
                    SettingsManager::setSetting( "demoCode",
                                                 enteredCode );

                    // start 
                    codeChecker = new DemoCodeChecker( 
                        enteredCode,
                        getDemoCodeSharedSecret(),
                        getDemoCodeServerURL() );
                    }
                }
            else if( inKey == 'q' || inKey == 'Q' || inKey == 27 ) {
                // q or escape
                exit( 0 );
                }
            }


        char isFocused() {
            // always focused
            return true;
            }
        
		void specialKeyPressed( int inKey, int inX, int inY ) {
            }

		void keyReleased( unsigned char inKey, int inX, int inY ) {
            }

		void specialKeyReleased( int inKey, int inX, int inY ) {
            }
        
    };



DemoCodePanelKeyboardHandler *codeCheckerKeyHandler;



// common init for both demo code panel and write failed message
static void panelCommonInit( ScreenGL *inScreen, const char *inFontTGAFileName,
                             int inWidth, int inHeight ) {
    
    codeCheckerScreen = inScreen;
    
    codeCheckerWidth = inWidth;
    
    
    // load text font
    Image *fontImage = readTGAFile( inFontTGAFileName );

    if( fontImage == NULL ) {
        // default
        // blank font
        fontImage = new Image( 256, 512, 4, true );
        }

    codeCheckerTextGLFixedWidth = 
        new TextGL( fontImage,
                    // use alpha
                    true,
                    // fixed character width
                    true,
                    // extra space around each character
                    0,
                    // space is full char width
                    1.0 );

    codeCheckerTextGL = 
        new TextGL( fontImage,
                    // use alpha
                    true,
                    // variable character width
                    false,
                    // extra space around each character, one pixel
                    0.125,
                    // space is 4 pixels wide (out of 8)
                    0.5 );
    
    delete fontImage;

    
    

    codeCheckerMainPanel = new GUIPanelGL( 0, 0, codeCheckerWidth, 
                                           codeCheckerWidth,
                                           new Color( 0,
                                                      0,
                                                      0, 1.0 ) );

	codeCheckerGuiTranslator = new GUITranslatorGL( codeCheckerMainPanel, 
                                                    codeCheckerScreen, 
                                                    codeCheckerWidth );

    

	codeCheckerScreen->addSceneHandler( codeCheckerGuiTranslator );
    codeCheckerScreen->addKeyboardHandler( codeCheckerGuiTranslator );
    codeCheckerScreen->addMouseHandler( codeCheckerGuiTranslator );

    codeCheckerKeyHandler = new DemoCodePanelKeyboardHandler();
    
    codeCheckerScreen->addKeyboardHandler( codeCheckerKeyHandler );
    }





void showDemoCodePanel( ScreenGL *inScreen, const char *inFontTGAFileName,
                        int inWidth, int inHeight ) {


    panelCommonInit( inScreen, inFontTGAFileName, inWidth, inHeight );
    
    

    messageLabel = createLabel( 0.5 * codeCheckerWidth + 15, 
                                      "enterDemoCode" );
    codeCheckerMainPanel->add( messageLabel );


    // 1:1 aspect ratio
    // we know font is 8 pixels wide/tall
    
    double height = 8;
    double width = height * demoCodeLength;
                
    double centerW = codeCheckerWidth / 2;
    

    char *defaultCode = 
        SettingsManager::getStringSetting( "demoCode" );

    char *fieldDefault = (char*)"";
    if( defaultCode != NULL ) {
        fieldDefault = defaultCode;
        }


    enterDemoCodeField = new TextFieldGL( centerW - 0.5 * width,
                                          0.5 * codeCheckerWidth - 15,
                                          width,
                                          height,
                                          1,
                                          fieldDefault,
                                          codeCheckerTextGLFixedWidth,
                                          new Color( 0.75, 0.75, 0.75 ),
                                          new Color( 0.75, 0.75, 0.75 ),
                                          new Color( 0.15, 0.15, 0.15 ),
                                          new Color( 0.75, 0.75, 0.75 ),
                                          demoCodeLength,
                                          true );

    codeCheckerMainPanel->add( enterDemoCodeField );

    
    enterDemoCodeField->setEnabled( true );
    enterDemoCodeField->setFocus( true );
    enterDemoCodeField->lockFocus( true );
    


    if( defaultCode != NULL ) {

        enterDemoCodeField->setCursorPosition( strlen( defaultCode ) );
        

            
        // don't destroy this
        char *enteredCode = enterDemoCodeField->getText();
        
        if( strlen( enteredCode ) > 0 ) {

            // run with this code
            // disable further input
            enterDemoCodeField->setEnabled( false );
            enterDemoCodeField->setFocus( false );
            enterDemoCodeField->lockFocus( false );
            
            setLabelString( messageLabel, 
                            "checkingCode" );
            
            // start 
            codeChecker = new DemoCodeChecker( enteredCode,
                                               getDemoCodeSharedSecret(),
                                               getDemoCodeServerURL() );
            }
        
        delete [] defaultCode;
        }
    


    }



char isDemoCodePanelShowing() {    

    // process old codeCheckers to clear pending ops and destroy them
    for( int i=0; i<oldCodeCheckers.size(); i++ ) {
        DemoCodeChecker *c = *( oldCodeCheckers.getElement( i ) );
        
        if( c->step() == false ) {
            // done
            AppLog::info( "Destroying old codeChecker.\n" );
            
            delete c;

            oldCodeCheckers.deleteElement( i );
            
            i--;
            }
        }



    if( codeChecker != NULL ) {
        
        if( codeChecker->isError() ) {
            // re-enable input
            enterDemoCodeField->setEnabled( true );
            enterDemoCodeField->setFocus( true );
            enterDemoCodeField->lockFocus( true );
            
            
            char *message = codeChecker->getErrorString();
            
            setLabelString( messageLabel, message );
            
            oldCodeCheckers.push_back( codeChecker );
            
            codeChecker = NULL;
            }
        else {

            char checkerWorkLeft = codeChecker->step();

            if( ! checkerWorkLeft 
                && ! codeChecker->isError()
                && codeChecker->codePermitted() ) {
                
                // will have error if not permitted, which is handled above

                oldCodeCheckers.push_back( codeChecker );
                
                codeChecker = NULL;
                
                // hide/destroy panel
                freeDemoCodePanel();
                
                // done:  code approved
                return false;
                }
            }
        }
    
    // still showing
    return true;
    }



static void panelCommonFree() {
    codeCheckerScreen->removeSceneHandler( codeCheckerGuiTranslator );
    codeCheckerScreen->removeKeyboardHandler( codeCheckerGuiTranslator );
    codeCheckerScreen->removeMouseHandler( codeCheckerGuiTranslator );

    codeCheckerScreen->removeKeyboardHandler( codeCheckerKeyHandler );
    delete codeCheckerKeyHandler;
    

    // recursively deletes all GUI components
    delete codeCheckerGuiTranslator;

    delete codeCheckerTextGLFixedWidth;
    delete codeCheckerTextGL;
    }



void freeDemoCodePanel() {
    
    panelCommonFree();
    

    if( codeChecker != NULL ) {
        delete codeChecker;
        codeChecker = NULL;
        }

    for( int i=0; i<oldCodeCheckers.size(); i++ ) {
        delete *( oldCodeCheckers.getElement( i ) );
        }
    }







void showWriteFailedPanel( ScreenGL *inScreen, const char *inFontTGAFileName,
                           int inWidth, int inHeight ) {
    panelCommonInit( inScreen, inFontTGAFileName, inWidth, inHeight );

    
    messageLabel = createLabel( 0.5 * codeCheckerWidth, 
                                "writeFailed" );
    codeCheckerMainPanel->add( messageLabel );
    }



void freeWriteFailedPanel() {
    
    panelCommonFree();
    }

