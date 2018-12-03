<?php

$yubicoClientID = $_REQUEST[ "id" ];

$yubikey = $_REQUEST[ "otp" ];

$nonce = $_REQUEST[ "nonce" ];

$callURL =
    "https://api2.yubico.com/wsapi/2.0/verify?id=$yubicoClientID".
    "&otp=$yubikey&nonce=$nonce";

$result = file_get_contents( $callURL );

echo "$result";
?>