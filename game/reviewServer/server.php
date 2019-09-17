<?php



global $rs_version;
$rs_version = "1";



// edit settings.php to change server' settings
include( "settings.php" );




// no end-user settings below this point





// no caching
//header('Expires: Mon, 26 Jul 1997 05:00:00 GMT');
header('Cache-Control: no-store, no-cache, must-revalidate');
header('Cache-Control: post-check=0, pre-check=0', FALSE);
header('Pragma: no-cache'); 



// enable verbose error reporting to detect uninitialized variables
error_reporting( E_ALL );



// page layout for web-based setup
$setup_header = "
<HTML>
<HEAD><TITLE>Review Server Web-based setup</TITLE></HEAD>
<BODY BGCOLOR=#FFFFFF TEXT=#000000 LINK=#0000FF VLINK=#FF0000>

<CENTER>
<TABLE WIDTH=75% BORDER=0 CELLSPACING=0 CELLPADDING=1>
<TR><TD BGCOLOR=#000000>
<TABLE WIDTH=100% BORDER=0 CELLSPACING=0 CELLPADDING=10>
<TR><TD BGCOLOR=#EEEEEE>";

$setup_footer = "
</TD></TR></TABLE>
</TD></TR></TABLE>
</CENTER>
</BODY></HTML>";






// ensure that magic quotes are OFF
// we hand-filter all _REQUEST data with regexs before submitting it to the DB
if( get_magic_quotes_gpc() ) {
    // force magic quotes to be removed
    $_GET     = array_map( 'rs_stripslashes_deep', $_GET );
    $_POST    = array_map( 'rs_stripslashes_deep', $_POST );
    $_REQUEST = array_map( 'rs_stripslashes_deep', $_REQUEST );
    $_COOKIE  = array_map( 'rs_stripslashes_deep', $_COOKIE );
    }
    


// Check that the referrer header is this page, or kill the connection.
// Used to block XSRF attacks on state-changing functions.
// (To prevent it from being dangerous to surf other sites while you are
// logged in as admin.)
// Thanks Chris Cowan.
function rs_checkReferrer() {
    global $fullServerURL;
    
    if( !isset($_SERVER['HTTP_REFERER']) ||
        strpos($_SERVER['HTTP_REFERER'], $fullServerURL) !== 0 ) {
        
        die( "Bad referrer header" );
        }
    }




// all calls need to connect to DB, so do it once here
rs_connectToDatabase();

// close connection down below (before function declarations)


// testing:
//sleep( 5 );


$maxNumAnswers = 10;

$answerNames = array();
$answerLetters = array( "A", "B", "C",
                        "D", "E", "F",
                        "G", "H", "I",
                        "J" );
for( $i=0; $i<$maxNumAnswers; $i++ ) {
    $answerNames[$i] = "answer" . $answerLetters[$i];
    }


// general processing whenver server.php is accessed directly




// grab POST/GET variables
$action = rs_requestFilter( "action", "/[A-Z_]+/i" );

$debug = rs_requestFilter( "debug", "/[01]/" );

$remoteIP = "";
if( isset( $_SERVER[ "REMOTE_ADDR" ] ) ) {
    $remoteIP = $_SERVER[ "REMOTE_ADDR" ];
    }




if( $action == "version" ) {
    global $rs_version;
    echo "$rs_version";
    }
else if( $action == "get_sequence_number" ) {
    rs_getSequenceNumber();
    }
else if( $action == "get_stats" ) {
    rs_getStats();
    }
else if( $action == "log_game" ) {
    rs_logGame();
    }
else if( $action == "submit_review" ) {
    rs_submitReview();
    }
else if( $action == "remove_review" ) {
    rs_removeReview();
    }
else if( $action == "update_html" ) {
    rs_updateHTML();
    }
else if( $action == "show_log" ) {
    rs_showLog();
    }
else if( $action == "clear_log" ) {
    rs_clearLog();
    }
else if( $action == "show_data" ) {
    rs_showData();
    }
else if( $action == "regen_static_html" ) {
    rs_regenStaticHTML();
    }
else if( $action == "show_detail" ) {
    rs_showDetail();
    }
else if( $action == "create_poll" ) {
    rs_createPoll();
    }
else if( $action == "delete_poll" ) {
    rs_deletePoll();
    }
else if( $action == "check_for_poll" ) {
    rs_checkForPoll();
    }
else if( $action == "poll_vote" ) {
    rs_pollVote();
    }
else if( $action == "list_polls" ) {
    rs_listPolls();
    }
else if( $action == "view_review" ) {
    rs_viewReview();
    }
else if( $action == "list_recent" ) {
    rs_listRecent();
    }
else if( $action == "list_playtime" ) {
    rs_listPlaytime();
    }
else if( $action == "logout" ) {
    rs_logout();
    }
else if( $action == "rs_setup" ) {
    global $setup_header, $setup_footer;
    echo $setup_header; 

    echo "<H2>Review Server Web-based Setup</H2>";

    echo "Creating tables:<BR>";

    echo "<CENTER><TABLE BORDER=0 CELLSPACING=0 CELLPADDING=1>
          <TR><TD BGCOLOR=#000000>
          <TABLE BORDER=0 CELLSPACING=0 CELLPADDING=5>
          <TR><TD BGCOLOR=#FFFFFF>";

    rs_setupDatabase();

    echo "</TD></TR></TABLE></TD></TR></TABLE></CENTER><BR><BR>";
    
    echo $setup_footer;
    }
else if( preg_match( "/server\.php/", $_SERVER[ "SCRIPT_NAME" ] ) ) {
    // server.php has been called without an action parameter

    // the preg_match ensures that server.php was called directly and
    // not just included by another script
    
    // quick (and incomplete) test to see if we should show instructions
    global $tableNamePrefix;
    
    // check if our tables exist
    $exists = rs_doesTableExist( $tableNamePrefix . "user_stats" ) &&
        rs_doesTableExist( $tableNamePrefix . "polls" ) &&
        rs_doesTableExist( $tableNamePrefix . "log" );
    
        
    if( $exists  ) {
        echo "Review Server database setup and ready";
        }
    else {
        // start the setup procedure

        global $setup_header, $setup_footer;
        echo $setup_header; 

        echo "<H2>Review Server Web-based Setup</H2>";
    
        echo "Review Server will walk you through a " .
            "brief setup process.<BR><BR>";
        
        echo "Step 1: ".
            "<A HREF=\"server.php?action=rs_setup\">".
            "create the database tables</A>";

        echo $setup_footer;
        }
    }



// done processing
// only function declarations below

rs_closeDatabase();







/**
 * Creates the database tables needed by seedBlogs.
 */
function rs_setupDatabase() {
    global $tableNamePrefix;

    $tableName = $tableNamePrefix . "log";
    if( ! rs_doesTableExist( $tableName ) ) {

        // this table contains general info about the server
        // use INNODB engine so table can be locked
        $query =
            "CREATE TABLE $tableName(" .
            "entry TEXT NOT NULL, ".
            "entry_time DATETIME NOT NULL );";

        $result = rs_queryDatabase( $query );

        echo "<B>$tableName</B> table created<BR>";
        }
    else {
        echo "<B>$tableName</B> table already exists<BR>";
        }

    
    
    $tableName = $tableNamePrefix . "user_stats";
    if( ! rs_doesTableExist( $tableName ) ) {

        // this table contains general info about each ticket
        $query =
            "CREATE TABLE $tableName(" .
            "id INT UNSIGNED NOT NULL PRIMARY KEY AUTO_INCREMENT," .
            "email VARCHAR(254) NOT NULL," .
            "UNIQUE KEY( email )," .
            "sequence_number INT NOT NULL," .
            "first_game_date DATETIME NOT NULL," .
            "last_game_date DATETIME NOT NULL," .
            "last_game_seconds INT NOT NULL," .
            "game_count INT NOT NULL," .
            "game_total_seconds INT NOT NULL," .
            "INDEX( game_total_seconds )," .
            // -1 if not submitted yet
            "review_score TINYINT NOT NULL," .
            "INDEX( review_score )," .
            "review_name VARCHAR(20) NOT NULL," .
            "review_text TEXT NOT NULL," .
            // stats about player's state when they posted the review
            // they may have played more games since the review
            "review_date DATETIME NOT NULL," .
            "INDEX( review_date )," .
            "review_game_seconds INT NOT NULL," .
            "review_game_count INT NOT NULL," .
            // in future, we may allow users to upvote/downvote reviews
            "review_votes INT NOT NULL,".
            "lives_since_recent_poll INT NOT NULL,".
            "seconds_lived_since_recent_poll INT NOT NULL, ".
            "recent_poll_answered TINYINT NOT NULL );";

        $result = rs_queryDatabase( $query );

        echo "<B>$tableName</B> table created<BR>";
        }
    else {
        echo "<B>$tableName</B> table already exists<BR>";
        }



    $tableName = $tableNamePrefix . "polls";
    if( ! rs_doesTableExist( $tableName ) ) {

        // this table contains general info about each ticket
        $query =
            "CREATE TABLE $tableName(" .
            "id INT UNSIGNED NOT NULL PRIMARY KEY AUTO_INCREMENT," .
            "post_date DATETIME NOT NULL," .
            "start_date DATETIME NOT NULL," .
            "end_date DATETIME NOT NULL," .
            "min_lives INT NOT NULL,".
            "min_lives_since_post_date INT NOT NULL,".
            "min_lived_seconds INT NOT NULL,".
            "min_lived_seconds_since_post_date INT NOT NULL,".
            "question TEXT NOT NULL," .
            // up to 10 answers
            // later answers are "" if there are fewer answers
            "answerA TEXT NOT NULL," .
            "answerB TEXT NOT NULL," .
            "answerC TEXT NOT NULL," .
            "answerD TEXT NOT NULL," .
            "answerE TEXT NOT NULL," .
            "answerF TEXT NOT NULL," .
            "answerG TEXT NOT NULL," .
            "answerH TEXT NOT NULL," .
            "answerI TEXT NOT NULL," .
            "answerJ TEXT NOT NULL," .
            "answerA_count int NOT NULL," .
            "answerB_count int NOT NULL," .
            "answerC_count int NOT NULL," .
            "answerD_count int NOT NULL," .
            "answerE_count int NOT NULL," .
            "answerF_count int NOT NULL," .
            "answerG_count int NOT NULL," .
            "answerH_count int NOT NULL," .
            "answerI_count int NOT NULL," .
            "answerJ_count int NOT NULL );";

        $result = rs_queryDatabase( $query );

        echo "<B>$tableName</B> table created<BR>";
        }
    else {
        echo "<B>$tableName</B> table already exists<BR>";
        }
    }



function rs_showLog() {
    rs_checkPassword( "show_log" );

     echo "[<a href=\"server.php?action=show_data" .
         "\">Main</a>]<br><br><br>";
    
    global $tableNamePrefix;

    $query = "SELECT * FROM $tableNamePrefix"."log ".
        "ORDER BY entry_time DESC;";
    $result = rs_queryDatabase( $query );

    $numRows = mysqli_num_rows( $result );



    echo "<a href=\"server.php?action=clear_log\">".
        "Clear log</a>";
        
    echo "<hr>";
        
    echo "$numRows log entries:<br><br><br>\n";
        

    for( $i=0; $i<$numRows; $i++ ) {
        $time = rs_mysqli_result( $result, $i, "entry_time" );
        $entry = htmlspecialchars( rs_mysqli_result( $result, $i, "entry" ) );

        echo "<b>$time</b>:<br>$entry<hr>\n";
        }
    }



function rs_clearLog() {
    rs_checkPassword( "clear_log" );

     echo "[<a href=\"server.php?action=show_data" .
         "\">Main</a>]<br><br><br>";
    
    global $tableNamePrefix;

    $query = "DELETE FROM $tableNamePrefix"."log;";
    $result = rs_queryDatabase( $query );
    
    if( $result ) {
        echo "Log cleared.";
        }
    else {
        echo "DELETE operation failed?";
        }
    }




















function rs_logout() {
    rs_checkReferrer();
    
    rs_clearPasswordCookie();

    echo "Logged out";
    }




function rs_showData( $checkPassword = true ) {
    // these are global so they work in embeded function call below
    global $skip, $search, $order_by;

    if( $checkPassword ) {
        rs_checkPassword( "show_data" );
        }
    
    global $tableNamePrefix, $remoteIP;


    $query = "SELECT COUNT(*) ".
            "FROM $tableNamePrefix"."user_stats;";

    $result = rs_queryDatabase( $query );
    
    $count = rs_mysqli_result( $result, 0, 0 );

    $halfCount = round( $count / 2 );

    $query = "SELECT game_total_seconds ".
        "FROM $tableNamePrefix"."user_stats ORDER BY game_total_seconds DESC ".
        "LIMIT $halfCount, 1;";

    $result = rs_queryDatabase( $query );
    
    $medianSec = rs_mysqli_result( $result, 0, 0 );

    $medianTime = rs_secondsToTimeSummary( $medianSec );

    
    $query = "SELECT game_count ".
        "FROM $tableNamePrefix"."user_stats ORDER BY game_count DESC ".
        "LIMIT $halfCount, 1;";

    $result = rs_queryDatabase( $query );
    
    $medianGames = rs_mysqli_result( $result, 0, 0 );


    

    $query =
        "SELECT SUM( game_total_seconds ) / COUNT(*) ".
        "   as average_game_total_seconds, ".
        "SUM( game_count ) / COUNT(*) ".
        "   as average_game_count ".
        "FROM $tableNamePrefix"."user_stats;";
    $result = rs_queryDatabase( $query );

    $average_game_total_seconds =
        rs_mysqli_result( $result, 0, "average_game_total_seconds" );

    $averageTotal = rs_secondsToTimeSummary( $average_game_total_seconds );
        
    $average_game_count = rs_mysqli_result( $result, 0, "average_game_count" );
    

    echo "<table width='100%' border=0><tr>".
        "<td>[<a href=\"server.php?action=show_data" .
            "\">Main</a>] [<a href='server.php?action=regen_static_html'>".
        "Regen HTML</a>]</td>".
        "<td align=center>Average: ".
        "$averageTotal spent in $average_game_count games<br>".
        "Median: $medianTime spent in $medianGames games</td>". 
        "<td align=right>[<a href=\"server.php?action=logout" .
            "\">Logout</a>]</td>".
        "</tr></table><br><br><br>";




    $skip = rs_requestFilter( "skip", "/[0-9]+/", 0 );
    
    global $usersPerPage;
    
    $search = rs_requestFilter( "search", "/[A-Z0-9_@. \-]+/i" );

    $order_by = rs_requestFilter( "order_by", "/[A-Z_]+/i",
                                  "last_game_date" );
    
    $keywordClause = "";
    $searchDisplay = "";
    
    if( $search != "" ) {
        

        $keywordClause = "WHERE ( email LIKE '%$search%' " .
            "OR id LIKE '%$search%' " .
            "OR review_name LIKE '%$search%' " .
            "OR review_text LIKE '%$search%' ) ";

        $searchDisplay = " matching <b>$search</b>";
        }
    

    

    // first, count results
    $query = "SELECT COUNT(*) FROM $tableNamePrefix".
        "user_stats $keywordClause;";

    $result = rs_queryDatabase( $query );
    $totalRecords = rs_mysqli_result( $result, 0, 0 );


    $orderDir = "DESC";

    if( $order_by == "email" ) {
        $orderDir = "ASC";
        }
    
             
    $query = "SELECT *, ".
        "TIMESTAMPDIFF( SECOND, first_game_date, last_game_date ) ".
        "   as play_span, ".
        "ROUND( game_total_seconds / game_count ) as average_game_seconds ".
        "FROM $tableNamePrefix"."user_stats $keywordClause".
        "ORDER BY $order_by $orderDir ".
        "LIMIT $skip, $usersPerPage;";
    $result = rs_queryDatabase( $query );
    
    $numRows = mysqli_num_rows( $result );

    $startSkip = $skip + 1;
    
    $endSkip = $startSkip + $usersPerPage - 1;

    if( $endSkip > $totalRecords ) {
        $endSkip = $totalRecords;
        }



        // form for searching tickets
?>
        <hr>
            <FORM ACTION="server.php" METHOD="post">
    <INPUT TYPE="hidden" NAME="action" VALUE="show_data">
    <INPUT TYPE="hidden" NAME="order_by" VALUE="<?php echo $order_by;?>">
    <INPUT TYPE="text" MAXLENGTH=40 SIZE=20 NAME="search"
             VALUE="<?php echo $search;?>">
    <INPUT TYPE="Submit" VALUE="Search">
    </FORM>
        <hr>
<?php

    

    
    echo "$totalRecords user records". $searchDisplay .
        " (showing $startSkip - $endSkip):<br>\n";

    
    $nextSkip = $skip + $usersPerPage;

    $prevSkip = $skip - $usersPerPage;
    
    if( $prevSkip >= 0 ) {
        echo "[<a href=\"server.php?action=show_data" .
            "&skip=$prevSkip&search=$search&order_by=$order_by\">".
            "Previous Page</a>] ";
        }
    if( $nextSkip < $totalRecords ) {
        echo "[<a href=\"server.php?action=show_data" .
            "&skip=$nextSkip&search=$search&order_by=$order_by\">".
            "Next Page</a>]";
        }

    echo "<br><br>";
    
    echo "<table border=1 cellpadding=5>\n";

    function orderLink( $inOrderBy, $inLinkText ) {
        global $skip, $search, $order_by;
        if( $inOrderBy == $order_by ) {
            // already displaying this order, don't show link
            return "<b>$inLinkText</b>";
            }

        // else show a link to switch to this order
        return "<a href=\"server.php?action=show_data" .
            "&search=$search&skip=$skip&order_by=$inOrderBy\">$inLinkText</a>";
        }

    
    echo "<tr>\n";    
    echo "<tr><td>".orderLink( "email", "Email" )."</td>\n";
    echo "<td>".orderLink( "last_game_date", "Last Game" )."</td>\n";
    echo "<td>".orderLink( "last_game_seconds", "Last Game Time" )."</td>\n";
    echo "<td>".orderLink( "game_count", "Game Count" )."</td>\n";
    echo "<td>".orderLink( "game_total_seconds", "Total Game Time" )."</td>\n";
    echo "<td>".orderLink( "average_game_seconds",
                           "Ave Game Time" )."</td>\n";
    echo "<td>".orderLink( "first_game_date", "First Game" )."</td>\n";
    echo "<td>".orderLink( "play_span", "Play Span" )."</td>\n";
    echo "<td>".orderLink( "review_votes", "Review Votes" )."</td>\n";
    echo "<td>Review</td>\n";
    echo "</tr>\n";


    for( $i=0; $i<$numRows; $i++ ) {
        $email = rs_mysqli_result( $result, $i, "email" );
        $last_game_date = rs_mysqli_result( $result, $i, "last_game_date" );
        $last_game_seconds = rs_mysqli_result( $result, $i,
                                              "last_game_seconds" );

        $play_span = rs_mysqli_result( $result, $i, "play_span" );

        $average_game_seconds =
            rs_mysqli_result( $result, $i, "average_game_seconds" );

        
        $game_count = rs_mysqli_result( $result, $i, "game_count" );
        $game_total_seconds = rs_mysqli_result( $result, $i,
                                               "game_total_seconds" );

        $first_game_date = rs_mysqli_result( $result, $i, "first_game_date" );

        $review_votes = rs_mysqli_result( $result, $i, "review_votes" );
        
        $review_text = rs_mysqli_result( $result, $i, "review_text" );

        $review_score = rs_mysqli_result( $result, $i, "review_score" );

        $review_name = rs_mysqli_result( $result, $i, "review_name" );
    

        $lastDuration = rs_secondsToTimeSummary( $last_game_seconds );
        $totalDuration = rs_secondsToTimeSummary( $game_total_seconds );

        $averageDuration = rs_secondsToTimeSummary( $average_game_seconds );

        $spanDuration = rs_secondsToAgeSummary( $play_span );
        
        $lastGameAgo = rs_secondsToAgeSummary( strtotime( "now" ) -
                                               strtotime( $last_game_date ) );

        $firstGameAgo = rs_secondsToAgeSummary( strtotime( "now" ) -
                                                strtotime( $first_game_date ) );
        

        $oldLen = strlen( $review_text );
        
        $review_text = substr( $review_text, 0, 80 );

        if( $oldLen > 80 ) {
            $review_text = $review_text . " ...";
            }

        $review_text = wordwrap( $review_text, 20, " ", true );

        
        
        $encodedEmail = urlencode( $email );

        $nameString = "";

        if( $review_name != "" ) {
            $nameString = "($review_name) ";
            }
        
        
        echo "<tr>\n";
        
        echo "<td>".
            "<a href=\"server.php?action=show_detail&email=$encodedEmail\">".
            "$email</a></td>\n";
        echo "<td>$last_game_date<br>($lastGameAgo ago)</td>\n";
        echo "<td>$lastDuration</td>\n";
        echo "<td>$game_count</td>\n";
        echo "<td>$totalDuration</td>\n";
        echo "<td>$averageDuration</td>\n";
        echo "<td>$first_game_date<br>($firstGameAgo ago)</td>\n";
        echo "<td>$spanDuration</td>\n";
        echo "<td>$review_votes</td>\n";
        echo "<td>$nameString<b>($review_score)</b><br>$review_text</td>\n";
        echo "</tr>\n";
        }
    echo "</table>";



    echo "<br><br><hr><br><br>";

    

    // now show polls

    $query = "SELECT COUNT(*) FROM $tableNamePrefix".
        "polls;";

    $result = rs_queryDatabase( $query );
    $totalRecords = rs_mysqli_result( $result, 0, 0 );


    $poll_skip = rs_requestFilter( "poll_skip", "/[0-9]+/", 0 );

    $startSkip = $poll_skip + 1;
    
    $endSkip = $startSkip + $usersPerPage - 1;

    if( $endSkip > $totalRecords ) {
        $endSkip = $totalRecords;
        }

    echo "$totalRecords poll records". $searchDisplay .
        " (showing $startSkip - $endSkip):<br>\n";
    
    
    $nextSkip = $poll_skip + $usersPerPage;

    $prevSkip = $poll_skip - $usersPerPage;
    
    if( $prevSkip >= 0 ) {
        echo "[<a href=\"server.php?action=show_data" .
            "&poll_skip=$prevSkip\">".
            "Previous Page</a>] ";
        }
    if( $nextSkip < $totalRecords ) {
        echo "[<a href=\"server.php?action=show_data" .
            "&poll_skip=$nextSkip\">".
            "Next Page</a>]";
        }

    echo "<br><br>";
    
    

    $query = "SELECT * FROM $tableNamePrefix"."polls ".
        "ORDER BY post_date desc ".
        "LIMIT $poll_skip, $usersPerPage;";
    $result = rs_queryDatabase( $query );
    
    $numRows = mysqli_num_rows( $result );
    
    echo "<table border=1 cellpadding=5>\n";
    
    echo "<tr>\n";    
    echo "<td>Post Date</td>\n";
    echo "<td>Start/End</td>\n";
    echo "<td>Min Lives</td>\n";
    echo "<td>Min Lived Time</td>\n";
    echo "<td>Question</td>\n";
    echo "<td>Answers</td>\n";
    echo "<td></td>\n";
    echo "</tr>\n";


    global $maxNumAnswers, $answerNames;
    
    for( $i=0; $i<$numRows; $i++ ) {
        $id = rs_mysqli_result( $result, $i, "id" );

        $post_date = rs_mysqli_result( $result, $i, "post_date" );
        $start_date = rs_mysqli_result( $result, $i, "start_date" );
        $end_date = rs_mysqli_result( $result, $i, "end_date" );

        $min_lives = rs_mysqli_result( $result, $i, "min_lives" );
        $min_lives_since_post_date =
            rs_mysqli_result( $result, $i, "min_lives_since_post_date" );

        $min_lived_seconds =
            rs_mysqli_result( $result, $i, "min_lived_seconds" );
        $min_lived_seconds_since_post_date =
            rs_mysqli_result( $result, $i,
                              "min_lived_seconds_since_post_date" );

        
        $question = rs_mysqli_result( $result, $i, "question" );

        $answers = array();

        $answerCounts = array();
        $totalCounts = 0;
        
        for( $a=0; $a < $maxNumAnswers; $a++ ) {
            $ans = rs_mysqli_result( $result, $i, $answerNames[$a] );

            if( $ans != "" ) {
                $answers[] = $ans;
                $num = rs_mysqli_result( $result, $i,
                                         $answerNames[$a] . "_count" );
                $answerCounts[] = $num;
                $totalCounts += $num;
                
                }
            }
        
        echo "<tr>\n";

        $postAgo = rs_secondsToAgeSummary( strtotime( "now" ) -
                                           strtotime( $post_date ) );
        
        echo "<td>$post_date<br>($postAgo ago)</td>\n";

        $startAgoSec = strtotime( "now" ) - strtotime( $start_date );

        $startAgo;

        if( $startAgoSec > 0 ) {
            $startAgo = rs_secondsToAgeSummary( $startAgoSec ) . " ago";
            }
        else {
            $startAgo = "in " . rs_secondsToAgeSummary( -$startAgoSec );
            }


        $endAgoSec = strtotime( "now" ) - strtotime( $end_date );

        $endAgo;

        if( $endAgoSec > 0 ) {
            $endAgo = rs_secondsToAgeSummary( $endAgoSec ) . " ago";
            }
        else {
            $endAgo = "in " . rs_secondsToAgeSummary( -$endAgoSec );
            }

        $runTime = rs_secondsToTimeSummary( strtotime( $end_date ) -
                                            strtotime( $start_date ) );
        
        
        echo "<td>$start_date<br>($startAgo)<br><br>".
            "to<br><br>$end_date<br>($endAgo)<br><br>".
            "Run time:  $runTime</td>\n";
        
        echo "<td>$min_lives<br>($min_lives_since_post_date new)</td>\n";
        $livedTime = rs_secondsToTimeSummary( $min_lived_seconds );
        $livedTimeSince =
            rs_secondsToTimeSummary( $min_lived_seconds_since_post_date );

        echo "<td>$livedTime<br>($livedTimeSince new)</td>\n";
        
        echo "<td>$question</td>\n";

        echo "<td>";

        $a = 0;
        foreach( $answers as $ans ) {
            $percent =  ( 100 * $answerCounts[$a] / $totalCounts );
            
            echo "$ans : <b>" . $answerCounts[$a] . "</b> ($percent%)<br>";
            $a ++;
            }
        echo "</td>";

        echo "<td><FORM ACTION=server.php METHOD=post>".
            "<INPUT TYPE=hidden NAME=action VALUE=delete_poll>".
            "<INPUT TYPE=hidden NAME=id VALUE=$id>".
            "<INPUT TYPE=checkbox NAME=confirm VALUE=1>".      
            "<INPUT TYPE=Submit VALUE=Delete>".
            "</FORM></td>";
        
        echo "</tr>\n";
        }
    echo "</table>";


    echo "<br><br><hr><br><br>";

    // form for adding a new poll

    echo "Create new Poll:<br><br>"
?>
    <FORM ACTION="server.php" METHOD="post">
    <INPUT TYPE="hidden" NAME="action" VALUE="create_poll">
         Start in:
    <INPUT TYPE="text" MAXLENGTH=10 SIZE=10 NAME="start_hours"
          value="2"> hours<br>
         Run for:
    <INPUT TYPE="text" MAXLENGTH=10 SIZE=10 NAME="run_days"
          value="2"> days<br>
          Min Lives:
    <INPUT TYPE="text" MAXLENGTH=10 SIZE=10 NAME="min_lives"
          value="10"><br>
         Min Lives Since Poll Posted:
    <INPUT TYPE="text" MAXLENGTH=10 SIZE=10 NAME="min_lives_since_post_date"
          value="3"><br>

         Min Lived Hours:
    <INPUT TYPE="text" MAXLENGTH=10 SIZE=10 NAME="min_lived_hours"
          value="2"><br>
         Min Lived Minutes Since Poll Posted:
    <INPUT TYPE="text" MAXLENGTH=10 SIZE=10 NAME="min_lived_minutes_since_post_date"
          value="60"><br>
         
         Question:<br>
     <TEXTAREA NAME="question" COLS=60 ROWS=10></TEXTAREA><br>
               Answers:<br>
<?php
    for( $a=0; $a < $maxNumAnswers; $a++ ) {
        $name = $answerNames[$a];
        
        echo "<INPUT TYPE=text MAXLENGTH=43 SIZE=43 NAME=$name ".
        "value='' ><br>";
        }
    ?>
    <INPUT TYPE="checkbox" NAME="confirm" VALUE=1> Confirm<br>      
    <INPUT TYPE="Submit" VALUE="Send">
    </FORM>
<?php
    

    echo "<hr>";
    
    echo "<a href=\"server.php?action=show_log\">".
        "Show log</a>";
    echo "<hr>";
    echo "Generated for $remoteIP\n";

    }



function rs_showDetail( $checkPassword = true ) {
    if( $checkPassword ) {
        rs_checkPassword( "show_detail" );
        }
    
    echo "[<a href=\"server.php?action=show_data" .
         "\">Main</a>]<br><br><br>";
    
    global $tableNamePrefix;
    

    $email = rs_requestFilter( "email", "/[A-Z0-9._%+\-]+@[A-Z0-9.\-]+/i" );
            
    $query = "SELECT * FROM $tableNamePrefix"."user_stats ".
            "WHERE email = '$email';";
    $result = rs_queryDatabase( $query );

    $id = rs_mysqli_result( $result, 0, "id" );
    $email = rs_mysqli_result( $result, 0, "email" );
    $review_date = rs_mysqli_result( $result, 0, "review_date" );

    $review_text = rs_mysqli_result( $result, 0, "review_text" );
    
    $review_score = rs_mysqli_result( $result, 0, "review_score" );
    
    $review_name = rs_mysqli_result( $result, 0, "review_name" );

    echo "<center><table border=0><tr><td>";
    
    echo "<b>Email:</b> $email<br><br>";
    echo "<b>Date:</b> $review_date<br><br>";
    echo "<b>Name:</b> $review_name<br><br>";
    echo "<b>Score:</b> $review_score<br><br>";


    global $fullServerURL;
    
    echo "<b>View:</b> ".
        "<a href='$fullServerURL?action=view_review&id=$id'>[$id]</a><br><br>";
    
    
    $review_text = preg_replace( '/\n/', "<br>", $review_text );

    $review_text = wordwrap( $review_text, 20, " ", true );
    
    echo "<b>Review:</b><br> ".
        "<table width=600 border=1 cellpadding=5 cellspacing=0><tr><td>$review_text</td></tr></table>";

    echo "</td></tr></table></center>";
    
    
    }




function rs_createPoll() {
    rs_checkPassword( "create_poll" );
    

    
    global $tableNamePrefix, $rs_mysqlLink;

    $confirm = rs_requestFilter( "confirm", "/[01]/" );


    if( $confirm != 1 ) {
        echo "[<a href=\"server.php?action=show_data" .
            "\">Main</a>]<br><br><br>";

        echo "You must check the Confirm box to create a poll\n";
        return;
        }
    

    $question = "";
    if( isset( $_REQUEST[ "question" ] ) ) {
        $question = $_REQUEST[ "question" ];
        }
    
    $question = preg_replace( '/\r\n/', "  ", $question );

    $question = preg_replace( '/    /', "  ", $question );

    
    $question = mysqli_real_escape_string( $rs_mysqlLink, $question );
    
    $start_hours = rs_requestFilter( "start_hours", "/[0-9.]+/", 1 );
    
    $run_days = rs_requestFilter( "run_days", "/[0-9.]+/", 1 );

    $min_lives = rs_requestFilter( "min_lives", "/[0-9]+/", 10 );

    $min_lives_since_post_date =
        rs_requestFilter( "min_lives_since_post_date", "/[0-9]+/", 2 );

        $min_lives = rs_requestFilter( "min_lives", "/[0-9]+/", 10 );

    $min_lived_hours =
        rs_requestFilter( "min_lived_hours", "/[0-9]+/", 2 );

    $min_lived_minutes_since_post_date =
        rs_requestFilter( "min_lived_minutes_since_post_date", "/[0-9]+/", 2 );

    
    $answers = array();

    global $maxNumAnswers, $answerNames;
    
    for( $i=0; $i<$maxNumAnswers; $i++ ) {
        $answers[$i] = "";

        if( isset( $_REQUEST[ $answerNames[$i] ] ) ) {
            $answers[$i] = $_REQUEST[ $answerNames[$i] ];
            }

        $answers[$i] = mysqli_real_escape_string( $rs_mysqlLink, $answers[$i] );
        }

    global $maxNumAnswers, $answerNames;
    
    $answerClause = "";
    $countClause = "";
    for( $i=0; $i<$maxNumAnswers; $i++ ) {
        $answerClause =
            $answerClause .
            $answerNames[$i] .
            " = '" . $answers[$i] . "' ";

        $countClause = $countClause . $answerNames[$i] . "_count = 0, ";
        
        if( $i < $maxNumAnswers - 1 ) {
            $answerClause = $answerClause . " , ";
            }
        }

    global $tableNamePrefix;
    
    $query = "INSERT INTO $tableNamePrefix". "polls SET " .
        "post_date = CURRENT_TIMESTAMP, ".
        "start_date = DATE_ADD( CURRENT_TIMESTAMP, ".
        "  INTERVAL $start_hours * 3600 SECOND ), ".
        "end_date = DATE_ADD( DATE_ADD( CURRENT_TIMESTAMP, ".
        "  INTERVAL $start_hours * 3600 SECOND ), ".
        "  INTERVAL $run_days * 3600 * 24 SECOND ), ".
        "min_lives = $min_lives, ".
        "min_lives_since_post_date = $min_lives_since_post_date, ".
        "min_lived_seconds = $min_lived_hours * 3600, ".
        "min_lived_seconds_since_post_date = ".
        "    $min_lived_minutes_since_post_date * 60, ".
        "question = '$question', ".
        $countClause .
        $answerClause .
        ";";

    rs_log( $query );
    
    rs_queryDatabase( $query );


    // reset lives for all users
    $query = "UPDATE $tableNamePrefix". "user_stats SET " .
        "lives_since_recent_poll = 0, seconds_lived_since_recent_poll = 0, ".
        "recent_poll_answered = 0;";

    rs_queryDatabase( $query );


    rs_showData( false );
    }



function rs_deletePoll() {
    rs_checkPassword( "delete_poll" );

    
    global $tableNamePrefix;

    $confirm = rs_requestFilter( "confirm", "/[01]/" );
    
    if( $confirm != 1 ) {
        echo "[<a href=\"server.php?action=show_data" .
            "\">Main</a>]<br><br><br>";
        echo "You must check the Confirm box to delete a poll\n";
        return;
        }

    
    $id = rs_requestFilter( "id", "/[0-9]+/", -1 );
    
    global $tableNamePrefix;
    
    rs_queryDatabase( "DELETE FROM $tableNamePrefix"."polls " .
                      "WHERE id = $id;" );
    
    
    rs_showData( false );
    }




// if $doNotPrint is true, returns active poll ID or -1
function rs_checkForPoll( $doNotPrint = false ) {

    $email = rs_requestFilter( "email", "/[A-Z0-9._%+\-]+@[A-Z0-9.\-]+/i", "" );

    if( $email == "" ) {
        if( $doNotPrint ) {
            return -1;
            }
        
        echo "DENIED";
        return;
        }
    
    
    global $tableNamePrefix;


    $query = "SELECT * FROM $tableNamePrefix"."user_stats ".
            "WHERE email = '$email';";
    $result = rs_queryDatabase( $query );

    $numRows = mysqli_num_rows( $result );

    if( $numRows == 0 ) {
        if( $doNotPrint ) {
            return -1;
            }
        echo "DENIED";
        return;
        }
    

    $game_count =
        rs_mysqli_result( $result, 0, "game_count" );
    $game_total_seconds =
        rs_mysqli_result( $result, 0, "game_total_seconds" );


    $seconds_lived_since_recent_poll =
        rs_mysqli_result( $result, 0, "seconds_lived_since_recent_poll" );

    $lives_since_recent_poll =
        rs_mysqli_result( $result, 0, "lives_since_recent_poll" );
    
    $recent_poll_answered =
        rs_mysqli_result( $result, 0, "recent_poll_answered" );

    if( $recent_poll_answered ) {
        if( $doNotPrint ) {
            return -1;
            }
        echo "DENIED";
        return;
        }


    $query = "SELECT * FROM $tableNamePrefix"."polls ".
        "WHERE start_date <= CURRENT_TIMESTAMP AND ".
        "end_date > CURRENT_TIMESTAMP AND ".
        "min_lives <= $game_count AND ".
        "min_lives_since_post_date <= $lives_since_recent_poll AND ".
        "min_lived_seconds <= $game_total_seconds AND ".
        "min_lived_seconds_since_post_date <= ".
        "    $seconds_lived_since_recent_poll;";


    $result = rs_queryDatabase( $query );

    $numRows = mysqli_num_rows( $result );

    if( $numRows == 0 ) {
        if( $doNotPrint ) {
            return -1;
            }
        echo "DENIED";
        return;
        }

    $poll_id = rs_mysqli_result( $result, 0, "id" );

    // one exists
    if( $doNotPrint ) {
        return $poll_id;
        }
        


    echo "$poll_id\n";


    $question = rs_mysqli_result( $result, 0, "question" );

    echo "$question\n";


    global $maxNumAnswers, $answerNames;
    
    for( $i=0; $i<$maxNumAnswers; $i++ ) {
        $answers[$i] = "";

        $ans = rs_mysqli_result( $result, 0, $answerNames[$i] );

        if( $ans != "" ) {
            echo "$ans\n";
            }
        }

    echo "OK";
    }




function rs_pollVote() {
    global $tableNamePrefix, $sharedGameServerSecret, $rs_mysqlLink;
    

    $email = rs_requestFilter( "email", "/[A-Z0-9._%+\-]+@[A-Z0-9.\-]+/i", "" );
    $poll_id = rs_requestFilter( "poll_id", "/[0-9]+/i", "0" );
    $vote_number = rs_requestFilter( "vote_number", "/[0-9]+/i", "0" );

    $hash_value = rs_requestFilter( "hash_value", "/[A-F0-9]+/i", "" );

    $hash_value = strtoupper( $hash_value );

    
    

    if( $email == "" ) {
        rs_log( "poll_vote denied for bad email" );

        echo "DENIED";
        return;
        }


    $stringToHash = $poll_id . "v" . $vote_number;


    $encodedString = urlencode( $stringToHash );
    $encodedEmail = urlencode( $email );

    global $ticketServerURL;

    $request = "$ticketServerURL".
        "?action=check_ticket_hash".
        "&email=$encodedEmail" .
        "&hash_value=$hash_value" .
        "&string_to_hash=$encodedString";
    
    $result = file_get_contents( $request );

    if( $result != "VALID" ) {
        rs_log( "poll_vote denied for failed hash check " );

        echo "DENIED";
        return;
        }

    // lock table to prevent double-voiting
    rs_queryDatabase( "lock tables $tableNamePrefix"."user_stats write;" );


    $poll_id = rs_checkForPoll( true );

    if( $poll_id == -1 ) {
        rs_log( "poll_vote denied, no poll exists" );

        rs_queryDatabase( "unlock tables;" );

        echo "DENIED";
        return;
        }

    
    $query = "UPDATE $tableNamePrefix".
        "user_stats SET recent_poll_answered = 1 WHERE  email = '$email';";

    $result = rs_queryDatabase( $query );

    rs_queryDatabase( "unlock tables;" );

    global $maxNumAnswers, $answerNames;

    $thisAnswerName = "";
    
    for( $i=0; $i<$maxNumAnswers; $i++ ) {

        if( $i == $vote_number ) {
            $thisAnswerName = $answerNames[$i];
            break;
            }
        }


    if( $thisAnswerName == "" ) {
        rs_log( "poll_vote denied, bad answer index $vote_number" );
        
        echo "DENIED";
        return;
        }
    

    $query = "UPDATE $tableNamePrefix".
        "polls SET $thisAnswerName" . "_count = ".
        "$thisAnswerName" . "_count + 1 WHERE id = $poll_id;";

    $result = rs_queryDatabase( $query );

    
    echo "OK";
    }



function rs_listPolls() {
    global $header, $footer;

    eval( $header );
    
    $posNeg = rs_requestFilter( "pos_neg", "/[\-01]+/i", -1 );
    $skip = rs_requestFilter( "skip", "/[0-9]+/i", 0 );

    global $reviewPageWidth;
    echo "<center><table border=0 width=$reviewPageWidth><tr><td>";

    global $tableNamePrefix;
    
    $query = "SELECT * FROM $tableNamePrefix"."polls ".
        "ORDER BY post_date desc;";
    $result = rs_queryDatabase( $query );

    $numRows = mysqli_num_rows( $result );

    
    for( $i=0; $i<$numRows; $i++ ) {
        $id = rs_mysqli_result( $result, $i, "id" );

        $post_date = rs_mysqli_result( $result, $i, "post_date" );
        $start_date = rs_mysqli_result( $result, $i, "start_date" );
        $end_date = rs_mysqli_result( $result, $i, "end_date" );

        $min_lives = rs_mysqli_result( $result, $i, "min_lives" );
        $min_lives_since_post_date =
            rs_mysqli_result( $result, $i, "min_lives_since_post_date" );

        $min_lived_seconds =
            rs_mysqli_result( $result, $i, "min_lived_seconds" );
        $min_lived_seconds_since_post_date =
            rs_mysqli_result( $result, $i,
                              "min_lived_seconds_since_post_date" );

        
        $question = rs_mysqli_result( $result, $i, "question" );

        $answers = array();

        $answerCounts = array();
        $totalCounts = 0;

        global $answerNames, $maxNumAnswers;
        
        for( $a=0; $a < $maxNumAnswers; $a++ ) {
            $ans = rs_mysqli_result( $result, $i, $answerNames[$a] );

            if( $ans != "" ) {
                $answers[] = $ans;
                $num = rs_mysqli_result( $result, $i,
                                         $answerNames[$a] . "_count" );
                $answerCounts[] = $num;
                $totalCounts += $num;
                
                }
            }

        echo "<table border=0 width=100% cellpadding=20>".
            "<tr><td BGCOLOR=#444444>";

        $startAgoSec = strtotime( "now" ) - strtotime( $start_date );

        $startAgo;

        if( $startAgoSec > 0 ) {
            $startAgo = rs_secondsToAgeSummary( $startAgoSec ) . " ago";
            }
        else {
            $startAgo = "in " . rs_secondsToAgeSummary( -$startAgoSec );
            }

        $startString = date('l F j, Y g:i:s A', strtotime( $start_date ) );

                
        $runTime = rs_secondsToTimeSummary( strtotime( $end_date ) -
                                            strtotime( $start_date ) );

        $endHint = "";

        $endAgoSec = strtotime( "now" ) - strtotime( $end_date );

        if( $endAgoSec < 0 ) {
            $endHint =
                " (ends in " . rs_secondsToAgeSummary( -$endAgoSec ) . ")";
            }
        

        echo "<table border=0 cellspacing=0 cellpadding=0 width=100%>";
        
        echo "<tr><td>$startString</td><td align=right>$startAgo</td></tr>";
        echo "<tr><td colspan=2 align=left>Run time: $runTime".
            "$endHint</td></tr>";
        echo "</table>";

        echo "<hr><br>";
        

        echo "Question:<br><br>";

        echo "<center><table border=0 width=90% cellpadding=15 cellspacing=0>";
        echo "<tr><td BGCOLOR=#333333>$question</td></tr>";

        echo "</table></center>";
        
        echo "<br>";

        
        echo "Answers:<br><br>";

        echo "<center><table border=0 width=90% cellpadding=15 cellspacing=0>";

        $bgColor = "BGCOLOR=#555555";

        $bgColorAlt = "BGCOLOR=#333333";
        
        $a = 0;

        $answerCountMap = array();

        foreach( $answers as $answer ) {
            $num = $answerCounts[$a];

            $answerCountMap[ $answer ] = $num;
            $a++;
            }
        
        arsort( $answerCountMap );
        
        
        foreach( $answerCountMap as $answer => $num ) {
            $percent = 100 * $num / $totalCounts;
            $percent = number_format( $percent, 1 );
            
            echo "<tr><td $bgColor>$answer</td>".
                "<td $bgColor align=right>$num</td>".
                "<td $bgColor align=right nowrap>$percent %</td></tr>";

            $temp = $bgColor;
            $bgColor = $bgColorAlt;
            $bgColorAlt = $temp;
            }
        echo "</td></tr></table>";
        
        
        echo "</td></tr></table><br><br><br><br>";
        }
    
    
    echo "</td></tr></table></center>";
    
    eval( $footer );
    }



// called from web UI
function rs_regenStaticHTML() {
    rs_checkPassword( "regen_static_html" );

    rs_generateRecentStatic();
    rs_generateTopPlaytimeStatic();
    rs_generateReviewCountStatic();

    rs_showData( false );
    }


// called from cron job
function rs_updateHTML() {
    rs_generateRecentStatic();
    rs_generateTopPlaytimeStatic();
    rs_generateReviewCountStatic();

    echo "OK";
    }



function rs_getReviewHTML( $inID, $inWidth=600, $inTextLengthLimit = -1 ) {
    global $tableNamePrefix;

    $query = "SELECT * FROM $tableNamePrefix"."user_stats ".
            "WHERE id = '$inID';";
    $result = rs_queryDatabase( $query );

    $game_total_seconds = rs_mysqli_result( $result, 0, "game_total_seconds" );
    
    $last_game_date = rs_mysqli_result( $result, 0, "last_game_date" );

    $review_date = rs_mysqli_result( $result, 0, "review_date" );

    $review_text = rs_mysqli_result( $result, 0, "review_text" );
    
    $review_score = rs_mysqli_result( $result, 0, "review_score" );
    
    $review_name = rs_mysqli_result( $result, 0, "review_name" );


    $totalDuration = rs_secondsToTimeSummary( $game_total_seconds );

        
    $lastGameAgo = rs_secondsToAgeSummary( strtotime( "now" ) -
                                           strtotime( $last_game_date ) );

    $reviewAgo = rs_secondsToAgeSummary( strtotime( "now" ) -
                                         strtotime( $review_date ) );

    global $recIcon, $notRecIcon, $fullServerURL;

    $icon = $recIcon;

    $recText = "Recommended";

    if( $review_score == 0 ) {
        $icon = $notRecIcon;
        $recText = "Not Recommended";
        }

    $text = "<table width=$inWidth cellspacing=0 cellpadding=10 border=0><tr>";


    if( $inWidth >= 500 ) {
        // full width view
        $text= $text.
            "<td valign=middle bgcolor=#404040 width=40>".
            "<a href='$fullServerURL?action=view_review&id=$inID'>".
            "<img src='$icon' border=0></a></td>".
            "<td valign=middle bgcolor=#404040>".
            "<table width=100% cellspacing=0 cellpadding=0><tr>".
            "<td valign=middle><font size=5><b>$recText</b></font></td>".
            "<td align=right>by $review_name<br>".
            "$totalDuration on record</td></tr></table></td></tr>";
        }
    else {
        // compact view
        $text= $text.
            "<td valign=middle bgcolor=#404040 width=40>".
            "<a href='$fullServerURL?action=view_review&id=$inID'>".
            "<img src='$icon' border=0></a></td>".
            "<td valign=middle bgcolor=#404040>".
            "<table width=100% cellspacing=0 cellpadding=0><tr>".
            "<td valign=middle><b>$recText</b><br>".
            "$review_name ".
            "($totalDuration on record)</td></tr></table></td></tr>";
        }
    
    $review_text = preg_replace( '/\n/', "<br>", $review_text );

    $bottomLink = "";
    
    if( $inTextLengthLimit != -1 ) {
        if( strlen( $review_text ) > $inTextLengthLimit ) {
            $review_text = substr( $review_text, 0, $inTextLengthLimit );

            $review_text = $review_text . "...";
            
            global $fullServerURL;
            $bottomLink =
                "<div style='text-align:right'>[<a ".
                "href='$fullServerURL?action=view_review&id=$inID'>".
                "Read more</a>]</div>";
            }
        }
    
    $text = $text .
        "<tr>".
        "<td colspan=2 bgcolor=#202020>".
        "<table border=0 cellpadding=0 cellspacing=0 width=100%>";

    if( $inWidth >= 500 ) {
        $text = $text .
            "<tr><td>Posted $reviewAgo ago</td>".
            "<td align=right>Last Played $lastGameAgo ago</td></tr></table>";
        }
    else {
        $text = $text .
            "<tr><td colspan=2>Posted $reviewAgo ago</td></tr></table>";
        }

    
    $review_text = wordwrap( $review_text, 20, " ", true );
    
    $text = $text .
        "<div style='border-top: 2px solid gray'></div><br>".
        "$review_text$bottomLink</td></tr></table>";

    return $text;
    }



// pass in full ORDER BY clause
// $inPosNeg is 1 or 0, only pos/neg reviews will be shown
//             -1 to show all
// $inFractionList 1 means list length modulated by fraction of positive
//                 or negative reviews
function rs_getListHTML( $inOrderBy, $inAction, $inWidth = -1,
                         $inSkip=0, $inPosNeg=-1,
                         $inFractionList = 0 ) {
    
    global $tableNamePrefix, $reviewListLength, $summaryTextLength,
        $reviewListWidth;

    
    if( $inWidth == -1 ) {
        $inWidth = $reviewListWidth;
        }

    $posNegClause = "";

    if( $inPosNeg != -1 ) {
        $posNegClause = "AND review_score = $inPosNeg";
        }


    $displayListLength = $reviewListLength;


    if( $inPosNeg != -1 && $inFractionList  ) {
        $query = "SELECT COUNT(*) FROM $tableNamePrefix"."user_stats ".
            "WHERE review_score != -1;";
        $result = rs_queryDatabase( $query );
        
        $count = rs_mysqli_result( $result, 0, 0 );
        
        
        $query = "SELECT COUNT(*) FROM $tableNamePrefix"."user_stats ".
            "WHERE review_score = 1;";
        $result = rs_queryDatabase( $query );
        
        
        $posCount = rs_mysqli_result( $result, 0, 0 );
        
        
        $fraction = $posCount / $count;

        if( $inPosNeg == 1 ) {
            $displayListLength = $fraction * $displayListLength;
            }
        else {
            $displayListLength = (1 - $fraction ) * $displayListLength;
            }

        // extend list by 1
        // most lists have at least 2, then, if positive fraction < 1.00
        $displayListLength = ceil( $displayListLength ) + 1;        
        }

    
    $query = "SELECT id FROM $tableNamePrefix"."user_stats ".
        "WHERE review_score != -1 $posNegClause ".
        "$inOrderBy LIMIT $inSkip, $displayListLength;";
    $result = rs_queryDatabase( $query );

    $numRows = mysqli_num_rows( $result );

    $totalText = "";
    
    for( $i=0; $i<$numRows; $i++ ) {
        $id = rs_mysqli_result( $result, $i, "id" );

        $text = rs_getReviewHTML( $id, $inWidth, $summaryTextLength );

        $totalText = $totalText . $text . "<br>";

        if( $inWidth >= 500 ) {
            $totalText = $totalText . "<br>";
            }
        }

    // next/prev widget on bottom

    $widgetText = "<table border=0 width=$inWidth><tr>";

    global $fullServerURL;

    $prevShown = false;
    
    if( $inSkip > 0 ) {
        $prev = $inSkip - $displayListLength;
        if( $prev < 0 ) {
            $prev = 0;
            }
        $widgetText = $widgetText .
            "<td>[<a ".
            "href='$fullServerURL?action=$inAction".
            "&pos_neg=$inPosNeg&skip=$prev'>Prev Page</a>]".
            "</td>";
        
        $prevShown = true;
        }

    $next = $inSkip + $displayListLength;

    $query = "SELECT COUNT(*) FROM $tableNamePrefix"."user_stats ".
        "WHERE review_score != -1 $posNegClause ".
        "$inOrderBy;";
    $result = rs_queryDatabase( $query );

    $count = rs_mysqli_result( $result, 0, 0 );

    if( $count > $next ) {
        if( $prevShown ) {
            $widgetText = $widgetText .
                "<td align=right>[<a ".
                "href='$fullServerURL?action=$inAction".
                "&pos_neg=$inPosNeg&skip=$next'>".
                "Next Page</a>]</td>";
            }
        else {
            $widgetText = $widgetText .
                "<td align=center>[<a ".
                "href='$fullServerURL?action=$inAction".
                "&pos_neg=$inPosNeg&skip=$next'>".
                "More Reviews</a>]</td>";
            }
        }
    
    $widgetText = $widgetText . "</tr></table>";
    
    
    $totalText = $totalText . $widgetText;
    
    
    return $totalText;
    }



function rs_getRecentReviewHTML( $inWidth, $inSkip, $inPosNeg = -1,
                                 $inFractionList = 0 ) {
    return rs_getListHTML( "ORDER BY review_date DESC",
                           "list_recent", $inWidth, $inSkip, $inPosNeg,
                           $inFractionList );
    }


function rs_getTopPlaytimeReviewHTML( $inWidth, $inSkip, $inPosNeg = -1,
                                      $inFractionList = 0 ) {
    return rs_getListHTML( "ORDER BY game_total_seconds DESC",
                           "list_playtime", $inWidth, $inSkip, $inPosNeg,
                           $inFractionList );
    }



function rs_listRecent() {
    global $header, $footer;

    eval( $header );
    
    $posNeg = rs_requestFilter( "pos_neg", "/[\-01]+/i", -1 );
    $skip = rs_requestFilter( "skip", "/[0-9]+/i", 0 );
    
    echo "<center><table border=0><tr><td>";

    global $reviewPageWidth;
    
    $text = rs_getRecentReviewHTML( $reviewPageWidth, $skip, $posNeg );

    echo $text;
    
    echo "</td></tr></table></center>";
    
    eval( $footer );
    }



function rs_listPlaytime() {
    global $header, $footer;

    eval( $header );
    
    $posNeg = rs_requestFilter( "pos_neg", "/[\-01]+/i", -1 );
    $skip = rs_requestFilter( "skip", "/[0-9]+/i", 0 );
    
    echo "<center><table border=0><tr><td>";

    global $reviewPageWidth;
    
    $text = rs_getTopPlaytimeReviewHTML( $reviewPageWidth, $skip, $posNeg );

    echo $text;
    
    echo "</td></tr></table></center>";
    
    eval( $footer );
    }



// saves HTML to disk
function rs_generateRecentStatic() {
    $text = rs_getRecentReviewHTML( -1, 0 );

    global $outputPathRecent;

    file_put_contents ( $outputPathRecent, $text );
    
    $text = rs_getRecentReviewHTML( -1, 0, 1, 1 );

    global $outputPathRecentPositive;

    file_put_contents ( $outputPathRecentPositive, $text );

    $text = rs_getRecentReviewHTML( -1, 0, 0, 1 );

    global $outputPathRecentNegative;

    file_put_contents ( $outputPathRecentNegative, $text );
    }


function rs_generateTopPlaytimeStatic() {
    $text = rs_getTopPlaytimeReviewHTML( -1, 0 );

    global $outputPathPlaytime;

    file_put_contents ( $outputPathPlaytime, $text );

    
    $text = rs_getTopPlaytimeReviewHTML( -1, 0, 1, 1 );

    global $outputPathPlaytimePositive;

    file_put_contents ( $outputPathPlaytimePositive, $text );


    $text = rs_getTopPlaytimeReviewHTML( -1, 0, 0, 1 );

    global $outputPathPlaytimeNegative;

    file_put_contents ( $outputPathPlaytimeNegative, $text );
    }



function rs_generateReviewCountStatic() {
    global $tableNamePrefix;
    
    $query = "SELECT COUNT(*) FROM $tableNamePrefix"."user_stats ".
        "WHERE review_score != -1;";
    $result = rs_queryDatabase( $query );

    $count = rs_mysqli_result( $result, 0, 0 );

    
    $query = "SELECT COUNT(*) FROM $tableNamePrefix"."user_stats ".
        "WHERE review_score = 1;";
    $result = rs_queryDatabase( $query );

    
    $posCount = rs_mysqli_result( $result, 0, 0 );

    
    $fraction = $posCount / $count;
    $negCount = $count - $posCount;
    
    $percent = ceil( $fraction * 100 );
    $negPercent = 100 - $percent;
    
    $text = "<?php \$rs_reviewCount = $count; ".
        "\$rs_reviewCount_positive = $posCount; ".
        "\$rs_reviewCount_negative = $negCount; ".
        "\$rs_positivePercent = $percent; ".
        "\$rs_negativePercent = $negPercent; ".
        "?>";

    global $outputReviewCountPHP;

    file_put_contents ( $outputReviewCountPHP, $text );
    }





function rs_viewReview() {
    global $header, $footer;

    eval( $header );
    
    $id = rs_requestFilter( "id", "/[0-9]+/i", 0 );
    $w = rs_requestFilter( "w", "/[0-9]+/i", 600 );

    echo "<center><table border=0><tr><td>";

    global $reviewPageWidth;
    
    $text = rs_getReviewHTML( $id, $reviewPageWidth, -1 );

    echo $text;
    
    echo "</td></tr></table></center>";
    
    eval( $footer );
    }




function rs_getSequenceNumber() {
    global $tableNamePrefix;
    

    $email = rs_requestFilter( "email", "/[A-Z0-9._%+\-]+@[A-Z0-9.\-]+/i", "" );

    if( $email == "" ) {
        echo "DENIED";
        return;
        }
    
    
    $seq = rs_getSequenceNumberForEmail( $email );

    echo "$seq\n"."OK";
    }



// assumes already-filtered, valid email
// returns 0 if not found
function rs_getSequenceNumberForEmail( $inEmail ) {
    global $tableNamePrefix;
    
    $query = "SELECT sequence_number FROM $tableNamePrefix"."user_stats ".
        "WHERE email = '$inEmail';";
    $result = rs_queryDatabase( $query );

    $numRows = mysqli_num_rows( $result );

    if( $numRows < 1 ) {
        return 0;
        }
    else {
        return rs_mysqli_result( $result, 0, "sequence_number" );
        }
    }



function rs_logGame() {
    global $tableNamePrefix, $sharedGameServerSecret;
    

    $email = rs_requestFilter( "email", "/[A-Z0-9._%+\-]+@[A-Z0-9.\-]+/i", "" );
    $game_seconds = rs_requestFilter( "game_seconds", "/[0-9]+/i", "0" );
    $sequence_number = rs_requestFilter( "sequence_number", "/[0-9]+/i", "0" );

    $hash_value = rs_requestFilter( "hash_value", "/[A-F0-9]+/i", "" );

    $hash_value = strtoupper( $hash_value );


    if( $email == "" ||
        $game_seconds == 0 ) {

        rs_log( "logGame denied for bad email or game_seconds" );
        
        echo "DENIED";
        return;
        }
    
    $trueSeq = rs_getSequenceNumberForEmail( $email );

    if( $trueSeq > $sequence_number ) {
        rs_log( "logGame denied for stale sequence number" );

        echo "DENIED";
        return;
        }

    $computedHashValue =
        strtoupper( rs_hmac_sha1( $sharedGameServerSecret, $sequence_number ) );

    if( $computedHashValue != $hash_value ) {
        rs_log( "logGame denied for bad hash value" );

        echo "DENIED";
        return;
        }

    if( $trueSeq == 0 ) {
        // no record exists, add one
        $query = "INSERT INTO $tableNamePrefix". "user_stats SET " .
            "email = '$email', ".
            "sequence_number = $sequence_number + 1, ".
            "first_game_date = CURRENT_TIMESTAMP, ".
            "last_game_date = CURRENT_TIMESTAMP, ".
            "last_game_seconds = $game_seconds, ".
            "game_count = 1,".
            "game_total_seconds = $game_seconds,".
            "review_score = -1, ".
            "review_name = '',".
            "review_text = '',".
            "review_date = CURRENT_TIMESTAMP,".
            "review_game_seconds = 0,".
            "review_game_count=0,".
            "review_votes = 0,".
            "lives_since_recent_poll = 1,".
            "seconds_lived_since_recent_poll = $game_seconds,".
            "recent_poll_answered = 0;";
        }
    else {
        // update the existing one
        $query = "UPDATE $tableNamePrefix"."user_stats SET " .
            "sequence_number = $sequence_number + 1, ".
            "game_count = game_count + 1, " .
            "game_total_seconds = game_total_seconds + $game_seconds, " .
            "last_game_date = CURRENT_TIMESTAMP, " .
            "last_game_seconds = $game_seconds, " .
            "lives_since_recent_poll = lives_since_recent_poll + 1, " .
            "seconds_lived_since_recent_poll = ".
            "    seconds_lived_since_recent_poll + $game_seconds " .
            "WHERE email = '$email'; ";
        
        }

    rs_queryDatabase( $query );

    // recent reviews won't change, but top playtime might

    // still, regenerate them all so that the Posted On timestamps
    // will update
    rs_generateRecentStatic();
    rs_generateTopPlaytimeStatic();
    rs_generateReviewCountStatic();
    
    echo "OK";
    }



function rs_getStats() {
    global $tableNamePrefix, $sharedGameServerSecret;
    

    $email = rs_requestFilter( "email", "/[A-Z0-9._%+\-]+@[A-Z0-9.\-]+/i", "" );
    $sequence_number = rs_requestFilter( "sequence_number", "/[0-9]+/i", "0" );

    $hash_value = rs_requestFilter( "hash_value", "/[A-F0-9]+/i", "" );

    $hash_value = strtoupper( $hash_value );


    if( $email == "" ) {

        rs_log( "getStats denied for bad email" );
        
        echo "DENIED";
        return;
        }
    
    $trueSeq = rs_getSequenceNumberForEmail( $email );

    if( $trueSeq > $sequence_number ) {
        rs_log( "getStats denied for stale sequence number" );

        echo "DENIED";
        return;
        }

    $computedHashValue =
        strtoupper( rs_hmac_sha1( $sharedGameServerSecret, $sequence_number ) );

    if( $computedHashValue != $hash_value ) {
        rs_log( "getStats denied for bad hash value" );

        echo "DENIED";
        return;
        }

    $game_count = 0;
    $game_total_seconds = 0;
    
    
    if( $trueSeq == 0 ) {
        // no record exists....
        // don't add one for now

        // when user doesn't exist, it's fine to keep returning
        // 0 sequence number for them
        }
    else {
        // update the existing seq #
        $query = "UPDATE $tableNamePrefix"."user_stats SET " .
            "sequence_number = $sequence_number + 1 ".
            "WHERE email = '$email'; ";
        rs_queryDatabase( $query );

        $query = "SELECT game_count, game_total_seconds ".
            "FROM $tableNamePrefix"."user_stats ".
            "WHERE email = '$email'; ";

        $result = rs_queryDatabase( $query );
        $numRows = mysqli_num_rows( $result );

        
        if( $numRows == 1 ) {

            $game_count = rs_mysqli_result( $result, 0, "game_count" );
            $game_total_seconds =
                rs_mysqli_result( $result, 0, "game_total_seconds" );
            }
        }
    

    echo "$game_count\n$game_total_seconds\n";
    echo "OK";
    }







function rs_submitReview() {
    global $tableNamePrefix, $sharedGameServerSecret, $rs_mysqlLink;
    

    $email = rs_requestFilter( "email", "/[A-Z0-9._%+\-]+@[A-Z0-9.\-]+/i", "" );
    $review_score = rs_requestFilter( "review_score", "/[0-9]+/i", "0" );

    $review_name = rs_requestFilter( "review_name", "/[A-Z0-9._\- ]+/i", "" );
    
    $review_text_RAW = $_REQUEST[ "review_text" ];
    
    $slashedText = mysqli_real_escape_string( $rs_mysqlLink, $review_text_RAW );
    
    $hash_value = rs_requestFilter( "hash_value", "/[A-F0-9]+/i", "" );

    $hash_value = strtoupper( $hash_value );

    
    

    if( $email == "" ) {
        rs_log( "submitReview denied for bad email" );

        echo "DENIED";
        return;
        }


    $textSHA1 = strtoupper( sha1( $review_text_RAW ) );
    $nameSHA1 = strtoupper( sha1( $review_name ) );

    $stringToHash = $review_score . $nameSHA1 . $textSHA1;


    $encodedString = urlencode( $stringToHash );
    $encodedEmail = urlencode( $email );

    global $ticketServerURL;

    $request = "$ticketServerURL".
        "?action=check_ticket_hash".
        "&email=$encodedEmail" .
        "&hash_value=$hash_value" .
        "&string_to_hash=$encodedString";
    
    $result = file_get_contents( $request );

    if( $result != "VALID" ) {
        rs_log( "submitReview denied for failed hash check " );

        echo "DENIED";
        return;
        }

    $query = "SELECT COUNT(*) FROM $tableNamePrefix".
        "user_stats WHERE email = '$email';";

    $result = rs_queryDatabase( $query );
    $count = rs_mysqli_result( $result, 0, 0 );

    if( $count == 0 ) {
        // can post review with no games logged
        $query = "INSERT INTO $tableNamePrefix". "user_stats SET " .
            "email = '$email', ".
            "sequence_number = 1, ".
            "first_game_date = CURRENT_TIMESTAMP, ".
            "last_game_date = CURRENT_TIMESTAMP, ".
            "last_game_seconds = 0, ".
            "game_count = 0,".
            "game_total_seconds = 0,".
            "review_score = $review_score, ".
            "review_name = '$review_name',".
            "review_text = '$slashedText',".
            "review_date = CURRENT_TIMESTAMP,".
            "review_game_seconds = 0,".
            "review_game_count=0,".
            "review_votes = 0,".
            "lives_since_recent_poll = 0,".
            "seconds_lived_since_recent_poll = 0,".
            "recent_poll_answered = 0;";
        }
    else {
        // update the existing one
        $query = "UPDATE $tableNamePrefix"."user_stats SET " .
            "review_score = $review_score, ".
            "review_name = '$review_name', ".
            "review_text = '$slashedText', ".
            "review_date = CURRENT_TIMESTAMP,".
            "review_game_count = game_count,".
            "review_game_seconds = game_total_seconds ".
            "WHERE email = '$email'; ";
        }
    
    rs_queryDatabase( $query );

    rs_generateRecentStatic();
    rs_generateTopPlaytimeStatic();
    rs_generateReviewCountStatic();
    
    echo "OK";
    }




function rs_removeReview() {
    global $tableNamePrefix, $rs_mysqlLink;
    

    $email = rs_requestFilter( "email", "/[A-Z0-9._%+\-]+@[A-Z0-9.\-]+/i", "" );

    $sequence_number = rs_requestFilter( "sequence_number", "/[0-9]+/i", "0" );

    $hash_value = rs_requestFilter( "hash_value", "/[A-F0-9]+/i", "" );

    $hash_value = strtoupper( $hash_value );
    

    if( $email == "" ) {
        rs_log( "removeReview denied for bad email" );

        echo "DENIED";
        return;
        }
    
    $trueSeq = rs_getSequenceNumberForEmail( $email );

    if( $trueSeq > $sequence_number || $trueSeq == 0 ) {
        // stale sequence number or user doesn't exist in DB
        rs_log( "removeReview denied for stale sequence number" );

        echo "DENIED";
        return;
        }
    

    $stringToHash = $sequence_number;


    $encodedString = urlencode( $stringToHash );
    $encodedEmail = urlencode( $email );

    global $ticketServerURL;
    
    $result = file_get_contents(
            "$ticketServerURL".
            "?action=check_ticket_hash".
            "&email=$encodedEmail" .
            "&hash_value=$hash_value" .
            "&string_to_hash=$encodedString" );

    if( $result != "VALID" ) {
        rs_log( "removeReview denied for failed hash check" );

        echo "DENIED";
        return;
        }

    // just set score as -1
    // keep old review text in place for reference
    
    $query = "UPDATE $tableNamePrefix"."user_stats SET " .
        "sequence_number = $sequence_number + 1, ".
        "review_score = -1 ".
        "WHERE email = '$email'; ";

    rs_queryDatabase( $query );

    rs_generateRecentStatic();
    rs_generateTopPlaytimeStatic();
    rs_generateReviewCountStatic();
    
    echo "OK";
    }



$rs_mysqlLink;


// general-purpose functions down here, many copied from seedBlogs

/**
 * Connects to the database according to the database variables.
 */  
function rs_connectToDatabase() {
    global $databaseServer,
        $databaseUsername, $databasePassword, $databaseName,
        $rs_mysqlLink;
    
    
    $rs_mysqlLink =
        mysqli_connect( $databaseServer, $databaseUsername, $databasePassword )
        or rs_operationError( "Could not connect to database server: " .
                              mysqli_error( $rs_mysqlLink ) );
    
    mysqli_select_db( $rs_mysqlLink, $databaseName )
        or rs_operationError( "Could not select $databaseName database: " .
                              mysqli_error( $rs_mysqlLink ) );
    }


 
/**
 * Closes the database connection.
 */
function rs_closeDatabase() {
    global $rs_mysqlLink;
    
    mysqli_close( $rs_mysqlLink );
    }


/**
 * Returns human-readable summary of a timespan.
 * Examples:  10.5 hours
 *            34 minutes
 *            45 seconds
 */
function rs_secondsToTimeSummary( $inSeconds ) {
    if( $inSeconds < 120 ) {
        if( $inSeconds == 1 ) {
            return "$inSeconds second";
            }
        return "$inSeconds seconds";
        }
    else if( $inSeconds < 3600 ) {
        $min = number_format( $inSeconds / 60, 0 );

        if( $min == 1 ) {
            return "$min minute";
            }
        
        return "$min minutes";
        }
    else {
        $hours = number_format( $inSeconds / 3600, 1 );

        if( $hours == 1 ) {
            return "$hours hour";
            }
        
        return "$hours hours";
        }
    }


/**
 * Returns human-readable summary of a distance back in time.
 * Examples:  10 hours
 *            34 minutes
 *            45 seconds
 *            19 days
 *            3 months
 *            2.5 years
 */
function rs_secondsToAgeSummary( $inSeconds ) {
    if( $inSeconds < 120 ) {
        return "$inSeconds seconds";
        }
    else if( $inSeconds < 3600 * 2 ) {
        $min = number_format( $inSeconds / 60, 0 );
        return "$min minutes";
        }
    else if( $inSeconds < 24 * 3600 * 2 ) {
        $hours = number_format( $inSeconds / 3600, 0 );
        return "$hours hours";
        }
    else if( $inSeconds < 24 * 3600 * 60 ) {
        $days = number_format( $inSeconds / ( 3600 * 24 ), 0 );
        return "$days days";
        }
    else if( $inSeconds < 24 * 3600 * 365 * 2 ) {
        // average number of days per month
        // based on 400 year calendar cycle
        // we skip a leap year every 100 years unless the year is divisible by 4
        $months = number_format( $inSeconds / ( 3600 * 24 * 30.436875 ), 0 );
        return "$months months";
        }
    else {
        // same logic behind computing average length of a year
        $years = number_format( $inSeconds / ( 3600 * 24 * 365.2425 ), 1 );
        return "$years years";
        }
    }



/**
 * Queries the database, and dies with an error message on failure.
 *
 * @param $inQueryString the SQL query string.
 *
 * @return a result handle that can be passed to other mysql functions.
 */
function rs_queryDatabase( $inQueryString ) {
    global $rs_mysqlLink;
    
    if( gettype( $rs_mysqlLink ) != "resource" ) {
        // not a valid mysql link?
        rs_connectToDatabase();
        }
    
    $result = mysqli_query( $rs_mysqlLink, $inQueryString );
    
    if( $result == FALSE ) {

        $errorNumber = mysqli_errno( $rs_mysqlLink );
        
        // server lost or gone?
        if( $errorNumber == 2006 ||
            $errorNumber == 2013 ||
            // access denied?
            $errorNumber == 1044 ||
            $errorNumber == 1045 ||
            // no db selected?
            $errorNumber == 1046 ) {

            // connect again?
            rs_closeDatabase();
            rs_connectToDatabase();

            $result = mysqli_query( $rs_mysqlLink, $inQueryString )
                or rs_operationError(
                    "Database query failed:<BR>$inQueryString<BR><BR>" .
                    mysqli_error( $rs_mysqlLink ) );
            }
        else {
            // some other error (we're still connected, so we can
            // add log messages to database
            rs_fatalError( "Database query failed:<BR>$inQueryString<BR><BR>" .
                           mysqli_error( $rs_mysqlLink ) );
            }
        }

    return $result;
    }



/**
 * Replacement for the old mysql_result function.
 */
function rs_mysqli_result( $result, $number, $field=0 ) {
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
function rs_doesTableExist( $inTableName ) {
    // check if our table exists
    $tableExists = 0;
    
    $query = "SHOW TABLES";
    $result = rs_queryDatabase( $query );

    $numRows = mysqli_num_rows( $result );


    for( $i=0; $i<$numRows && ! $tableExists; $i++ ) {

        $tableName = rs_mysqli_result( $result, $i, 0 );
        
        if( $tableName == $inTableName ) {
            $tableExists = 1;
            }
        }
    return $tableExists;
    }



function rs_log( $message ) {
    global $enableLog, $tableNamePrefix, $rs_mysqlLink;

    if( $enableLog ) {
        $slashedMessage = mysqli_real_escape_string( $rs_mysqlLink, $message );
    
        $query = "INSERT INTO $tableNamePrefix"."log VALUES ( " .
            "'$slashedMessage', CURRENT_TIMESTAMP );";
        $result = rs_queryDatabase( $query );
        }
    }



/**
 * Displays the error page and dies.
 *
 * @param $message the error message to display on the error page.
 */
function rs_fatalError( $message ) {
    //global $errorMessage;

    // set the variable that is displayed inside error.php
    //$errorMessage = $message;
    
    //include_once( "error.php" );

    // for now, just print error message
    $logMessage = "Fatal error:  $message";
    
    echo( $logMessage );

    rs_log( $logMessage );
    
    die();
    }



/**
 * Displays the operation error message and dies.
 *
 * @param $message the error message to display.
 */
function rs_operationError( $message ) {
    
    // for now, just print error message
    echo( "ERROR:  $message" );
    die();
    }


/**
 * Recursively applies the addslashes function to arrays of arrays.
 * This effectively forces magic_quote escaping behavior, eliminating
 * a slew of possible database security issues. 
 *
 * @inValue the value or array to addslashes to.
 *
 * @return the value or array with slashes added.
 */
function rs_addslashes_deep( $inValue ) {
    return
        ( is_array( $inValue )
          ? array_map( 'rs_addslashes_deep', $inValue )
          : addslashes( $inValue ) );
    }



/**
 * Recursively applies the stripslashes function to arrays of arrays.
 * This effectively disables magic_quote escaping behavior. 
 *
 * @inValue the value or array to stripslashes from.
 *
 * @return the value or array with slashes removed.
 */
function rs_stripslashes_deep( $inValue ) {
    return
        ( is_array( $inValue )
          ? array_map( 'rs_stripslashes_deep', $inValue )
          : stripslashes( $inValue ) );
    }



/**
 * Filters a $_REQUEST variable using a regex match.
 *
 * Returns "" (or specified default value) if there is no match.
 */
function rs_requestFilter( $inRequestVariable, $inRegex, $inDefault = "" ) {
    if( ! isset( $_REQUEST[ $inRequestVariable ] ) ) {
        return $inDefault;
        }

    return rs_filter( $_REQUEST[ $inRequestVariable ], $inRegex, $inDefault );
    }


/**
 * Filters a value  using a regex match.
 *
 * Returns "" (or specified default value) if there is no match.
 */
function rs_filter( $inValue, $inRegex, $inDefault = "" ) {
    
    $numMatches = preg_match( $inRegex,
                              $inValue, $matches );

    if( $numMatches != 1 ) {
        return $inDefault;
        }
        
    return $matches[0];
    }



// this function checks the password directly from a request variable
// or via hash from a cookie.
//
// It then sets a new cookie for the next request.
//
// This avoids storing the password itself in the cookie, so a stale cookie
// (cached by a browser) can't be used to figure out the password and log in
// later. 
function rs_checkPassword( $inFunctionName ) {
    $password = "";
    $password_hash = "";

    $badCookie = false;
    
    
    global $accessPasswords, $tableNamePrefix, $remoteIP, $enableYubikey,
        $passwordHashingPepper;

    $cookieName = $tableNamePrefix . "cookie_password_hash";

    $passwordSent = false;
    
    if( isset( $_REQUEST[ "passwordHMAC" ] ) ) {
        $passwordSent = true;

        // already hashed client-side on login form
        // hash again, because hash client sends us is not stored in
        // our settings file
        $password = rs_hmac_sha1( $passwordHashingPepper,
                                  $_REQUEST[ "passwordHMAC" ] );
        
        
        // generate a new hash cookie from this password
        $newSalt = time();
        $newHash = md5( $newSalt . $password );
        
        $password_hash = $newSalt . "_" . $newHash;
        }
    else if( isset( $_COOKIE[ $cookieName ] ) ) {
        rs_checkReferrer();
        $password_hash = $_COOKIE[ $cookieName ];
        
        // check that it's a good hash
        
        $hashParts = preg_split( "/_/", $password_hash );

        // default, to show in log message on failure
        // gets replaced if cookie contains a good hash
        $password = "(bad cookie:  $password_hash)";

        $badCookie = true;
        
        if( count( $hashParts ) == 2 ) {
            
            $salt = $hashParts[0];
            $hash = $hashParts[1];

            foreach( $accessPasswords as $truePassword ) {    
                $trueHash = md5( $salt . $truePassword );
            
                if( $trueHash == $hash ) {
                    $password = $truePassword;
                    $badCookie = false;
                    }
                }
            
            }
        }
    else {
        // no request variable, no cookie
        // cookie probably expired
        $badCookie = true;
        $password_hash = "(no cookie.  expired?)";
        }
    
        
    
    if( ! in_array( $password, $accessPasswords ) ) {

        if( ! $badCookie ) {
            
            echo "Incorrect password.";

            rs_log( "Failed $inFunctionName access with password:  ".
                    "$password" );
            }
        else {
            echo "Session expired.";
                
            rs_log( "Failed $inFunctionName access with bad cookie:  ".
                    "$password_hash" );
            }
        
        die();
        }
    else {
        
        if( $passwordSent && $enableYubikey ) {
            global $yubikeyIDs, $yubicoClientID, $yubicoSecretKey,
                $passwordHashingPepper;
            
            $yubikey = $_REQUEST[ "yubikey" ];

            $index = array_search( $password, $accessPasswords );
            $yubikeyIDList = preg_split( "/:/", $yubikeyIDs[ $index ] );

            $providedID = substr( $yubikey, 0, 12 );

            if( ! in_array( $providedID, $yubikeyIDList ) ) {
                echo "Provided Yubikey does not match ID for this password.";
                die();
                }
            
            
            $nonce = rs_hmac_sha1( $passwordHashingPepper, uniqid() );
            
            $callURL =
                "https://api2.yubico.com/wsapi/2.0/verify?id=$yubicoClientID".
                "&otp=$yubikey&nonce=$nonce";
            
            $result = trim( file_get_contents( $callURL ) );

            $resultLines = preg_split( "/\s+/", $result );

            sort( $resultLines );

            $resultPairs = array();

            $messageToSignParts = array();
            
            foreach( $resultLines as $line ) {
                // careful here, because = is used in base-64 encoding
                // replace first = in a line (the key/value separator)
                // with #
                
                $lineToParse = preg_replace( '/=/', '#', $line, 1 );

                // now split on # instead of =
                $parts = preg_split( "/#/", $lineToParse );

                $resultPairs[$parts[0]] = $parts[1];

                if( $parts[0] != "h" ) {
                    // include all but signature in message to sign
                    $messageToSignParts[] = $line;
                    }
                }
            $messageToSign = implode( "&", $messageToSignParts );

            $trueSig =
                base64_encode(
                    hash_hmac( 'sha1',
                               $messageToSign,
                               // need to pass in raw key
                               base64_decode( $yubicoSecretKey ),
                               true) );
            
            if( $trueSig != $resultPairs["h"] ) {
                echo "Yubikey authentication failed.<br>";
                echo "Bad signature from authentication server<br>";
                die();
                }

            $status = $resultPairs["status"];
            if( $status != "OK" ) {
                echo "Yubikey authentication failed: $status";
                die();
                }

            }
        
        // set cookie again, renewing it, expires in 24 hours
        $expireTime = time() + 60 * 60 * 24;
    
        setcookie( $cookieName, $password_hash, $expireTime, "/" );
        }
    }
 



function rs_clearPasswordCookie() {
    global $tableNamePrefix;

    $cookieName = $tableNamePrefix . "cookie_password_hash";

    // expire 24 hours ago (to avoid timezone issues)
    $expireTime = time() - 60 * 60 * 24;

    setcookie( $cookieName, "", $expireTime, "/" );
    }
 
 







function rs_hmac_sha1( $inKey, $inData ) {
    return hash_hmac( "sha1", 
                      $inData, $inKey );
    } 

 
function rs_hmac_sha1_raw( $inKey, $inData ) {
    return hash_hmac( "sha1", 
                      $inData, $inKey, true );
    } 


 
 
 
 
// decodes a ASCII hex string into an array of 0s and 1s 
function rs_hexDecodeToBitString( $inHexString ) {
    $digits = str_split( $inHexString );

    $bitString = "";

    foreach( $digits as $digit ) {
        $index = hexdec( $digit );

        $binDigitString = decbin( $index );

        // pad with 0s
        $binDigitString =
            substr( "0000", 0, 4 - strlen( $binDigitString ) ) .
            $binDigitString;

        $bitString = $bitString . $binDigitString;
        }

    return $bitString;
    }
 


 
?>
