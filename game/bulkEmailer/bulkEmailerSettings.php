<?php

// Basic settings
// You must set these for the script to work

$be_databaseServer = "localhost";
$be_databaseUsername = "testUser";
$be_databasePassword = "testPassword";
$be_databaseName = "test";



// End Basic settings



// Customization settings

// Adjust these to change the way the server  works.


// Prefix to use in table names (in case more than one application is using
// the same database).
//
// If $tableNamePrefix is "test_" then the tables might be named
// "test_games" and "test_columns".  Thus, more than one server
// installation can use the same database (or the server can share a database
// with another application that uses similar table names).
$be_tableNamePrefix = "bulkEmailer_";


$be_enableLog = 1;


// if 1, then emails are NOT stored in database for batch sending, but instead
// are sent immediately as they are generated.
$be_disableBatches = 0;


// Should single, transactional emails be sent instantly instead of batched?
$be_instantSendTrans = 1;



// batch size for sending email
// useful if your server has a "max emails per hour" limit
// (My server has a limit of 500/hour)
// You can have cron invok this script multiple times per hour
// Example:  408 emails/hour, invoked every 5 minutes (12 batches per hour)
// 34 emails per batch
$be_emailMaxBatchSize = 34;


// parameters for emails that are sent out
$be_siteEmailAddress = "jcr13@cornell.edu";


// where batch reports are sent
$be_reportToEmailAddress = "jcr13@cornell.edu";




// SMTP settings

// if off, then raw sendmail is used instead 
$be_useSMTP = 0;

// SMTP requires that the PEAR Mail package is installed
// set the include path here for Mail.php, if needed:
/*
ini_set( 'include_path',
         '/home/jcr13/php' . PATH_SEPARATOR . ini_get( 'include_path' ) );
*/


$be_smtpHost = "ssl://mail.server.com";

$be_smtpPort = "465";

$be_smtpUsername = "jason@server.com";

$be_smtpPassword = "secret";


// an alternative SMTP for mission-critical, transactional emails
// we may want to use a separate service for these

// by default, use the same service

$be_smtpHostTrans = $be_smtpHost;

$be_smtpPortTrans = $be_smtpPort;

$be_smtpUsernameTrans = $be_smtpUsername;

$be_smtpPasswordTrans = $be_smtpPassword;



?>