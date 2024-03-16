<?php
// 检查是否有 POST 数据
if ($_SERVER["REQUEST_METHOD"] == "POST") {
    // 打印 name 和 age 字段
    if(isset($_POST['name']) && isset($_POST['age'])) {
        echo "Name: " . $_POST['name'] . "<br>";
        echo "Age: " . $_POST['age'];
    } else {
        echo "Name or Age is not set in POST request.";
    }
} else {
    echo "No POST data received.";
}
?>
