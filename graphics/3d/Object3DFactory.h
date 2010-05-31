/*
 * Modification History
 *
 * 2001-March-14   Jason Rohrer
 * Created.
 *
 * 2001-March-17   Jason Rohrer
 * Finished implementation.
 *
 * 2001-April-1   Jason Rohrer
 * Fixed flag name.
 */

#ifndef OBJECT_3D_FACTORY_INCLUDED
#define OBJECT_3D_FACTORY_INCLUDED 

#include "Object3D.h"

// Run-time type identification interface (RTTI)
#include <typeinfo>


// include these objects only if we are part of subreal
#ifdef SUBREAL
#include "subreal/entity/EntityObject3D.h"
#define FACTORY_ENTITY_OBJECT_FLAG 1

#endif

// the default flag
#define FACTORY_DEFAULT_OBJECT_FLAG 0


/**
 * Class that maps Object3D integer subclass type flags to Object3D
 * subclass instances.
 *
 * Motivation for this class:
 * We have Object3D instances that exist simultaneously on opposite
 * sides of the network (at both the client and server ends).  Initially,
 * we just let the objects get sent to the server via the base class
 * Object3D serialization function.  At that time, subclasses of Object3D
 * did nothing more than init an Object3D in a particular way.  This
 * worked fine when we didn't have animations associated with various
 * Object3D subclasses, but doesn't work now that we have animation
 * code in various subclasses.  During serialization (if the object
 * is sent as a generic Object3D), the animation code is lost on
 * the client end.  We need a way for the client to pick the correct
 * subclass to construct for deserialization.  We can encode the
 * various subtypes by integers, and then this factory class can
 * be used to construct a class of appropriate subtype before
 * deserialization.
 *
 * @author Jason Rohrer
 */
class Object3DFactory {
	public:

		/**
		 * Finds an integer subclass type flag for an object instance.
		 *
		 * @param inObject the object to determine a flag for.  Must
		 *   be destroyed by the caller.
		 *
		 * @return a type flag for inObject.  0 (the defautl Object3D
		 *   baseclass flag) will be returned if subclass determination fails.
		 */
		static int object3DToInt( Object3D *inObject );

		
		/**
		 * Constructs a new, unitialized Object3D (in other words,
		 * an Object3D ready for deserialization) of a subclass
		 * type matching inTypeFlag.
		 *
		 * @param inTypeFlag the type flag specifying the class
		 *   of the returned object.
		 *
		 * @return an (unitialized) Object3D class instance with a
		 *   subclass type corresponding to inTypeFlag.  If no
		 *   matching class is found, a default Object3D baseclass
		 *   instance is returned.  Must be destroyed by the caller. 
		 */
		static Object3D *intToObject3D( int inTypeFlag );
		
	};



inline int Object3DFactory::object3DToInt( Object3D *inObject ) {

	// use RTTI to determine type of inObject
	
#ifdef SUBREAL
	if( typeid( *inObject ) == typeid( EntityObject3D ) ) {
		return FACTORY_ENTITY_OBJECT_FLAG;
		}
#endif
	// else return the default flag
	return FACTORY_DEFAULT_OBJECT_FLAG;
	}



inline Object3D *Object3DFactory::intToObject3D( int inTypeFlag ) {
	switch( inTypeFlag ) {
		case FACTORY_DEFAULT_OBJECT_FLAG:
			return new Object3D();
			break;

			/* these objects are only defined if
			 * we are part of subreal
			 */
#ifdef SUBREAL
		case FACTORY_ENTITY_OBJECT_FLAG:
			return new EntityObject3D();
			break;
#endif

		default:
			// unknown object flag type
			return new Object3D();
			break;
		}
			
	}


#endif
