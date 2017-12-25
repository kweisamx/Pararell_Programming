#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <CL/opencl.h>

const char *histogram = "\
__kernel void histogram(\
	__global unsigned char *img,\
	unsigned int count,\
	__global unsigned int *result)\
{\
	const int idx = get_global_id(0);\
    int i;\
    if(idx<256){\
        for(i = 0;i < count; i++){\
            if(i%3 == 0 && idx == img[i]){\
                result[idx]++;\
            }\
        }\
    }\
    if(256 <= idx && idx < 512){\
        for(i = 0;i < count; i++){\
            if(i%3 == 1 && (idx-256) == img[i]){\
                result[idx+256]++;\
            }\
        }\
    }\
    if(512 <= idx && idx < 768){\
        for(i = 0;i < count; i++){\
            if(i%3 == 2 && (idx-256) == img[i]){\
                result[idx+512]++;\
            }\
        }\
    }\
}\
";

int main(int argc, char const *argv[])
{
	cl_int err;

	cl_uint num_device, num;
	cl_device_id device;
	cl_platform_id platform_id;
	err = clGetPlatformIDs(1, &platform_id, &num);
	err = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_GPU, 1, &device,  &num_device);
	cl_context context = clCreateContext(NULL, 1, &device, NULL, NULL, &err);
	cl_command_queue queue = clCreateCommandQueue(context, device, 0, &err);

	FILE *inFile = fopen("input", "r");
	FILE *outFile = fopen("0556148.out", "w");

	unsigned int i = 0, a, input_size;

	fscanf(inFile, "%u", &input_size);

	cl_mem img = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(unsigned char) * input_size, NULL, &err);
	cl_mem his_res = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(unsigned int) * 256 * 3, NULL, &err);
	unsigned char *tmp = (unsigned char *) malloc(sizeof(unsigned char) * input_size);
	unsigned int *result = (unsigned int *) malloc(sizeof(unsigned int) * 256 * 3);

	memset(result, 0x00, sizeof(unsigned int) * 256 * 3);
	clEnqueueWriteBuffer(queue, his_res, CL_TRUE, 0, sizeof(unsigned int) * 256 * 3, result, 0, NULL, NULL);
	
    while( fscanf(inFile, "%u", &a) != EOF ) {
		tmp[i++] = a;
	}
	err = clEnqueueWriteBuffer(queue, img, CL_TRUE, 0, sizeof(unsigned char) * input_size, tmp, 0, NULL, NULL);

	size_t source_size = strlen(histogram);
	unsigned int input_count = input_size/3;
	cl_program myprog = clCreateProgramWithSource(context, 1, (const char **) &histogram, &source_size, &err);
	err = clBuildProgram(myprog, 1, &device, NULL, NULL, NULL);
	cl_kernel mykernel = clCreateKernel(myprog, "histogram", &err);
	clSetKernelArg(mykernel, 0, sizeof(cl_mem), &img);
	clSetKernelArg(mykernel, 1, sizeof(unsigned int), &input_count);
	clSetKernelArg(mykernel, 2, sizeof(cl_mem), &his_res);

	size_t worksize = 768;
	err= clEnqueueNDRangeKernel(queue, mykernel, 1, 0, &worksize, 0, 0, NULL, NULL);

	err = clEnqueueReadBuffer(queue, his_res, CL_TRUE, 0, sizeof(unsigned int) * 256 * 3, result, NULL, NULL, NULL);
	for(i = 0; i < 256 * 3; ++i) {
		if (i % 256 == 0 && i != 0)
			fprintf(outFile, "\n");
		fprintf(outFile, "%u ", result[i]);
    }
	fclose(inFile);
	fclose(outFile);

	return 0;
}
