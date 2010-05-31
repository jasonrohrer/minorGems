/*
 * Modification History
 *
 * 2000-September-13		Jason Rohrer
 * Created.
 *
 * 2000-September-17	Jason Rohrer
 * Added a virtual member function so dynamic_cast operator
 * would work.
 */

#ifndef TRAINING_EXAMPLE_INCLUDED
#define TRAINING_EXAMPLE_INCLUDED


/**
 * Abstract interface for examples used in training.
 *
 * @author Jason Rohrer 
 */
class TrainingExample {
	public:
		// need to include a virtual function so
		// that dynamic casts work
		virtual ~TrainingExample();
	};

inline TrainingExample::~TrainingExample() {
	// do nothing here.
	// Just included so dynamic casts would work.
	}
	
#endif
