/*
 * Modification History
 *
 * 2002-May-25   Jason Rohrer
 * Changed to use minorGems endian.h
 * Added a function for hashing an entire string to a hex digest.
 * Added a function for getting a raw digest.
 *
 * 2003-September-15   Jason Rohrer
 * Added support for hashing raw (non-string) data.
 * Removed legacy C code.
 *
 * 2013-January-7   Jason Rohrer
 * Added HMAC-SHA1 implementation.
 */



/*
 * sha.h
 *
 * Originally taken from the public domain SHA1 implementation
 * written by by Steve Reid <steve@edmweb.com>
 * 
 * Modified by Aaron D. Gifford <agifford@infowest.com>
 *
 * NO COPYRIGHT - THIS IS 100% IN THE PUBLIC DOMAIN
 *
 * The original unmodified version is available at:
 *    ftp://ftp.funet.fi/pub/crypt/hash/sha/sha1.c
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR(S) AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR(S) OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifndef __SHA1_H__
#define __SHA1_H__


#include "minorGems/system/endian.h"

    
/* Make sure you define these types for your architecture: */
typedef unsigned int sha1_quadbyte;	/* 4 byte type */
typedef unsigned char sha1_byte;	/* single byte type */

/*
 * Be sure to get the above definitions right.  For instance, on my
 * x86 based FreeBSD box, I define LITTLE_ENDIAN and use the type
 * "unsigned long" for the quadbyte.  On FreeBSD on the Alpha, however,
 * while I still use LITTLE_ENDIAN, I must define the quadbyte type
 * as "unsigned int" instead.
 */

#define SHA1_BLOCK_LENGTH	64
#define SHA1_DIGEST_LENGTH	20

/* The SHA1 structure: */
typedef struct _SHA_CTX {
	sha1_quadbyte	state[5];
	sha1_quadbyte	count[2];
	sha1_byte	buffer[SHA1_BLOCK_LENGTH];
} SHA_CTX;




// BIG NOTE:
// These overwrite the data!
// copy your data to a temporary buffer before passing it through
// (the "nicer" functions below these 3 do not overwrite data)
void SHA1_Init(SHA_CTX *context);
void SHA1_Update(SHA_CTX *context, sha1_byte *data, unsigned int len);
void SHA1_Final(sha1_byte digest[SHA1_DIGEST_LENGTH], SHA_CTX* context);



/**
 * Computes a unencoded 20-byte digest from data.
 *
 * @param inData the data to hash.
 *   Must be destroyed by caller.
 * @param inDataLength the length of the data to hash.
 *   Must be destroyed by caller.
 *
 * @return the digest as a byte array of length 20.
 *   Must be destroyed by caller.
 */
unsigned char *computeRawSHA1Digest( unsigned char *inData, int inDataLength );



/**
 * Computes a unencoded 20-byte digest from an arbitrary string message.
 *
 * @param inString the message as a \0-terminated string.
 *   Must be destroyed by caller.
 *
 * @return the digest as a byte array of length 20.
 *   Must be destroyed by caller.
 */
unsigned char *computeRawSHA1Digest( char *inString );


    
/**
 * Computes a hex-encoded string digest from data.
 *
 * @param inData the data to hash.
 *   Must be destroyed by caller.
 * @param inDataLength the length of the data to hash.
 *   Must be destroyed by caller.
 *
 * @return the digest as a \0-terminated string.
 *   Must be destroyed by caller.
 */
char *computeSHA1Digest( unsigned char *inData, int inDataLength );


    
/**
 * Computes a hex-encoded string digest from an arbitrary string message.
 *
 * @param inString the message as a \0-terminated string.
 *   Must be destroyed by caller.
 *
 * @return the digest as a \0-terminated string.
 *   Must be destroyed by caller.
 */
char *computeSHA1Digest( char *inString );



// computes SHA-1 based HMAC as defined in RFC 2104
char *hmac_sha1( const char *inKey, const char *inData );



#endif

