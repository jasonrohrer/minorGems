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
        or be_fatalError( "Could not connect to database server: " .
                          mysql_error() );
    
	mysql_select_db( $be_databaseName )
        or be_fatalError( "Could not select $be_databaseName database: " .
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
    
    $result = mysql_query( $inQueryString, $be_mysqlLink )
        or be_fatalError( "Database query failed:<BR>$inQueryString<BR><BR>" .
                       mysql_error() );

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



?>