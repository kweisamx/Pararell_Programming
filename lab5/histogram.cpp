#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <vector>
#include <CL/cl.h>


int main(int argc, char const *argv[])
{
    
    /* Start program */
    std::ifstream in("histogram.cl", std::ios_base::binary);
	std::fstream inFile("input", std::ios_base::in);
	std::fstream outFile("0556148.out", std::ios_base::out);
	unsigned int i=0, a, input_size;

    /* Get Input Size */
	inFile >> input_size;
	unsigned int *image = new unsigned int[input_size];
	unsigned int result[768];

    /* Get Input Data */
	while( inFile >> a ) {
		image[i++] = a;
	}

    /* Load Program Data */
    // get file length
    in.seekg(0, std::ios_base::end);
    size_t length = in.tellg();
    in.seekg(0, std::ios_base::beg);

    // read program source
    std::vector<char> data(length + 1);
    in.read(&data[0], length);
    data[length] = 0;

    // create and build program 
    const char* source = &data[0];

    cl_device_id device_id = 0;
    cl_int err = 0;
    cl_uint num = 0;
    cl_platform_id platform_id = 0;
    cl_uint device_num = 0;


    /* Get platform ID */
    err = clGetPlatformIDs(1, &platform_id, &num);
    if(err != CL_SUCCESS){
        std::cerr<<"Unable to get platforms\n";
        return 0;
    }
    std::cout<<"GET platform ID is OK!"<<std::endl;

    /* Get deivce ID*/
    err = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_DEFAULT, 1, &device_id, &device_num);
    if(err != CL_SUCCESS){
        std::cerr << "Unable to get platform ID\n";
        return 0;
    }
    std::cout<<"GET Device is OK!"<<std::endl;

    /* Create OpenCL context */
    cl_context context = clCreateContext(0, 1, &device_id, NULL, NULL, &err);
    if(err != 0){
        std::cerr << "Can't create OpenCL context\n";
        return 0;
    }
    std::cout<<"Creating OpenCL context is OK!"<<std::endl;

    /* Create Command Queue */
    cl_command_queue queue = clCreateCommandQueue(context, device_id, 0, &err);
    if(err != 0){
        std::cerr << "Can't create command queue ,error is "<< err << std::endl;
        clReleaseContext(context);
        return 0;
    }
    std::cout<<"Creating OpenCL Command Queue is OK!"<<std::endl;



    /* Set Memory and Copy Data */
    cl_mem cl_img = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(unsigned int) * input_size, &image[0],&err);
    if(err != 0){
        std::cerr << "Can't create buffer ,error is "<< err << std::endl;
        clReleaseContext(context);
        return 0;
    }
    std::cout<<"Creating OpenCL buffer is OK!"<<std::endl;

    cl_mem cl_res = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(unsigned int) * 768, NULL, &err);
    if(err != 0){
        std::cerr << "Can't create buffer ,error is "<< err << std::endl;
        clReleaseContext(context);
        return 0;
    }
    std::cout<<"Creating OpenCL buffer is OK!"<<std::endl;

    cl_mem cl_size = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(unsigned int), &input_size, &err);
    if(err != 0){
        std::cerr << "Can't create buffer ,error is "<< err << std::endl;
        clReleaseContext(context);
        return 0;
    }
    std::cout<<"Creating OpenCL buffer is OK!"<<std::endl;

    /* load program */
    cl_program program = clCreateProgramWithSource(context, 1, &source, NULL, &err);
    if(program == 0 && err != 0){
        std::cerr << "can't load program, err is "<< err <<std::endl;;
        clReleaseMemObject(cl_img);
        clReleaseMemObject(cl_size);
        clReleaseMemObject(cl_res);    
        clReleaseCommandQueue(queue);
        clReleaseContext(context);
        return 0;
    }
    std::cout<<"Load Program is OK!"<<std::endl;

    err = clBuildProgram(program, 1, &device_id, "-Werror", 0, 0);
	if (err != 0) {
        std::cerr << "can't build program, err is "<< err <<std::endl;;
        clReleaseMemObject(cl_img);
        clReleaseMemObject(cl_size);
        clReleaseMemObject(cl_res);    
        clReleaseCommandQueue(queue);
        clReleaseContext(context);
	}
    std::cout<<"Build Program is OK!"<<std::endl;

    /* load clean */
    
    cl_kernel clean = clCreateKernel(program, "clean", &err);
    if(clean == 0 && err != 0){
        std::cerr << "can't load kernel, err is "<< err << std::endl;
        clReleaseMemObject(cl_img);
        clReleaseMemObject(cl_size);
        clReleaseMemObject(cl_res);    
        clReleaseCommandQueue(queue);
        clReleaseContext(context);
        return 0;
    }
    std::cout<<"Create Kernel is OK!"<<std::endl;


    /* run the kernel */
    err = clSetKernelArg(clean, 0, sizeof(cl_mem), &cl_img);
    if( err != 0){
        std::cerr << "can't set kernel, err is "<< err << std::endl;
        clReleaseMemObject(cl_img);
        clReleaseMemObject(cl_size);
        clReleaseMemObject(cl_res);    
        clReleaseCommandQueue(queue);
        clReleaseContext(context);
        return 0;
    }
    std::cout<<"Set Kernel is OK!"<<std::endl;

    err = clSetKernelArg(clean, 1, sizeof(cl_mem), &cl_res);
    if( err != 0){
        std::cerr << "can't set kernel, err is "<< err << std::endl;
        clReleaseMemObject(cl_img);
        clReleaseMemObject(cl_size);
        clReleaseMemObject(cl_res);    
        clReleaseCommandQueue(queue);
        clReleaseContext(context);
        return 0;
    }
    std::cout<<"Set Kernel is OK!"<<std::endl;

    err = clSetKernelArg(clean, 2, sizeof(cl_mem), &cl_size);
    if( err != 0){
        std::cerr << "can't set kernel, err is "<< err << std::endl;
        clReleaseMemObject(cl_img);
        clReleaseMemObject(cl_size);
        clReleaseMemObject(cl_res);    
        clReleaseCommandQueue(queue);
        clReleaseContext(context);
        return 0;
    }
    std::cout<<"Set Kernel is OK!"<<std::endl;

    size_t work_size = 1;
    err = clEnqueueNDRangeKernel(queue, clean, 1, 0, &work_size, 0, 0, 0, 0);
    err = clEnqueueReadBuffer(queue, cl_res, CL_TRUE, 0, 768 * sizeof(unsigned int), result, 0, 0, 0);
    if(err != CL_SUCCESS){
        printf("enqueue fail");
        return 0;
    }
    clFlush(queue);
    clFinish(queue);
    clReleaseProgram(program);
    clReleaseMemObject(cl_img);
    clReleaseMemObject(cl_size);
    clReleaseMemObject(cl_res);    
    clReleaseCommandQueue(queue);
    clReleaseContext(context);

    
	//histogram_results = histogram(image, input_size);
	for(unsigned int i = 0; i < 256 * 3; ++i) {
		if (i % 256 == 0 && i != 0)
			outFile<< std::endl;
		outFile << result[i]<< ' ';
	}


	inFile.close();
	outFile.close();

	return 0;
}
