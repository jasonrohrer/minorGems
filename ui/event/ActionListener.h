/*
 * Modification History
 *
 * 2001-September-15		Jason Rohrer
 * Created.
 *
 * 2006-July-3		Jason Rohrer
 * Fixed warnings.
 */
 
 
#ifndef ACTION_LISTENER_INCLUDED
#define ACTION_LISTENER_INCLUDED 


#include "minorGems/ui/GUIComponent.h"


/**
 * An interface for a class that can handle a GUI action.
 *
 * @author Jason Rohrer
 */
class ActionListener {


	public:

        
        virtual ~ActionListener();

        
		/**
		 * Tells this class that an action has been performed.
		 *
		 * @param inTarget the GUI component on which the action
		 *   is being performed.
		 */
		virtual void actionPerformed( GUIComponent *inTarget ) = 0;

		
	};



inline ActionListener::~ActionListener() {
    }



#endif



