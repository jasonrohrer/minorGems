<?php

// include this file in PHP scripts that want to add emails
// to the bulkEmailer queue



include( "bulkEmailerCommon.php" );
include( "bulkEmailerSettings.php" );




// note that a connection to bulkEmailer's database will be opened
// and later closed by this function call

function be_addMessage( $subject, $body, $recipientEmailArray ) {
    be_connectToDatabase();

    global $be_tableNamePrefix;
    
    
    $query = "INSERT INTO $be_tableNamePrefix"."messages ".
        "(subject, body, send_time) ".
        "VALUES( '$subject', '$body', CURRENT_TIMESTAMP );";
    be_queryDatabase( $query );

    $message_id = mysql_insert_id();

    foreach( $recipientEmailArray as $email ) {
        $query = "INSERT INTO $be_tableNamePrefix"."recipients ".
        "VALUES( '$email', $message_id );";
        be_queryDatabase( $query );
        }

    be_closeDatabase();
    }



    

?>