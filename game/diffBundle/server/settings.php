<?php




// should not be web-accessible
$downloadFilePath = "/home/thomasbailey/diffDownloads/";

// files in the download path must be named like:
// 5_inc_mac.dbz
// 5_full_mac.dbz
// 5_inc_win.dbz
// 5_full_win.dbz
// 5_inc_all.dbz
// 5_full_all.dbz

// [version number}_[inc or full]_[platform code].dbz

// these are diff bundle binary files to be served directly


// OR

// 5_inc_mac_urls.txt
// 5_full_mac_urls.txt
// 5_inc_win_urls.txt
// 5_full_win_urls.txt
// 5_inc_all_urls.txt
// 5_full_all_urls.txt

// these are URL list files for mirrors hosting .dbz files


// also, to limit which file versions are served (while building
// newer versions):

// latest.txt  can contain the latest version number


?>