__kernel void pro(__global const unsigned int *img, __global unsigned int *result, __global const unsigned int *size){
    unsigned int idx;

	for (idx = 0; idx < 768; ++idx)
		result[idx] = 0;

	for (idx = 0; idx < *size; idx += 3)
		result[img[idx]]++;

	result += 256;
	for (idx = 1; idx < *size; idx += 3)
		result[img[idx]]++;

	result += 256;
	for (idx = 2; idx < *size; idx += 3)
		result[img[idx]]++;
    
}
