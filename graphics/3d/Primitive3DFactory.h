/*
 * Modification History
 *
 * 2001-March-14   Jason Rohrer
 * Created.
 *
 * 2001-April-1   Jason Rohrer
 * Added subreal entity vane class to factory. 
 */

#ifndef PRIMITIVE_3D_FACTORY_INCLUDED
#define PRIMITIVE_3D_FACTORY_INCLUDED 

#include "Primitive3D.h"

// Run-time type identification interface (RTTI)
#include <typeinfo>


// include these primitives only if we are part of subreal
#ifdef SUBREAL
#include "subreal/entity/EntityBodyPrimitive3D.h"
#define FACTORY_ENTITY_BODY_PRIMITIVE_FLAG 1
#include "subreal/entity/EntityVanePrimitive3D.h"
#define FACTORY_ENTITY_VANE_PRIMITIVE_FLAG 2

#endif

// the default flag
#define FACTORY_DEFAULT_PRIMITIVE_FLAG 0


/**
 * Class that maps Primitive3D integer subclass type flags to Primitive3D
 * subclass instances.
 *
 * Motivation for this class:
 * We want to extend Object3D to support subclass typed serialization
 * and deserialization without placing too much of a burden on
 * future Object3D subclasses.  For instance, we don't want subclasses
 * to have to write their own de/serialization functions so that
 * particular Primitive3D subclasses are serialized correctly.
 * We can avoid this burden by writing the base Object3D serialization
 * code so that it uses this factory to transmit subclasses with
 * type informaton.
 *
 * @author Jason Rohrer
 */
class Primitive3DFactory {
	public:

		/**
		 * Finds an integer subclass type flag for a primitive instance.
		 *
		 * @param inPrimitive the primitive to determine a flag for.  Must
		 *   be destroyed by the caller.
		 *
		 * @return a type flag for inObject.  0 (the default Object3D
		 *   baseclass flag) will be returned if subclass determination fails.
		 */
		static int primitive3DToInt( Primitive3D *inPrimitive );

		
		/**
		 * Constructs a new, unitialized Primitive3D (in other words,
		 * an Primitive3D ready for deserialization) of a subclass
		 * type matching inTypeFlag.
		 *
		 * @param inTypeFlag the type flag specifying the class
		 *   of the returned primitive.
		 *
		 * @return an (unitialized) Primitive3D class instance with a
		 *   subclass type corresponding to inTypeFlag.  If no
		 *   matching class is found, a default Primitive3D baseclass
		 *   instance is returned.  Must be destroyed by the caller. 
		 */
		static Primitive3D *intToPrimitive3D( int inTypeFlag );
		
	};



inline int Primitive3DFactory::primitive3DToInt( Primitive3D *inPrimitive ) {

	// use RTTI to determine type of inPrimitive
	
#ifdef SUBREAL
	if( typeid( *inPrimitive ) == typeid( EntityBodyPrimitive3D ) ) {
		return FACTORY_ENTITY_BODY_PRIMITIVE_FLAG;
		}
	else if( typeid( *inPrimitive ) == typeid( EntityVanePrimitive3D ) ) {
		return FACTORY_ENTITY_VANE_PRIMITIVE_FLAG;
		}
#endif

	// else return the default flag
	return FACTORY_DEFAULT_PRIMITIVE_FLAG;
	}



inline Primitive3D *Primitive3DFactory::intToPrimitive3D( int inTypeFlag ) {
	switch( inTypeFlag ) {
		case FACTORY_DEFAULT_PRIMITIVE_FLAG:
			return new Primitive3D();
			break;

			/* these primitives are only defined if
			 * we are part of subreal
			 */
#ifdef SUBREAL
		case FACTORY_ENTITY_BODY_PRIMITIVE_FLAG:
			return new EntityBodyPrimitive3D();
			break;
		case FACTORY_ENTITY_VANE_PRIMITIVE_FLAG:
			return new EntityVanePrimitive3D();
			break;
#endif

		default:
			// unknown primitive flag type
			return new Primitive3D();
			break;
		}
			
	}


#endif
