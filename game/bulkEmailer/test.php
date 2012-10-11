<?php

include( "bulkEmailerAPI.php" );


be_addMessage( "Test Message",
               "This is another test message",
               array( "bob@asfdsd.com",
                      "sam@74asdf.com", "bill@dsfaasd.com" ) );
?>