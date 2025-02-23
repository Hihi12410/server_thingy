#include "stdint.h" 

//A memory image instance.
typedef struct mem_image
{
    //ID inside memReg.
    long long id;
    //Size, in bytes.
    long long size;
    //The actual array of bytes.
    unsigned int * mem_adress;
} 
memImage;

//Data in memory.
typedef struct mem_instance
{
    long long start_p;
    long long size;
    memImage * container;
}
memInstance;

//Memory image instance domain.
typedef struct mem_reg
{
    //Count of images in registry.
    long long image_count;
    //The images.
    memImage * memImages;
} 
memReg;
