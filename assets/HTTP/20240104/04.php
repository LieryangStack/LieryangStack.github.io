<?php
    // 确保在发送任何输出之前调用 header 函数
    header('Location: http://www.baidu.com');
    exit; // 在重定向后停止脚本的执行
?>
