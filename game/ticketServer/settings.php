<?php

// Basic settings
// You must set these for the server to work

$databaseServer = "localhost";
$databaseUsername = "testUser";
$databasePassword = "testPassword";
$databaseName = "test";

// The URL of to the server.php script.
$fullServerURL = "http://localhost/jcr13/ticketServer/server.php";


// The URL of the main, public-face website
$mainSiteURL = "http://sleepisdeath.net";



// End Basic settings



// Customization settings

// Adjust these to change the way the server  works.


// Prefix to use in table names (in case more than one application is using
// the same database).  Two tables are created:  "games" and "columns".
//
// If $tableNamePrefix is "test_" then the tables will be named
// "test_games" and "test_columns".  Thus, more than one server
// installation can use the same database (or the server can share a database
// with another application that uses similar table names).
$tableNamePrefix = "ticketServer_";


$enableLog = 1;

// for web-based admin access
$accessPassword = "secret";


// batch size for sending email to all opt-in users
// useful if your server has a "max emails per hour" limit
// (My server has a limit of 1000/hour)
$emailMaxBatchSize = 900;




// maps item purchase tags to dates in New York time
$allowedDownloadDates = array( "april_9" => "2010-04-09 00:00:00",
                               "april_12" => "2010-04-12 00:00:00" );

// secret shared with fastspring server, one per purchase tag from above
$fastspringPrivateKeys = array( "april_9" => "secret A",
                                "april_12" => "secret B" );


// files to serve, from path below
$fileList = array( "sueFamilyWeb.jpg", "SleepIsDeath_v2_UnixSource.tar.gz" );

$fileDescriptions = array(
    "Sue's family picture, from the archive.",
    "Unix (and Mac/Windows) Source Code (<A HREF=\"sourceCompileNotes.shtml\">notes</A>)" );



// should not be web-accessible
$downloadFilePath = "/home/jcr13/sidDownloads/";



// header and footers for various pages
$header = "include( \"header.php\" );";
$footer = "include( \"footer.php\" );";

$fileListHeader = $header .
"echo \"<center><font size=6>Downloads</font></center><br><br>\";";




// parameters for download emails that are sent out
$siteName = "Sleep Is Death";
$siteEmailAddress = "jcr13@cornell.edu";



// number of tickets shown per page in the browse view
$ticketsPerPage = 6;





?>