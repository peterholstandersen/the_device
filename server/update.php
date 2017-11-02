<?PHP

header('Content-type: text/plain; charset=utf8', true);

function check_header($name, $value = false) {
    if(!isset($_SERVER[$name])) {
        return false;
    }
    if($value && $_SERVER[$name] != $value) {
        return false;
    }
    return true;
}

function sendFile($path) {
    header($_SERVER["SERVER_PROTOCOL"].' 200 OK', true, 200);
    header('Content-Type: application/octet-stream', true);
    header('Content-Disposition: attachment; filename='.basename($path));
    header('Content-Length: '.filesize($path), true);
    header('x-MD5: '.md5_file($path), true);
    readfile($path);
}

if(!check_header('HTTP_USER_AGENT', 'ESP8266-http-Update')) {
    header($_SERVER["SERVER_PROTOCOL"].' 403 Forbidden', true, 403);
    echo "only for ESP8266 updater!\n";
    # error_log("only for ESP8266 updater!\n");
    exit();
}

if(
    !check_header('HTTP_X_ESP8266_STA_MAC') ||
    !check_header('HTTP_X_ESP8266_AP_MAC') ||
    !check_header('HTTP_X_ESP8266_FREE_SPACE') ||
    !check_header('HTTP_X_ESP8266_SKETCH_SIZE') ||
    !check_header('HTTP_X_ESP8266_CHIP_SIZE') ||
    !check_header('HTTP_X_ESP8266_SDK_VERSION') ||
    !check_header('HTTP_X_ESP8266_VERSION')
) {
    header($_SERVER["SERVER_PROTOCOL"].' 403 Forbidden', true, 403);
    echo "only for ESP8266 updater! (header)\n";
    # error_log("only for ESP8266 updater! (header)\n");
    exit();
}

$db = array(
    "5C:CF:7F:C4:07:E2" => "the_device.ino.bin",
    # "5C:CF:7F:C4:07:E2" => "BasicOTA.ino.bin",
    # "5E:CF:7F:C4:07:E2" => "BasicOTA.ino.bin"
);

if (isset($db[$_SERVER['HTTP_X_ESP8266_STA_MAC']])) {
  $file = $db[$_SERVER['HTTP_X_ESP8266_STA_MAC']];
  $file_md5 = shell_exec("md5sum $file");
  $device_md5 = $_SERVER['HTTP_X_ESP8266_SKETCH_MD5'];

  # error_log("file_md5: $file_md5\n");
  # error_log("file: $file\n");
  # error_log("device_md5: $device_md5\n");
  
  if(strncmp($device_md5, $file_md5, 32)) {
    # error_log("Updating\n");
    sendFile("./".$db[$_SERVER['HTTP_X_ESP8266_STA_MAC']]);
  } else {
    # error_log("Not modified\n");
    header($_SERVER["SERVER_PROTOCOL"].' 304 Not Modified', true, 304);
  }
  exit();
}

# error_log("No version available\n");

header($_SERVER["SERVER_PROTOCOL"].' 500 no version for ESP MAC', true, 500);
