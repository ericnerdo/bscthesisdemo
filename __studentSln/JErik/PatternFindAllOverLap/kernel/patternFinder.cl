/* Kernel for find-all*/
__kernel void findAllOverlap(__global const int* inArray, const int columns, __global const int* pattern, 
	const int patternLength, __global const int* hash, const int hashLength, __global int* results) {
	int row = get_global_id(0);
	
	int placeZero = row * columns;
	int mod = placeZero;
	int i = 0;
	
	while(mod <= (placeZero + columns - 1)) {
		if(inArray[mod + i] == pattern[i]) {
			if(i == (patternLength - 1)) {
				results[mod] = 1;
				mod += patternLength;
				i = 0;
			}
			else
				i++;
		}
		else {
			int nextHashedNumber = inArray[mod + patternLength];
			int nextMod = patternLength + 1;
			for(int k = 0; k < hashLength; k++)
				if(nextHashedNumber == hash[2 * k])
					nextMod = hash[2 * k + 1];
			mod += nextMod;
			i = 0;
		}
	}
	
}