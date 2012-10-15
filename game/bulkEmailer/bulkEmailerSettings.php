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

// for web-based admin access
$be_accessPassword = "secret";


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


?>