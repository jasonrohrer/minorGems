<?php


include( "bulkEmailerSettings.php" );


$be_mysqlLink;


// general-purpose functions in here, many copied from seedBlogs

/**
 * Connects to the database according to the database variables.
 */  
function be_connectToDatabase() {
    global $be_databaseServer,
        $be_databaseUsername, $be_databasePassword, $be_databaseName,
        $be_mysqlLink;
    
    
    $be_mysqlLink = mysql_connect( $be_databaseServer, $be_databaseUsername,
                                   $be_databasePassword )
        or be_operationError( "Could not connect to database server: " .
                              mysql_error() );
    
	mysql_select_db( $be_databaseName )
        or be_operationError( "Could not select $be_databaseName database: " .
                              mysql_error() );
    }


 
/**
 * Closes the database connection.
 */
function be_closeDatabase() {
    global $be_mysqlLink;

    mysql_close( $be_mysqlLink );
    }



/**
 * Queries the database, and dies with an error message on failure.
 *
 * @param $inQueryString the SQL query string.
 *
 * @return a result handle that can be passed to other mysql functions.
 */
function be_queryDatabase( $inQueryString ) {
    global $be_mysqlLink;

    if( gettype( $be_mysqlLink ) != "resource" ) {
        // not a valid mysql link?
        be_connectToDatabase();
        }
    
    $result = mysql_query( $inQueryString, $be_mysqlLink );
    
    if( $result == FALSE ) {

        $errorNumber = mysql_errno();
        
        // server lost or gone?
        if( $errorNumber == 2006 ||
            $errorNumber == 2013 ||
            // access denied?
            $errorNumber == 1044 ||
            $errorNumber == 1045 ||
            // no db selected?
            $errorNumber == 1046 ) {

            // connect again?
            be_closeDatabase();
            be_connectToDatabase();

            $result = mysql_query( $inQueryString, $be_mysqlLink )
                or be_operationError(
                    "Database query failed:<BR>$inQueryString<BR><BR>" .
                    mysql_error() );
            }
        else {
            // some other error (we're still connected, so we can
            // add log messages to database
            be_fatalError( "Database query failed:<BR>$inQueryString<BR><BR>" .
                           mysql_error() );
            }
        }
    
    return $result;
    }



/**
 * Checks whether a table exists in the currently-connected database.
 *
 * @param $inTableName the name of the table to look for.
 *
 * @return 1 if the table exists, or 0 if not.
 */
function be_doesTableExist( $inTableName ) {
    // check if our table exists
    $tableExists = 0;
    
    $query = "SHOW TABLES";
    $result = be_queryDatabase( $query );

    $numRows = mysql_numrows( $result );


    for( $i=0; $i<$numRows && ! $tableExists; $i++ ) {

        $tableName = mysql_result( $result, $i, 0 );
        
        if( $tableName == $inTableName ) {
            $tableExists = 1;
            }
        }
    return $tableExists;
    }



function be_log( $message ) {
    global $be_enableLog, $be_tableNamePrefix;

    if( $be_enableLog ) {
        $slashedMessage = mysql_real_escape_string( $message );
    
        $query = "INSERT INTO $be_tableNamePrefix"."log VALUES ( " .
            "'$slashedMessage', CURRENT_TIMESTAMP );";
        $result = be_queryDatabase( $query );
        }
    }



/**
 * Displays the error page and dies.
 *
 * @param $message the error message to display on the error page.
 */
function be_fatalError( $message ) {
    //global $errorMessage;

    // set the variable that is displayed inside error.php
    //$errorMessage = $message;
    
    //include_once( "error.php" );

    // for now, just print error message
    $logMessage = "Fatal error:  $message";
    
    echo( $logMessage );

    be_log( $logMessage );
    
    die();
    }



/**
 * Displays the operation error message and dies.
 *
 * @param $message the error message to display.
 */
function be_operationError( $message ) {
    
    // for now, just print error message
    echo( "ERROR:  $message" );
    die();
    }




// sends a note email to a specific address
// returns TRUE on success
function be_sendEmail( $message_subject, $message_text, $inEmail ) {
        
    
    $mailSubject = $message_subject;
    
    $mailBody = $message_text ."\n\n";

    
    $result = be_mail( $inEmail,
                       $mailSubject,
                       $mailBody );
    return $result;
    }





function be_mail( $inEmail,
                  $inSubject,
                  $inBody ) {
    
    global $be_useSMTP, $be_siteEmailAddress;

    if( $be_useSMTP ) {
        require_once "Mail.php";

        global $be_smtpHost, $be_smtpPort, $be_smtpUsername, $be_smtpPassword;

        $headers = array( 'From' => $be_siteEmailAddress,
                          'To' => $inEmail,
                          'Subject' => $inSubject );
        
        $smtp = Mail::factory( 'smtp',
                               array ( 'host' => $be_smtpHost,
                                       'port' => $be_smtpPort,
                                       'auth' => true,
                                       'username' => $be_smtpUsername,
                                       'password' => $be_smtpPassword ) );


        $mail = $smtp->send( $inEmail, $headers, $inBody );


        if( PEAR::isError( $mail ) ) {
            be_log( "Email send failed:  " .
                    $mail->getMessage() );
            return false;
            }
        else {
            return true;
            }
        }
    else {
        // raw sendmail
        $mailHeaders = "From: $be_siteEmailAddress";
        
        return mail( $inEmail,
                     $inSubject,
                     $inBody,
                     $mailHeaders );
        }
    }


?>