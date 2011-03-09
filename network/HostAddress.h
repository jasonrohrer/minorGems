/*
 * Modification History
 *
 * 2001-January-10		Jason Rohrer
 * Created.
 *
 * 2001-December-12		Jason Rohrer
 * Made serializable.
 * Changed the constructor signature.
 *
 * 2001-December-14     Jason Rohrer
 * Added a method for getting the local address.
 * Fixed several typos.
 * Added functions for testing equality and making copies.
 * Fixed a comment.
 *
 * 2002-February-25    Jason Rohrer
 * Made destructor virtual.
 *
 * 2002-March-25    Jason Rohrer
 * Added function for conversion to numerical addresses.
 * Changed equality function to resolve both addresses to
 * their numerical form first.
 * Changed equality function to be localhost savvy.
 *
 * 2002-March-26    Jason Rohrer
 * Added some print statements for testing.
 * Removed testing print statements.
 *
 * 2002-March-29    Jason Rohrer
 * Added Fortify inclusion.
 *
 * 2002-April-8    Jason Rohrer
 * Added an isNumerical function because hostname lookups on
 * numerical addresses are slow on Win32.
 *
 * 2002-April-11    Jason Rohrer
 * Fixed a bug in equals when both lookups fail.
 *
 * 2002-July-23    Jason Rohrer
 * Fixed a bug in equals when our local lookup fails.
 *
 * 2010-February-1    Jason Rohrer
 * Fixed a bug in getNumericalLocalAddress when our local lookup fails.
 *
 * 2011-March-9    Jason Rohrer
 * Removed Fortify inclusion.
 */

#include "minorGems/common.h"


#ifndef HOST_ADDRESS_CLASS_INCLUDED
#define HOST_ADDRESS_CLASS_INCLUDED

#include <stdio.h>

#include <string.h>

#include "minorGems/io/Serializable.h"
#include "minorGems/util/stringUtils.h"






/**
 * Address of a network host.
 *
 * @author Jason Rohrer
 */ 
class HostAddress : public Serializable {

	public:

		

		/**
		 * Constructs an uninitialized address.
		 *
		 * Note that all other functions (besides deserialize and the
		 * destructor) assume that the HostAddress is initialized.
		 *
		 * Useful prior to deserialization.
		 */
		HostAddress();

		
		
		/**
		 * Constructs an address.
		 *
		 * @param inAddressString a \0-terminated string containing the
		 *   host address. This string will be destroyed when this class
		 *   is destroyed, so it cannot be const.
		 * @param inPort the port of the host.
		 */
		HostAddress( char *inAddressString, int inPort );



		virtual ~HostAddress();



		/**
		 * Gets the address of the local host.
		 *
		 * @return the address of the local host.
		 *   Note that the mPort field in the returned HostAddress is
		 *   set to 0.
		 *   Must be destroyed by caller.
		 */
		static HostAddress *getLocalAddress();



        /**
         * Gets the address of the local host in numerical format.
		 *
		 * @return the address of the local host.
		 *   Note that the mPort field in the returned HostAddress is
		 *   set to 0.
		 *   Must be destroyed by caller.
		 */
        static HostAddress *getNumericalLocalAddress();



		/**
         * Gets a numerical version of this host address.
         *
         * For example, if we are using IPv4, this will generate a
         * HostAddress containing an aaa.bbb.ccc.ddd style address.
         *
         * @return a numerical version of this address, or NULL
         *   if address resolution fails.
         *   Must be destroyed by caller if non-NULL.
         */
        HostAddress *getNumericalAddress();



        /**
         * Gets whether this address is in numerical format.
         * For IpV4, this is xxx.xxx.xxx.xxx
         *
         * @return true if this address is numerical.
         */
        char isNumerical();

        

		/**
		 * Gets whether another address is equivalent to this address.
		 *
		 * @param inAddress the address to compare to this address.
		 *
		 * @return true iff inAddress is equivalent to this address.
		 */
		char equals( HostAddress *inAddress );



		/**
		 * Makes a copy of this host address.
		 *
		 * @return the copy of this address.
		 */
		HostAddress *copy();

		
		
		/**
		 * Prints this address to standard out.
		 */
		void print();



		// implement the Serializable interface
		virtual int serialize( OutputStream *inOutputStream );

		virtual int deserialize( InputStream *inInputStream );

		
		
		char *mAddressString;
		int mAddressLength;
		int mPort;
		
	};		



inline HostAddress::HostAddress( char *inAddressString, int inPort ) 
	: mAddressString( inAddressString ), mPort( inPort ) {
	
	mAddressLength = strlen( mAddressString );
	}



inline HostAddress::HostAddress() 
	: mAddressString( NULL ) {
	
	mAddressLength = 0;
	mPort = 0;
	}



inline HostAddress::~HostAddress() {
	if( mAddressString != NULL ) {
		delete [] mAddressString;
		}
	}



inline HostAddress *HostAddress::getNumericalLocalAddress() {
    HostAddress *address = getLocalAddress();

    if( address != NULL ) {
        
        HostAddress *numAddress = address->getNumericalAddress();
        
        delete address;

        return numAddress;
        }
    
    return NULL;
    }



inline char HostAddress::equals( HostAddress *inAddress ) {
    // if the port numbers are not equal, we can return
    // right away
    if( mPort != inAddress->mPort ) {
        return false;
        }

    // otherwise, the port numbers are equal,
    // so we need to compare the addresses
    
    // first, try to compare numercally looked-up addresses
    HostAddress *numericalThis = this->getNumericalAddress();

    if( numericalThis != NULL ) {

        HostAddress *numericalOther = inAddress->getNumericalAddress();

        if( numericalOther != NULL ) {

            char returnVal = false;
                        
            // watch out for localhost loopbacks
            
            if( !strcmp( numericalThis->mAddressString,
                         "127.0.0.1" ) ) {

                // this address is localhost

                // make sure other address is not our external local address

                HostAddress *localAddress = getNumericalLocalAddress();

                if( localAddress != NULL ) {
                    if( !strcmp( localAddress->mAddressString,
                                 numericalOther->mAddressString ) ) {

                        returnVal = true;
                        }

                    delete localAddress;
                    }
                else {
                    // numerical lookup failed for one but not the
                    // other, so assume addresses are different
                    returnVal = false;
                    }
                
                }
            else if( !strcmp( numericalOther->mAddressString,
                              "127.0.0.1" ) ) {
                // other address is localhost

                // make sure this address is not our external local address

                HostAddress *localAddress = getNumericalLocalAddress();
                if( localAddress != NULL ) {                
                    if( !strcmp( localAddress->mAddressString,
                                 numericalThis->mAddressString ) ) {
                        
                        returnVal = true;
                        }
                    
                    delete localAddress;                
                    }
                else {
                    // numerical lookup failed for one but not the
                    // other, so assume addresses are different
                    returnVal = false;
                    }
                }
            
            // if numerical addresses are identical,
            // then hosts are equal
            if( !strcmp( numericalThis->mAddressString,
                         numericalOther->mAddressString ) ) {
                
                returnVal = true;
                }
            delete numericalOther;
            delete numericalThis;

            
            return returnVal;
            }
        
        delete numericalThis;
        }

    // otherwise, if lookup fails, compare raw adddresses
    if( !strcmp( inAddress->mAddressString, mAddressString ) ) {
        return true;
        }
    else {
        return false;
        }
    }



inline HostAddress *HostAddress::copy() {
	char *stringCopy = new char[ strlen( mAddressString ) + 1 ];
	strcpy( stringCopy, mAddressString );

	return new HostAddress( stringCopy, mPort );
	}



inline void HostAddress::print() {
	printf( "%s:%d", mAddressString, mPort );
	}



inline int HostAddress::serialize( OutputStream *inOutputStream ) {
	int numTransmitted = 0;

	numTransmitted += inOutputStream->writeLong( (long)mAddressLength );

	numTransmitted +=inOutputStream->write(
		(unsigned char *)mAddressString, mAddressLength );
		
	numTransmitted += inOutputStream->writeLong( (long)mPort );

	return numTransmitted;
	}



inline int HostAddress::deserialize( InputStream *inInputStream ) {
	int numTransmitted = 0;

	long readLong;
	
	numTransmitted += inInputStream->readLong( &readLong );
	mAddressLength = (int)readLong;

	
	if( mAddressString != NULL ) {
		delete [] mAddressString;
		}

	mAddressString = new char[ mAddressLength ];
	
	numTransmitted +=inInputStream->read(
		(unsigned char *)mAddressString, mAddressLength );
		
	numTransmitted += inInputStream->readLong( &readLong );

	mPort = readLong;
	
	return numTransmitted;
	}


	
#endif
