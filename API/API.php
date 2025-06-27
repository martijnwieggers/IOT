<?php
header('Content-Type: application/json');

// Database instellingen
$servername = "localhost";
$username = "gebruiker";
$password = "wachtwoord";
$dbname = "test";

// Probeer verbinding met de database te maken
try {
    $dsn = "mysql:host=$servername;dbname=$dbname;charset=utf8mb4";
    $options = [
        PDO::ATTR_ERRMODE => PDO::ERRMODE_EXCEPTION,
        PDO::ATTR_DEFAULT_FETCH_MODE => PDO::FETCH_ASSOC,
        PDO::ATTR_EMULATE_PREPARES => false,
    ];

    $pdo = new PDO($dsn, $username, $password, $options);
} catch (PDOException $e) {
    die(json_encode(["error" => "Connection failed: " . $e->getMessage()]));
}

// Functie om de query-tekst te maken
function getQuery($start, $end) {
    if ($start && $end) {
        return "SELECT * FROM Data WHERE Timestamp >= :start AND Timestamp <= :end ORDER BY Timestamp DESC LIMIT 10";
    } else {
        return "SELECT * FROM Data ORDER BY Timestamp DESC LIMIT 1";
    }
}

// Verkrijg optionele datumbereik van de query parameters
$startDate = isset($_GET['start']) ? $_GET['start'] : null;
$endDate = isset($_GET['end']) ? $_GET['end'] : null;

try {
    // Opstellen en voorbereiden van de SQL-query
    $sql = getQuery($startDate, $endDate);
    $stmt = $pdo->prepare($sql);
    
    // Bind de parameters indien nodig en voer de query uit
    if ($startDate && $endDate) {
        $stmt->execute(['start' => $startDate, 'end' => $endDate]);
    } else {
        $stmt->execute();
    }
    
    $data = $stmt->fetchAll();
    echo json_encode($data);
} catch (PDOException $e) {
    echo json_encode(["error" => $e->getMessage()]);
}
?>
