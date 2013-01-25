
import time
array = []

for i in xrange(0, 5000):
	array.append(5000-i)

print('----begin sort----')

s = time.time()
for i in xrange(0,5000):
	min = array[i]
	for j in xrange(i,5000):
		if min>array[j] :
			min = array[j]
			array[j] = array[i]
			array[i] = min

e = time.time()
print('time = ', e-s)