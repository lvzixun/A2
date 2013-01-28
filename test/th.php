
<?php
	$al = array();
	$count = 5000;
	for ($i=0; $i < $count; $i++) { 
		$al[$i] = $count-$i;
	}

	echo "-------\n";
	$b = microtime (true);

	
	for ($i=0; $i < $count; $i++) { 
		$min = $al[$i];
		for ($j=$i; $j < $count; $j++) { 
			if($min>$al[$j]){
				$min = $al[$j];
				$al[$j] = $al[$i];
				$al[$i] = $min;
			}
		}
	}



	$e = microtime (true);
	printf("%.4f\n", $e-$b);
?>