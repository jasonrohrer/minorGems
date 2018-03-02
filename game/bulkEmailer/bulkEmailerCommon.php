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
    
    
    $be_mysqlLink = mysqli_connect( $be_databaseServer, $be_databaseUsername,
                                    $be_databasePassword )
        or be_operationError( "Could not connect to database server: " .
                              mysqli_error( $be_mysqlLink ) );
    
	mysqli_select_db( $be_mysqlLink, $be_databaseName )
        or be_operationError( "Could not select $be_databaseName database: " .
                              mysqli_error( $be_mysqlLink ) );
    }


 
/**
 * Closes the database connection.
 */
function be_closeDatabase() {
    global $be_mysqlLink;

    mysqli_close( $be_mysqlLink );
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

    if( gettype( $be_mysqlLink ) != "object" ) {
        // not a valid mysql link?
        be_connectToDatabase();
        }
    
    $result = mysqli_query( $be_mysqlLink, $inQueryString );
    
    if( $result == FALSE ) {

        $errorNumber = mysqli_errno( $be_mysqlLink );
        
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

            $result = mysqli_query( $be_mysqlLink, $inQueryString )
                or be_operationError(
                    "Database query failed:<BR>$inQueryString<BR><BR>" .
                    mysqli_error( $be_mysqlLink ) );
            }
        else {
            // some other error (we're still connected, so we can
            // add log messages to database
            be_fatalError( "Database query failed:<BR>$inQueryString<BR><BR>" .
                           mysqli_error( $be_mysqlLink ) );
            }
        }
    
    return $result;
    }



/**
 * Replacement for the old mysql_result function.
 */
function be_mysqli_result( $result, $number, $field=0 ) {
    mysqli_data_seek( $result, $number );
    $row = mysqli_fetch_array( $result );
    return $row[ $field ];
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

    $numRows = mysqli_num_rows( $result );


    for( $i=0; $i<$numRows && ! $tableExists; $i++ ) {

        $tableName = be_mysqli_result( $result, $i, 0 );
        
        if( $tableName == $inTableName ) {
            $tableExists = 1;
            }
        }
    return $tableExists;
    }



function be_log( $message ) {
    global $be_enableLog, $be_tableNamePrefix, $be_mysqlLink;

    if( $be_enableLog ) {
        $slashedMessage = mysqli_real_escape_string( $be_mysqlLink, $message );
    
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
function be_sendEmail( $message_subject, $message_text, $inEmail,
                       $inTrans = false ) {
        
    
    $mailSubject = $message_subject;
    
    $mailBody = $message_text ."\n\n";

    
    $result = be_mail( $inEmail,
                       $mailSubject,
                       $mailBody,
                       $inTrans );
    return $result;
    }





function be_mail( $inEmail,
                  $inSubject,
                  $inBody,
                  $inTrans ) {
    
    global $be_useSMTP, $be_siteEmailAddress;

    if( $be_useSMTP ) {
        require_once "Mail.php";

        global $be_smtpHost, $be_smtpPort, $be_smtpUsername, $be_smtpPassword;

        $headers = array( 'From' => $be_siteEmailAddress,
                          'To' => $inEmail,
                          'Subject' => $inSubject );
        
        $smtp;

        if( $inTrans ) {
            global $be_smtpHostTrans, $be_smtpPortTrans,
                $be_smtpUsernameTrans, $be_smtpPasswordTrans;

            $smtp = Mail::factory(
                'smtp',
                array ( 'host' => $be_smtpHostTrans,
                        'port' => $be_smtpPortTrans,
                        'auth' => true,
                        'username' => $be_smtpUsernameTrans,
                        'password' => $be_smtpPasswordTrans ) );
            }
        else {
            $smtp = Mail::factory( 'smtp',
                                   array ( 'host' => $be_smtpHost,
                                           'port' => $be_smtpPort,
                                           'auth' => true,
                                           'username' => $be_smtpUsername,
                                           'password' => $be_smtpPassword ) );
            }
        

        $mail = $smtp->send( $inEmail, $headers, $inBody );


        if( (new PEAR)->isError( $mail ) ) {
            echo "<br>Sending email failed: " . $mail->getMessage();
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