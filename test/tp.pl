my@array = ();

my$NUM=5000;

for ($count=1; $count <= $NUM; $count++){
	push(@array, $NUM-$count);
}

use Time::HiRes qw(gettimeofday);
($start_sec, $start_usec) = gettimeofday;

#print @array, "\n";
for (my $i = 0; $i < $NUM; $i++) {
	my $min = $array[$i];
	for (my $j = $i; $j < $NUM; $j++){
		if($min>$array[$j]){
			$min = $array[$j];
			$array[$j] = $array[$i];
			$array[$i] = $min;
		}
	}
}

($end_sec, $end_usec) = gettimeofday;
$time_used = ($end_sec - $start_sec) + ($end_usec - $start_usec)/1000000;
printf("time used  : %.3f\n", $time_used);

# print @array, "\n";