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
function be_addMessage( $subject, $body, $recipientEmailArray,
                        $customDataArray ) {
    
    be_connectToDatabase();

    global $be_tableNamePrefix;

    $subject = mysql_real_escape_string( $subject );
    $body = mysql_real_escape_string( $body );
    
    
    $query = "INSERT INTO $be_tableNamePrefix"."messages ".
        "(subject, body, send_time) ".
        "VALUES( '$subject', '$body', CURRENT_TIMESTAMP );";
    be_queryDatabase( $query );

    $message_id = mysql_insert_id();

    $i = 0;
    foreach( $recipientEmailArray as $email ) {
        $custom_data = $customDataArray[$i];
        $i++;


        $custom_data = mysql_real_escape_string( $custom_data );
        
        $query = "INSERT INTO $be_tableNamePrefix"."recipients ".
        "VALUES( '$email', '$custom_data', $message_id );";
        be_queryDatabase( $query );
        }

    be_closeDatabase();
    }



    

?>