
#include <stdio.h>

#include <time.h>
#include <sys/time.h>
#include <unistd.h>

static int a[5000];

inline double t_time(){
	struct timeval st;
	gettimeofday(&st, 0);
	return (st.tv_sec*1000000+st.tv_usec)/1000000.0;
}

int main(int argc, char const *argv[])
{
	int i, j, min;
	printf("----\n");
	double s = t_time();
	for(i=0; i<5000; i++){
		min = a[i];
		for(j=0; j<5000; j++){
			if(min>a[j]){
				min = a[j];
				a[j] = a[i];
				a[i] = min;
			}
		}
	}
	double e = t_time();

	printf("time = %lf\n", e-s);
	return 0;
}