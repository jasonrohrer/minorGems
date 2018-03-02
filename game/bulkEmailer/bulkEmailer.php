<?php

// Invoked from the command line (not web!)

// Invoke with the single argument "setup" to create database tables.

// Example:

// php bulkEmailer.php setup


// When invoked without arguments, it process the email queue and sends
// out a batch of emails.  Intended to be invoked by cron to space out batches.


// edit bulkEmailerSettings.php to change script's settings
include( "bulkEmailerSettings.php" );


include( "bulkEmailerCommon.php" );



// enable verbose error reporting to detect uninitialized variables
error_reporting( E_ALL );


// connect to db once before all processing
be_connectToDatabase();



if( count( $argv ) > 1 ) {

    if( $argv[1] == "setup" ) {

        be_setupDatabase();
        }
    else {
        echo "Unknown command:  $argv[1]\n";
        }
    }
else {
    be_sendBatch();
    }




// done processing request (only functions below)
be_closeDatabase();





/**
 * Creates the database tables.
 */
function be_setupDatabase() {
    global $be_tableNamePrefix;

    $tableName = $be_tableNamePrefix . "log";
    if( ! be_doesTableExist( $tableName ) ) {

        // this table contains general info about the server
        // use INNODB engine so table can be locked
        $query =
            "CREATE TABLE $tableName(" .
            "entry TEXT NOT NULL, ".
            "entry_time DATETIME NOT NULL );";

        $result = be_queryDatabase( $query );

        echo "$tableName table created.\n";
        }
    else {
        echo "$tableName table already exists\n";
        }

    
    
    $tableName = $be_tableNamePrefix . "messages";
    if( ! be_doesTableExist( $tableName ) ) {

        // this table contains general info about each ticket
        $query =
            "CREATE TABLE $tableName(" .
            "message_id INT NOT NULL PRIMARY KEY AUTO_INCREMENT,".
            "subject TEXT NOT NULL," .
            "body TEXT NOT NULL," .
            "send_time DATETIME NOT NULL );";

        $result = be_queryDatabase( $query );

        echo "$tableName table created.\n";
        }
    else {
        echo "$tableName table already exists\n";
        }


    $tableName = $be_tableNamePrefix . "recipients";
    if( ! be_doesTableExist( $tableName ) ) {

        // this table contains general info about each ticket
        $query =
            "CREATE TABLE $tableName(" .
            "email CHAR(255) NOT NULL," .
            "custom_data TEXT NOT NULL,".
            "custom2_data TEXT NOT NULL,".
            "message_id INT NOT NULL,".
            "PRIMARY KEY( email, message_id ) );";
        
        $result = be_queryDatabase( $query );

        echo "$tableName table created.\n";
        }
    else {
        echo "$tableName table already exists\n";
        }
    }




function be_sendBatch() {
    global $be_tableNamePrefix,
        $be_emailMaxBatchSize, $be_reportToEmailAddress;

    $numSent = 0;
    
    $reportBody = "";
            
    $foundMessage = true;

    while( $foundMessage && $numSent < $be_emailMaxBatchSize ) {
        $foundMessage = false;


        // pick the message set that had messages sent the longest time
        // ago
        // (As messages are sent from a set, the send_time is updated).
        // Thus, we never starve a message set if we have concurrent sets.
        $query = "SELECT message_id, subject, body ".
            "FROM $be_tableNamePrefix"."messages ".
            "ORDER BY send_time ASC LIMIT 1";
        $result = be_queryDatabase( $query );

        $numRows = mysqli_num_rows( $result );

        if( $numRows == 1 ) {
            echo "Found a message\n";
            
            $foundMessage = true;
            
            $row = mysqli_fetch_array( $result, MYSQLI_ASSOC );


            $message_id = $row[ "message_id" ];
            $subject = $row[ "subject" ];
            $body = $row[ "body" ];

            $numLeftInBatch = $be_emailMaxBatchSize - $numSent;

            // make sure sending lag doesn't result in cron jobs running
            // in parallel, which can lead to double-sending

            // lock table, get list of recipients in batch, then
            // delete recipients from table BEFORE sending any
            be_queryDatabase( "LOCK TABLE $be_tableNamePrefix".
                              "recipients WRITE;" );
            
            $query = "SELECT email, custom_data, custom2_data ".
                "FROM $be_tableNamePrefix"."recipients ".
                "WHERE message_id = $message_id LIMIT $numLeftInBatch";
            $result = be_queryDatabase( $query );

            $numRows = mysqli_num_rows( $result );

            $query = "DELETE FROM $be_tableNamePrefix"."recipients ".
                "WHERE message_id = $message_id LIMIT $numLeftInBatch";
            be_queryDatabase( $query );

            be_queryDatabase( "UNLOCK TABLES;" );
            

            if( $numRows > 0 ) {
                echo "Message has $numRows recipients waiting\n";
                for( $i=0; $i<$numRows; $i++ ) {
                    $email = be_mysqli_result( $result, $i, "email" );
                    $custom_data =
                        be_mysqli_result( $result, $i, "custom_data" );
                    $custom2_data =
                        be_mysqli_result( $result, $i, "custom2_data" );

                    $customBody =
                        preg_replace('/%CUSTOM%/', $custom_data, $body );
                    $customBody =
                        preg_replace('/%CUSTOM2%/', $custom2_data,
                                     $customBody );
                    
                    $success = be_sendEmail( $subject, $customBody, $email,
                                             false );

                    if( $success ) {
                        $reportBody = $reportBody . "$email SUCCESS\n";
                        }
                    else {
                        $reportBody = $reportBody . "$email FAILED\n";
                        }

                    $query = "DELETE FROM $be_tableNamePrefix"."recipients ".
                        "WHERE message_id = $message_id AND email = '$email';";
                    be_queryDatabase( $query );

                    $numSent ++;
                    }

                // send_time tracks last time messages from this set were
                // sent
                // This allows us to juggle concurrent sets without
                // starving any of them (see ORDER BY above)
                $query = "UPDATE $be_tableNamePrefix"."messages ".
                    "SET send_time = CURRENT_TIMESTAMP ".
                    "WHERE message_id = '$message_id';";
                be_queryDatabase( $query );
                }


            // check again here to see if we're done with this message
            // (so DONE message is included in same report email)

            $query = "SELECT COUNT(*) ".
                "FROM $be_tableNamePrefix"."recipients ".
                "WHERE message_id = $message_id;";
            $result = be_queryDatabase( $query );

            $numLeft = be_mysqli_result( $result, 0, 0 );
            
                        
            if( $numLeft == 0 ) {
                echo "Done with the message, removing\n";
                
                // done with this message
                $reportBody = $reportBody .
                    "\nDONE sending to all recipients\n\n";
                $query = "DELETE ".
                    "FROM $be_tableNamePrefix"."messages ".
                    "WHERE message_id = $message_id;";
                be_queryDatabase( $query );
                }
            else {
                $reportBody = $reportBody .
                    "\nRemaining recipients:  $numLeft\n\n";
                }
            

            $reportBody = $reportBody .
                "\n\nSubject:  $subject\n\nBody:\n$body\n\n";

            
            }
        }
    

    if( $numSent > 0 ) {
        be_sendEmail( "bulkEmailer Report:  Sent $numSent",
                      $reportBody, $be_reportToEmailAddress, true );
        }
    

    echo "Sent $numSent message\n";
    }

















?>