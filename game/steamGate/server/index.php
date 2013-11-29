
<?php

include( "settings.php" );


global $enableYubikey;

?>

<FORM ACTION="server.php" METHOD="post">
    <INPUT TYPE="password" MAXLENGTH=20 SIZE=20 NAME="password">
<?php

if( $enableYubikey ) {
?>
    <br>
    Yubikey:<br>
    <INPUT TYPE="text" MAXLENGTH=48 SIZE=48 NAME="yubikey">

<?php
    }
?>

    <INPUT TYPE="hidden" NAME="action" VALUE="show_data">
	        <INPUT TYPE="Submit" VALUE="login">
    </FORM>