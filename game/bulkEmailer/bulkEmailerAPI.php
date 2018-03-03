<?php

// include this file in PHP scripts that want to add emails
// to the bulkEmailer queue



include( "bulkEmailerCommon.php" );
include( "bulkEmailerSettings.php" );




// note that a connection to bulkEmailer's database will be opened
// and later closed by this function call

// $customDataArray is one data element for each email address
// (might be all "") that will be plugged into $body text to replace
// each occurrence of the %CUSTOM% tag.

// $custom2DataArray is one data element for each email address
// (might be all "") that will be plugged into $body text to replace
// each occurrence of the %CUSTOM2% tag.
function be_addMessage( $subject, $body, $recipientEmailArray,
                        $customDataArray, $custom2DataArray ) {

    global $be_disableBatches, $be_instantSendTrans;

    $num = count( $recipientEmailArray );
    
    if( $be_disableBatches ||
        ( $num == 1 && $be_instantSendTrans ) ) {
        // alternative, send all emails NOW

        
        
        for( $i=0; $i<$num; $i++ ) {
            $email = $recipientEmailArray[$i];
            $custom_data = $customDataArray[$i];
            $custom2_data = $custom2DataArray[$i];

            $customBody =
                preg_replace('/%CUSTOM%/', $custom_data, $body );
            $customBody =
                preg_replace('/%CUSTOM2%/', $custom2_data, $body );

            $trans = false;
            if( $num == 1 ) {
                $trans = true;
                }
            
            $success = be_sendEmail( $subject, $customBody, $email,
                                     $trans );
            }


        return;
        }

    // else add them to database to send in batches later


    
    be_connectToDatabase();

    global $be_tableNamePrefix, $be_mysqlLink;

    $subject = mysqli_real_escape_string( $be_mysqlLink, $subject );
    $body = mysqli_real_escape_string( $be_mysqlLink, $body );
    
    
    $query = "INSERT INTO $be_tableNamePrefix"."messages ".
        "(subject, body, send_time) ".
        "VALUES( '$subject', '$body', CURRENT_TIMESTAMP );";
    be_queryDatabase( $query );

    $message_id = mysqli_insert_id( $be_mysqlLink );

    $i = 0;
    foreach( $recipientEmailArray as $email ) {
        $custom_data = $customDataArray[$i];
        $custom2_data = $custom2DataArray[$i];
        $i++;


        $custom_data = mysqli_real_escape_string( $be_mysqlLink, $custom_data );
        
        $query = "INSERT INTO $be_tableNamePrefix"."recipients ".
            "VALUES( '$email', '$custom_data', '$custom2_data', $message_id ) ".
            "ON DUPLICATE KEY UPDATE email = '$email';";
        be_queryDatabase( $query );
        }

    be_closeDatabase();
    }



    

?>