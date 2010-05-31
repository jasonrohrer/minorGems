/*
 * Modification History
 *
 * 2000-December-21		Jason Rohrer
 * Created. 
 */
 
 
#ifndef MULTI_FILTER_INCLUDED
#define MULTI_FILTER_INCLUDED

#include "minorGems/graphics/ChannelFilter.h" 
#include "minorGems/util/SimpleVector.h" 
 
/**
 * Filter that applies a collection of filters.
 *
 * @author Jason Rohrer 
 */
class MultiFilter : public ChannelFilter { 
	
	public:
		
		/**
		 * Constructs a MultiFilter.
		 */
		MultiFilter();
		
		
		~MultiFilter();
		
		
		/**
		 * Adds a filter to end of this MultiFilter.  Filters
		 * are applied in the order in which they are added.
		 *
		 * @param inFilter filter to add.  Is not
		 *   destroyed when the MultiFilter is destroyed.
		 */
		void addFilter( ChannelFilter *inFilter );
		
		
		/**
		 * Removes a filter.
		 *
		 * @param inFilter filter to remove.
		 */
		void removeFilter( ChannelFilter *inFilter );
		
		
		// implements the ChannelFilter interface
		void apply( double *inChannel, int inWidth, int inHeight );
	
	
	private:
		SimpleVector<ChannelFilter*> *mFilterVector;

	};
	


inline MultiFilter::MultiFilter() 
	: mFilterVector( new SimpleVector<ChannelFilter*>() ) {

	}



inline MultiFilter::~MultiFilter() {
	delete mFilterVector;
	}
	
		
	
inline void MultiFilter::apply( double *inChannel, 
	int inWidth, int inHeight ) {
	
	int numFilters = mFilterVector->size();
	for( int i=0; i<numFilters; i++ ) {
		ChannelFilter *thisFilter = *( mFilterVector->getElement( i ) );
		thisFilter->apply( inChannel, inWidth, inHeight );
		}
	}
	
	
	
#endif
