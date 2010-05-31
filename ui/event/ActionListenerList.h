/*
 * Modification History
 *
 * 2001-September-15		Jason Rohrer
 * Created.
 *
 * 2001-September-17		Jason Rohrer
 * Fixed a missing include.
 *
 * 2006-July-3		Jason Rohrer
 * Fixed warnings.
 *
 * 2009-December-22  Jason Rohrer
 * New SimpleVector delete call.
 *
 * 2010-May-19  Jason Rohrer
 * Added call to check if a listener is on the list.
 */
 
 
#ifndef ACTION_LISTENER_LIST_INCLUDED
#define ACTION_LISTENER_LIST_INCLUDED 


#include "ActionListener.h"
#include "minorGems/ui/GUIComponent.h"
#include "minorGems/util/SimpleVector.h"


/**
 * A utility class to be subclassed by classes needing
 * to handle a list of action listeners.
 *
 * @author Jason Rohrer
 */
class ActionListenerList : protected SimpleVector<ActionListener*> {


	public:


        
        virtual ~ActionListenerList();
        

		
		/**
		 * Adds an action listener.
		 *
		 * @param inListener the listener to add.  Must 
		 *   be destroyed by caller after this class has been destroyed.
		 */
		virtual void addActionListener( ActionListener *inListener );



		/**
		 * Removes an action listener.
		 *
		 * @param inListener the listener to remove.  Must 
		 *   be destroyed by caller.
		 */
		virtual void removeActionListener( ActionListener *inListener );



		/**
		 * Checks if action listener added
		 *
		 * @param inListener the listener to look for.  Must 
		 *   be destroyed by caller.
         *
         * @return true iff listener is on the list
		 */
		virtual char isListening( ActionListener *inListener );



		/**
		 * Tells all registered listeners that an action has been
		 * performed.
		 *
		 * @param inTarget the GUI component on which the action
		 *   is being performed.
		 */
		virtual void fireActionPerformed( GUIComponent *inTarget );


		
	};



inline ActionListenerList::~ActionListenerList() {

    }



inline void ActionListenerList::addActionListener(
	ActionListener *inListener ) {
	
	push_back( inListener );
	}



inline void ActionListenerList::removeActionListener(
	ActionListener *inListener ) {

	deleteElementEqualTo( inListener );
	}



inline char ActionListenerList::isListening(
	ActionListener *inListener ) {

    int index = getElementIndex( inListener );
    
    if( index == -1 ) {
        return false;
        }
    else {
        return true;
        }
	}	



inline void ActionListenerList::fireActionPerformed( GUIComponent *inTarget ) {
	for( int i=0; i<size(); i++ ) {
		ActionListener *listener = *( getElement( i ) );
		listener->actionPerformed( inTarget );
		}
	}



#endif



