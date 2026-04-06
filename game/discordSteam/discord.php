<?php

include_once( "settings.php" );


// returns true if message sent
// false otherwise
function ds_discordSendMessageToChannel( $inChannelID,
                                         $inMessageText,
                                         $inAttachFilePath = "" ) {
    $endpoint =
        "https://discordapp.com/api".
        "/channels/$inChannelID/messages";

    $fileName = "";
    $attachPart = "";

    if( $inAttachFilePath != "" ) {
        $fileName = basename( $inAttachFilePath );

        $attachPart = ", \"attachments\": ".
            "[{ \"id\": 0, ".
            "\"description\": \"Attached file\", ".
            "\"filename\": \"$fileName\"} ]";
        }
    
    
    
    $postBody =
        "{ \"content\": ".
        "\"$inMessageText\" $attachPart }";
    
    $result =
        ds_discordAPICall( $endpoint,
                           "POST",
                           $postBody,
                           $fileName, $inAttachFilePath );

    if( $result[0] == 200 ) {
        return true;
        }
    else {
        return false;
        }
    }




// $inBody, if not empty string, must be JSON
//
// If a file is attached, both params must be specified
// $inAttachFileName is the path-free presentation like myTest.png
// $inAttachFilePath is the filesystem path of the data file
// Note that inBody JSON must contain:
// "attachments": [{
//      "id": 0,
//      "description": "Test image file",
//      "filename": "myTest.png"} ]
//
// with "filename" matching $inAttachFileName
//
// returns array with response code and response body
function ds_discordAPICall( $inEndpointURL, $inMethod, $inBody = "",
                            $inAttachFileName = "",
                            $inAttachFilePath = "" ) {
    global $discordBotToken;

    $contentType = "application/json";
    $content = $inBody;
    
    if( $inAttachFileName != "" ) {

        $boundary = "3892343204323479876734897689348";
        
        $contentType = "multipart/form-data; boundary=$boundary";

        $fileContents = file_get_contents( $inAttachFilePath );

        $fileMIME = mime_content_type( $inAttachFilePath );
        
        $content =
            "--$boundary\r\n".
            "Content-Disposition: form-data; name=\"payload_json\"\r\n".
            "Content-Type: application/json\r\n\r\n".
            $inBody .
            "\r\n--$boundary\r\n".
            "Content-Disposition: form-data; name=\"files[0]\"; ".
              "filename=\"$inAttachFileName\"\r\n".
            "Content-Type: $fileMIME\r\n\r\n".
            $fileContents .
            "\r\n--$boundary--";
        }
    
        
    
    $httpArray = array(
        'header'  =>
        "Authorization: Bot $discordBotToken\r\n".
        "Connection: close\r\n".
        "Content-type: $contentType\r\n".
        "Content-Length: " . strlen( $content ) . "\r\n",
        'method'  => $inMethod,
        'protocol_version' => 1.1,
        'content' => $content );

    $options = array( 'http' => $httpArray );
    $context  = stream_context_create( $options );
    $result = file_get_contents( $inEndpointURL, false, $context );

    // ds_log( var_export( $http_response_header, true ) );

    $status_line = $http_response_header[0];

    preg_match('{HTTP\/\S*\s(\d{3})}', $status_line, $match );

    $responseCode = $match[1];
    
    return array( $responseCode, $result );
    }


?>
