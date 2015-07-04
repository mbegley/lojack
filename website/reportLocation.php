<?php
$servername = "localhost";
$username = "DBUSERNAME";
$password = "DBPASSWORD";
$database = "lojack";

$latitude = htmlspecialchars($_GET["latitude"]);
$longitude = htmlspecialchars($_GET["longitude"]);
$deviceId = htmlspecialchars($_GET["deviceId"]);

echo "latitude: $latitude <br>";
echo "longitude: $longitude <br>";
echo "deviceId: $deviceId <br>";

try {
    $dbConnection = new PDO("mysql:host=$servername;dbname=$database", $username, $password);
    // set the PDO error mode to exception
    $dbConnection->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
//    echo "Connected successfully<p>";

    $sql = "INSERT INTO gpsUpdates (latitude,longitude,deviceId,timestamp) values ($latitude,$longitude,$deviceId,now())";
    $result = $dbConnection->query($sql);

    $dbConnection = null;
}
catch(PDOException $e)
{
	echo "Connection failed: " . $e->getMessage();
}
?>
