

// shows panel until a correct code has been entered
// assumes font TGA in "graphics" folder
void showDemoCodePanel( ScreenGL *inScreen, char *inFontTGAFileName );

// use this to check if demo checking done
char isDemoCodePanelShowing();


// call if app exits while panel still showing
void freeDemoCodePanel();




#include "minorGems/graphics/openGL/gui/GUIPanelGL.h"
#include "minorGems/graphics/openGL/gui/GUITranslatorGL.h"
#include "minorGems/graphics/openGL/gui/TextGL.h"
#include "minorGems/graphics/openGL/gui/TextFieldGL.h"


int codeCheckerWidth = 320;

ScreenGL *codeCheckerScreen;

GUIPanelGL *codeCheckerMainPanel;
GUITranslatorGL *codeCheckerGuiTranslator;
TextGL *codeCheckerTextGLFixedWidth;

LabelGL *enterDemoCodeLabel;
TextFieldGL *enterDemoCodeField;







static void setLabelString( LabelGL *inLabel, 
                            const char *inTranslationString,
                            double inScaleFactor ) {
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

      
    double centerW = gameWidth / 2;
    
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
    TextGL *inText ) {
    
    if( inText == NULL ) {
        inText = mTextGL;
        }

    LabelGL *returnLabel =
        new LabelGL( 0, inGuiY, 0, 0, "", inText );
    
    setLabelString( returnLabel, inTranslationString );

    return returnLabel;
    }







void showDemoCodePanel( ScreenGL *inScreen, char *inFontTGAFileName ) {

    codeCheckerScreen = inScreen;
    
    
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



    enterDemoCodeLabel = createLabel( 175, "enterDemoCode" );
    codeCheckerMainPanel->add( enterDemoCodeLabel );


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


    // FIXME:  direct copy from SID, variable names need changing

    mEnterDemoCodeField = new TextFieldGL( centerW - 0.5 * width,
                                           145,
                                           width,
                                           height,
                                           1,
                                           fieldDefault,
                                           mTextGLFixedWidth,
                                           new Color( 0.75, 0.75, 0.75 ),
                                           new Color( 0.75, 0.75, 0.75 ),
                                           new Color( 0.15, 0.15, 0.15 ),
                                           new Color( 0.75, 0.75, 0.75 ),
                                           demoCodeLength,
                                           true );

    mDemoCodePanel->add( mEnterDemoCodeField );


    if( demoMode ) {
        mEnterDemoCodeField->setEnabled( true );
        mEnterDemoCodeField->setFocus( true );
        mEnterDemoCodeField->lockFocus( true );
        }



    if( defaultCode != NULL ) {

        mEnterDemoCodeField->setCursorPosition( strlen( defaultCode ) );
        

        if( demoMode ) {
            
            // don't destroy this
            char *enteredCode = mEnterDemoCodeField->getText();
            
            if( strlen( enteredCode ) > 0 ) {

                // run with this code
                // disable further input
                mEnterDemoCodeField->setEnabled( false );
                mEnterDemoCodeField->setFocus( false );
                mEnterDemoCodeField->lockFocus( false );
                
                setLabelString( mEnterDemoCodeLabel, 
                                "checkingCode" );
                
                // start 
                codeChecker = new DemoCodeChecker( enteredCode );
                }
            }
        

        delete [] defaultCode;
        }
    


    }



char isDemoCodePanelShowing() {
    return true;
    }



void freeDemoCodePanel() {

    codeCheckerScreen->removeSceneHandler( codeCheckerGuiTranslator );
    codeCheckerScreen->removeKeyboardHandler( codeCheckerGuiTranslator );
    codeCheckerScreen->removeMouseHandler( codeCheckerGuiTranslator );

    // recursively deletes all GUI components
    delete codeCheckerGuiTranslator;

    delete codeCheckerTextGLFixedWidth;
    }
