/*
 * Modification History
 *
 * 2010-March-23   Jason Rohrer
 * Created.
 *
 * 2010-May-14    Jason Rohrer
 * String parameters as const to fix warnings.
 */

#ifndef PORT_MAPPING_INCLUDED
#define PORT_MAPPING_INCLUDED



/**
 * Maps a port on a UPNP Gateway Device (discovers it first).
 *
 * @param inPort the port to map.
 * @param inDescription the application description to log on the gateway.
 * @param inTimeoutMS timeout to use for UPNP discovery process.
 * @param outExternalIP pointer to where external IP of the gateway should
 *   be returned.  Populated with a newly-allocated string, or NULL on failure.
 *   Destroyed by caller.
 *
 * @return 1 on success.
 */
int mapPort( int inPort, const char *inDescription,
             int inTimeoutMS,
             char **outExternalIP );



/**
 * Maps a port on a UPNP Gateway Device (discovers it first).
 *
 * @param inPort the port to close on the gateway.
 * @param inTimeoutMS timeout to use for UPNP discovery process.
 *
 * @return 1 on success.
 */
int unmapPort( int inPort, int inTimeoutMS );




#endif
