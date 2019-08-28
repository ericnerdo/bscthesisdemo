__kernel void longestSubSeq2(__global const int* inputMatrix, __global int* outputArray, const int blocksize, const int seeked) {

	int globalID = get_global_id(0);	// Getting global id.

	int row = globalID;

	int head = 0;
	int longest = 0;
	int longestSpot = 0;
	int tail = 0;
	
	int streak = 0;

	
	while(1) {
		if(inputMatrix[row * blocksize + head] == seeked)
			head++;
		else 
			break;
	} 
		
	while(1) {
		if(inputMatrix[(row + 1) * blocksize - tail - 1] == seeked)
			tail++;
		else
			break;
	}
	
	for (int i = 0; i < blocksize; i++) {
		if (inputMatrix[row * blocksize + i] == seeked) {
			if (streak == 0)
				longestSpot = i;

			streak++;
			if (streak > longest)
				longest = streak;
		}
		else {
			streak = 0; 
		}
	}
	outputArray[4 * row] = head;
	outputArray[4 * row + 1] = longest;
	outputArray[4 * row + 2] = longestSpot;
	outputArray[4 * row + 3] = tail;

}