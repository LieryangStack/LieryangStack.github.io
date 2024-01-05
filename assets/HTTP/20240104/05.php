<?php
// 检查是否有 POST 请求
if ($_SERVER["REQUEST_METHOD"] == "POST") {

  header('Location: 06.php', true, 307);

    // 获取并打印名字和姓氏字段
    $fname = isset($_POST['fname']) ? htmlspecialchars($_POST['fname']) : '';
    $lname = isset($_POST['lname']) ? htmlspecialchars($_POST['lname']) : '';

    echo "名字: " . $fname . "<br>";
    echo "姓氏: " . $lname;
}
?>
