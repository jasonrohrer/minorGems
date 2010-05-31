# a brute-force, naive prime detector
def isPrime( inNumToCheck ):
	for i in range( 2, inNumToCheck ) :
		if inNumToCheck % i == 0 :
			return 0
	return 1

limit = 10000

numPrimes = 0	

for i in range( 5, limit ) :
	if isPrime( i ) :
		numPrimes = numPrimes + 1
			
print numPrimes	

