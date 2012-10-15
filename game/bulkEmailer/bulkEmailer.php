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
    
        $query = "SELECT message_id, subject, body ".
            "FROM $be_tableNamePrefix"."messages ORDER BY send_time LIMIT 1";
        $result = be_queryDatabase( $query );

        $numRows = mysql_numrows( $result );

        if( $numRows == 1 ) {
            echo "Found a message\n";
            
            $foundMessage = true;
            
            $row = mysql_fetch_array( $result, MYSQL_ASSOC );


            $message_id = $row[ "message_id" ];
            $subject = $row[ "subject" ];
            $body = $row[ "body" ];

            $numLeftInBatch = $be_emailMaxBatchSize - $numSent;
        
            $query = "SELECT email, custom_data ".
                "FROM $be_tableNamePrefix"."recipients ".
                "WHERE message_id = $message_id LIMIT $numLeftInBatch";
            $result = be_queryDatabase( $query );

            $numRows = mysql_numrows( $result );


            if( $numRows > 0 ) {
                echo "Message has $numRows recipients waiting\n";
                for( $i=0; $i<$numRows; $i++ ) {
                    $email = mysql_result( $result, $i, "email" );
                    $custom_data = mysql_result( $result, $i, "custom_data" );

                    $customBody =
                        preg_replace('/%CUSTOM%/', $custom_data, $body );
                    
                    $success = be_sendEmail( $subject, $customBody, $email );

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
                }


            // check again here to see if we're done with this message
            // (so DONE message is included in same report email)

            $query = "SELECT COUNT(*) ".
                "FROM $be_tableNamePrefix"."recipients ".
                "WHERE message_id = $message_id;";
            $result = be_queryDatabase( $query );

            $numLeft = mysql_result( $result, 0, 0 );
            
                        
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
            

            $reportBody = $reportBody .
                "\n\nSubject:  $subject\n\nBody:\n$body\n\n";

            
            }
        }
    

    if( $numSent > 0 ) {
        be_sendEmail( "bulkEmailer Report:  Sent $numSent",
                      $reportBody, $be_reportToEmailAddress );
        }
    

    echo "Sent $numSent message\n";
    }



// sends a note email to a specific address
// returns TRUE on success
function be_sendEmail( $message_subject, $message_text, $inEmail ) {
        
    
    global $be_siteEmailAddress;
    $mailHeaders = "From: $be_siteEmailAddress";


    $mailSubject = $message_subject;
    
    $mailBody = $message_text ."\n\n";

    
    $result = mail( $inEmail,
                    $mailSubject,
                    $mailBody,
                    $mailHeaders );
    return $result;
    }













?>