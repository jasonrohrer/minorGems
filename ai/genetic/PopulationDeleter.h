/*
 * Modification History
 *
 * 2000-October-23		Jason Rohrer
 * Created.
 */
 
#ifndef POPULATION_DELETER_INCLUDED
#define POPULATION_DELETER_INCLUDED

#include "Crossbreedable.h"
 
/**
 * Interface for a class that can delete a particular crossbreedable
 * population member.  Needed for the generic implmentation of PopulationBreeder
 * because we can't delete memory referenced by Crossbreedable* pointers
 * (since the actual object type is a subclass of Crossbreedable).
 * Thus, for PopulationBreeder to work (which requires that it delete
 * bottom population members), the caller must provide it with a deletion
 * function.
 */
class PopulationDeleter {
 
	public:
		/**
		 * Deletes the object pointed to by a Crossbreedable*.
		 * 
		 * @param inMember the object that needs to be deleted.
		 */
		//virtual void deleteCrossbreedable( Crossbreedable* inMember ) = 0;
		virtual void deleteCrossbreedable( int inMemberIndex ) = 0;
		
		/**
		 * Adds a new member to the population.
		 * 
		 * @param inMemberIndex index where new member should be added.
		 * @param inNewMember pointer to new member.
		 */
		virtual void addCrossbreedable( int inMemberIndex, 
			void *inNewMember ) = 0;
	};

#endif
