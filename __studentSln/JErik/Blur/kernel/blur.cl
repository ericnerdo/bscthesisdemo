__kernel void blur(__global const int* inputMatrix, __global int* outputMatrix, const int rows, const int columns) {

	int globalID = get_global_id(0);	// Getting global id.

	// Init i and j for indexing the array.
	int i = globalID / columns;
	int j = globalID % columns;

	
	int neighbours = 1; 
	double neighbourSum = inputMatrix[i * columns + j] / 2.0;

	// Computing neighbours if they exist.
	if ((i - 1) >= 0) {
		neighbours++;
		neighbourSum += inputMatrix[(i - 1) * columns + j] / 4.0;
	}
	if (((i - 1) >= 0) && ((j - 1) >= 0)) {
		neighbours++;
		neighbourSum += inputMatrix[(i - 1) * columns + j - 1] / 8.0;
	}
	if ((j - 1) >= 0) {
		neighbours++;
		neighbourSum += inputMatrix[i * columns + j - 1] / 4.0;
	}
	if (((i + 1) <= rows) && ((j - 1) >= 0)) {
		neighbours++;
		neighbourSum += inputMatrix[(i + 1) * columns + j - 1] / 8.0;
	}
	if ((i + 1) <= rows) {
		neighbours++;
		neighbourSum += inputMatrix[(i + 1) * columns + j] / 4.0;
	}
	if (((i + 1) <= rows) && ((j + 1) <= columns)) {
		neighbours++;
		neighbourSum += inputMatrix[(i + 1) * columns + j + 1] / 8.0;
	}
	if ((j + 1) <= columns) {
		neighbours++;
		neighbourSum += inputMatrix[i * columns + j + 1] / 4.0;
	}
	if (((i - 1) <= 0) && ((j + 1) <= columns)) {
		neighbours++;
		neighbourSum += inputMatrix[(i - 1) * columns + j + 1] / 8.0;
	}

	outputMatrix[i * columns + j] = (int)(neighbourSum / (2 * neighbours));
} 