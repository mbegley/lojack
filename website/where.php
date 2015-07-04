<html>
<head>
   <script type="text/javascript" 
           src="http://maps.google.com/maps/api/js?sensor=false"></script>
   <meta http-equiv="refresh" content="30">
</head> 
<body>

<h1>Where's Sandy?</h1>
<?php
$servername = "localhost";
$username = "DBUSERNAME";
$password = "DBPASSWORD";
$database = "lojack";

try {
    $dbConnection = new PDO("mysql:host=$servername;dbname=$database", $username, $password);
    // set the PDO error mode to exception
    $dbConnection->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);

    $sql = "select * from gpsUpdates ORDER BY id DESC LIMIT 1";
    $result = $dbConnection->query($sql);
    if ($result->rowcount() > 0) {
       $row=$result->fetch();

       echo "latitude: " . $row["latitude"]. " longitude: " . $row["longitude"]. "<br><p>";
    }

    $dbConnection = null;
}
catch(PDOException $e)
{
	echo "Connection failed: " . $e->getMessage();
}
?>
   <div id="map" style="width: 800px; height: 600px"></div> 

   <script>
     
      var myOptions = {
         zoom: 17,
         center: new google.maps.LatLng(<?php echo $row["latitude"].",".$row["longitude"];?>),
         mapTypeId: google.maps.MapTypeId.ROADMAP
      };
      var image={
        url:'http://FQDN.example.com/CarIcon.png',
        size: new google.maps.Size(40,40),
        origin: new google.maps.Point(0,0),
        anchor: new google.maps.Point(20,20)
      };

      var map = new google.maps.Map(document.getElementById("map"), myOptions);
      var marker = new google.maps.Marker({
        position: new google.maps.LatLng(<?php echo $row["latitude"].",".$row["longitude"];?>),
        map:map,
        icon:image,
        title: 'Sandy'
      });
   </script> 

</body>
</html>
