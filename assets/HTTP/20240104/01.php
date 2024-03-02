<?php
if (isset($_GET['name']) && isset($_GET['age'])) {
    // 如果 name 和 age 变量都存在
    $name = $_GET['name'];
    $age = $_GET['age'];

    // 打印 name 和 age
    echo "Name: " . $name . ", Age: " . $age;
} else {
    // 如果 name 和 age 变量中有任何一个不存在
    echo "没有输入";
}
?>