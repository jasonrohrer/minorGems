<?php


// server-side implementation of 1024-bit-P Diffie-Hellman key exchange


// RFC5114 standardized 1024-bit group (P and G choice )
global $p1024;
$p1024 = "B10B8F96A080E01DDE92DE5EAE5D54EC52C99FBCFB06A3C6" .
"9A6A9DCA52D23B616073E28675A23D189838EF1E2EE652C0" .
"13ECB4AEA906112324975C3CD49B83BFACCBDD7D90C4BD70" .
"98488E9C219A73724EFFD6FAE5644738FAA31A4FF55BCCC0" .
"A151AF5F0DC8B4BD45BF37DF365C1A65E68CFDA76D4DA708" .
"DF1FB2BC2E4A4371";

global $g1024;
$g1024 =
"A4D1CBD5C3FD34126765A442EFB99905F8104DD258AC507F" .
"D6406CFF14266D31266FEA1E5C41564B777E690F5504F213" .
"160217B4B01B886A5E91547F9E2749F4D7FBD7D3B9A92EE1" .
"909D0D2263F80A76A6A24C087A091F531DBF0A0169B6A28A" .
"D662A4D18E73AFA32D779D5918D08BC8858F4DCEF97C2A24" .
"855E6EEB22B3B2E5";



// test
dh_generateKey("aa",$B, $S );





// client picks "a" and then sends us A, where
// A = g^a mod p

// We pick "b" and then generate B and S, where
// B = g^b mod p
// S = A^b mod p = (g^a)^b mod p

// We can now us S as an encryption key and send B to the client along
// with our encrypted message.

// To obtain S, client calculates
// S = B^a mod p = (g^b)^a mod p


// returns B and S in call-by-reference parameters
function dh_generateKey( $inA, &$outB, &$outS ) {
    global $p1024, $g1024;
    
    
    // pick our secret b
    $bBytes = dh_getRandomBytes( 128 );

    $bHex = bin2hex( $bBytes );

    echo "b = $bHex<br>";

    echo "length = ". strlen( $bHex );

    $bDec = bchexdec( $bHex );

    echo "<br>bDec = $bDec";
    
    $pDec = bchexdec( $p1024 );
    $gDec = bchexdec( $g1024 );

    $outBDec = bcpowmod( $gDec, $bDec, $pDec );

    $outB = bcdechex( $outBDec );

    $outSDec = bcpowmod( bchexdec( $inA ), $bDec, $pDec );

    $outS = bcdechex( $outSDec );


    echo "<br>B = $outB";
    echo "<br>S = $outS";

    
    }




// gets random bytes... assumes /dev/urandom exists (linux)
function dh_getRandomBytes( $inNumBytes ) {
    $randomDev = @fopen( '/dev/urandom', 'rb' );

    if( $randomDev === FALSE ) {
        echo "Failed to open /dev/urandom to get secure random bytes";
        die;
        }

    $bytes = @fread( $randomDev, $inNumBytes );

    @fclose( $randomDev );

    return $bytes;
    }

    



function bchexdec( $hex ) {
    if( strlen( $hex ) == 1) {
        return hexdec( $hex );
        }
    else {
        $remain = substr( $hex, 0, -1 );
        $last = substr( $hex, -1 );
        return bcadd( bcmul( 16, bchexdec($remain) ), hexdec( $last ) );
        }
    }



function bcdechex( $dec ) {
    $last = bcmod( $dec, 16 );
    $remain = bcdiv( bcsub( $dec, $last ), 16);
    
    if( $remain == 0 ) {
        return strtoupper( dechex( $last ) );
        }
    else {
        return bcdechex( $remain ) . strtoupper( dechex( $last ) );
        }
    }

    