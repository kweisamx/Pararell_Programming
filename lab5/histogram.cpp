#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include <fstream>
#include <iostream>
#include <vector>
#include <CL/cl.h>




cl_program load_program(cl_context context, const char *filename, cl_device_id devices){
    std::ifstream in(filename, std::ios_base::binary);
    if(!in.good()){
        return 0;
    }
  
    // get file length 
    in.seekg(0, std::ios_base::end);
    size_t length = in.tellg();
    in.seekg(0, std::ios_base::beg);
  
    // read program source 
    std::vector<char> data(length + 1);
    in.read(&data[0], length);
    data[length] = 0;
  
    // create and build program
    const char *source = &data[0];
    cl_program program = clCreateProgramWithSource(context, 1, &source, 0, 0);
    if(program == 0){
        char *buff_err;
        cl_int errcode;
        size_t build_log_len;
        errcode = clGetProgramBuildInfo(program, devices, CL_PROGRAM_BUILD_LOG, 0, NULL, &build_log_len);
        if(errcode){
            std::cout<<"clGetProgramBuildInfo failed at line"<<errcode<<std::endl;
        }
        return 0;
    }
    if(clBuildProgram(program, 0, 0, 0, 0, 0) != CL_SUCCESS){
        char *buff_err;
        cl_int errcode;
        size_t build_log_len;
        errcode = clGetProgramBuildInfo(program, devices, CL_PROGRAM_BUILD_LOG, build_log_len, buff_err, &build_log_len);
        printf("%d\n",errcode);
        if(errcode){
            std::cout<<"clGetProgramBuildInfo failed at line "<<errcode<<std::endl;
        }
        return 0;
    }
    return program;
     
}
unsigned int * histogram(unsigned int *image_data, unsigned int _size) {

	unsigned int *img = image_data;
	unsigned int *ref_histogram_results;
	unsigned int *ptr;

	ref_histogram_results = (unsigned int *)malloc(256 * 3 * sizeof(unsigned int));
	ptr = ref_histogram_results;
	memset (ref_histogram_results, 0x0, 256 * 3 * sizeof(unsigned int));

	// histogram of R
	for (unsigned int i = 0; i < _size; i += 3)
	{
		unsigned int index = img[i];
		ptr[index]++;
	}

	// histogram of G
	ptr += 256;
	for (unsigned int i = 1; i < _size; i += 3)
	{
		unsigned int index = img[i];
		ptr[index]++;
	}

	// histogram of B
	ptr += 256;
	for (unsigned int i = 2; i < _size; i += 3)
	{
		unsigned int index = img[i];
		ptr[index]++;
	}

	return ref_histogram_results;
}

int main(int argc, char const *argv[])
{
    
  /* Get platform number */
    cl_int err;
    cl_uint num;
    err = clGetPlatformIDs(0, 0, &num);
    if(err != CL_SUCCESS){
        std::cerr<<"Unable to get platforms\n";
        return 0;
    }
    /* Get platform ID*/
    std::vector<cl_platform_id> platforms(num);
    err = clGetPlatformIDs(num, &platforms[0], &num);
    if(err != CL_SUCCESS){
        std::cerr << "Unable to get platform ID\n";
        return 0;
    }
    /* Create OpenCL context */
    cl_context_properties prop[] = { CL_CONTEXT_PLATFORM, reinterpret_cast<cl_context_properties>(platforms[0]), 0 };
    cl_context context = clCreateContextFromType(prop, CL_DEVICE_TYPE_DEFAULT, NULL, NULL, NULL);
    if(context == 0){
        std::cerr << "Can't create OpenCL context\n";
        return 0;
    }

    /* Get Device Info */
    size_t cb;
    clGetContextInfo(context, CL_CONTEXT_DEVICES, 0, NULL, &cb);
    std::vector<cl_device_id> devices(cb / sizeof(cl_device_id));
    clGetContextInfo(context, CL_CONTEXT_DEVICES, cb, &devices[0], 0);

    /* Get Device Name */
    clGetDeviceInfo(devices[0], CL_DEVICE_NAME, 0, NULL, &cb);
    std::string devname;
    devname.resize(cb);
    clGetDeviceInfo(devices[0], CL_DEVICE_NAME, cb ,&devname[0], 0);
    std::cout << "Device:" << devname.c_str() << "\n";

    /* Create Command Queue */
    cl_int ret;
    cl_command_queue queue = clCreateCommandQueue(context, devices[0], 0, &ret);
    if(queue == 0){
        std::cerr << "Can't create command queue \n";
        clReleaseContext(context);
        return 0;
    }



    /* Start program */
	unsigned int * histogram_results;
	unsigned int i=0, a, input_size;
	std::fstream inFile("input", std::ios_base::in);
	std::ofstream outFile("0556148.out", std::ios_base::out);

    /* Get Input Size */
	inFile >> input_size;
	unsigned int *image = new unsigned int[input_size];

    /* Get Input Data */
	while( inFile >> a ) {
		image[i++] = a;
	}

    /* Set Memory and Copy Data */
    cl_mem cl_img = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(cl_float) * input_size, &image[0],NULL);
    cl_mem cl_size = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(cl_int), &input_size,NULL);
    cl_mem cl_res = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(cl_float) * input_size, NULL, NULL);

    if(cl_img == 0 || cl_res == 0){
        std::cerr << "Can't create OpenCL buffer\n";
        clReleaseMemObject(cl_img);
        clReleaseMemObject(cl_size);
        clReleaseMemObject(cl_res);
        clReleaseCommandQueue(queue);
        clReleaseContext(context);
        return 0;
    }

    /* load program */
    cl_program program = load_program(context, "histogram.cl", devices[0]);
    if(program == 0){
        std::cerr << "can't load or build program\n";
        clReleaseMemObject(cl_img);
        clReleaseMemObject(cl_size);
        clReleaseMemObject(cl_res);    
        clReleaseCommandQueue(queue);
        clReleaseContext(context);
        return 0;
    }

    /* load adder */
    
    cl_kernel clean = clCreateKernel(program, "clean", 0);
    if(clean == 0){
        std::cerr << "can't load kernel\n";
        clReleaseMemObject(cl_img);
        clReleaseMemObject(cl_size);
        clReleaseMemObject(cl_res);    
        clReleaseCommandQueue(queue);
        clReleaseContext(context);
        return 0;
    }


    /* run the kernel */
    clSetKernelArg(clean, 0, sizeof(cl_mem), &cl_img);
    clSetKernelArg(clean, 1, sizeof(cl_mem), &cl_res);
    clSetKernelArg(clean, 2, sizeof(cl_mem), &cl_size);

    size_t work_size = input_size;
    err = clEnqueueNDRangeKernel(queue, clean, 1, 0, &work_size, 0, 0, 0, 0);
    printf("err :%d\n",err);


	histogram_results = histogram(image, input_size);
	for(unsigned int i = 0; i < 256 * 3; ++i) {
		if (i % 256 == 0 && i != 0)
			outFile << std::endl;
		outFile << histogram_results[i]<< ' ';
	}


	inFile.close();
	outFile.close();

	return 0;
}
