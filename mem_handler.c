#include "mem_handler.h"
#include "stdio.h"
#include "stdlib.h"

//Error registry if something fails while making a reg.
#define ERROR_REG (memReg) {-1,  NULL}

//An empty image.
#define EMPTY_IMAGE (memImage) {-1, -1, NULL}

//#define NULL ((void *) 0)

//Make a new memory registry.
//It stores a memory image.
memReg new_reg()
{
    //Malloc memory for a memory image.
    memImage *ptr = malloc (sizeof(memImage));

    //Error handling 
    if (ptr == NULL) 
    {
        printf("Error allocating memory for memory registry!");
        return ERROR_REG;
    }

    //Making the starting image empty
    ptr[0] = EMPTY_IMAGE;

    //Assemble the new registry & return
    return (memReg) {0, ptr};
}

//Make a new memory registry.
//It stores n memory images.
memReg new_reg_n(long long n)
{
    //Malloc memory for n memory images.
    memImage *ptr = malloc (sizeof(memImage) * n);

    //Error handling 
    if (ptr == NULL) 
    {
        printf("Error allocating memory for memory registry!");
        return ERROR_REG;
    }

    //Initializing the images empty.
    for (int i = 0; i < n; ++i) 
    {
        ptr[i] = EMPTY_IMAGE;
    }

    //Assemble the new registry & return
    return (memReg) {0, ptr};
}

//Internal
//Relabel the images in case of overflow
//Deletes empty images.
int reg_free_garbage(memReg * reg)
{
    //Find empty elements & move them to the back.
    //Realloc and relabel all elements.
    //For every image ->
    for (int i = 0; i < reg->image_count; ++i)
    {
        //If image is empty / invalid
        if (reg->memImages[i].id == -1)
        {
            //Move value.
            long long m = 0;
            
            //If invalid block is last, then no need to check more.
            //Works
            if (i == reg->image_count) {break;};

            //Do while not finding a valid block. If no valid block then image stays in place.
            //In progress
            
            while ((i+m+1) < reg->image_count && reg->memImages[i+m+1].id != -1){m++;};
            
            //If there are only invalid blocks then break early.
            if (i+m+1 == reg->image_count-1) {printf("\nBroke early");break;};
            
            //Replace the valid with the invalid one.
            printf("\n Swapping %d with %d", i, i+m);
            reg->memImages[i] = reg->memImages[i+m];
                
            //Make the moved element invalid.
            reg->memImages[i+m].id = -1;
            reg->image_count--;
        }
    }
    //Refresh register and remove empty values.
    /*reg->image_count -= d;
    memImage * new_ = realloc(reg->memImages, sizeof(memImage) * reg->image_count);
    
    //Error.
    if (new_ == NULL) {fprintf(stderr, "Error while reallocating register!"); return -1;}
    reg->memImages = new_;*/
    return 0;
}

//Reallocate a registry in case of new images.
//Takes an array of n images.
int reg_add_img(memReg * reg, memImage * img, long long n)
{
    //Last ID
    long long l_id = reg->memImages[reg->image_count-n-1].id;
     
    //In case of label overflow
    if (l_id+n > 0xFFFFFFFFFFFFFFF) reg_relabel(reg);
    l_id = reg->memImages[reg->image_count-n-1].id;
   
    //Register too big. Manually make another one.
    if (l_id+n > 0xFFFFFFFFFFFFFFF) return -2;
    
    //Resize the object
    reg->image_count += n;
    
    //Attempt to realloc the image array
    memImage *ptr = realloc(reg->memImages, reg->image_count *  sizeof(memImage));

    //Error handling
    if (ptr == NULL) 
    {
        fprintf(stderr, "Error reallocating memory register!");
        return -1;
    }

    //Placing in the images
    for (int i = reg->image_count - n; i<reg->image_count; ++i) 
    {
        ptr[i] = img[i - ((reg->image_count - n))];
        ptr[i].id = i;
    }

    //Success
    return 0;
}

//Find index of ID.
long long reg_id_index(memReg * reg, memImage img)
{
    for (int i = 0; i < reg->image_count; ++i) 
    {
        if (reg->memImages[i].id == img.id) return i;
    }

    return -1;
}

//Remove n images from a given list.
//If image not found, then no deletion.
void reg_remove_img(memReg * reg, memImage * imagesToRemove, int n)
{
    for (int i = 0; i < n ; ++i) 
    {
        long long j = reg_id_index(reg, imagesToRemove[i]);
        if (j == -1) continue;
        reg->memImages[i].id = -1;
    }
    reg_relabel(reg);
}

/*//Make a memory chunk.
memImage new_mem_image(long long desired_size, memReg reg)
{
    //Allocating memory.
    uintptr_t *ptr = malloc(desired_size);
    //Error.
    if (ptr == NULL) return (memImage) {-1, -1,(uintptr_t *)NULL};

    //Making the memory chunk.
    mem.mem_size = desired_size;
    mem.mem_adress = ptr;
    mem.mem_ptr = mem_image_reg_p;

    //Storing it (Ease of freeing).
    mem_image_registry[mem_image_reg_p++] = mem;
    return mem;
}

//Free all the memory chunks

void free_mem_image(memImage *  mem_image) 
{
    //Error
    if (mem_image->mem_size == -1 || mem_image->mem_ptr == -1)
    { 
        printf("ALREADY FREE\n");
        return;
    }

    //Free mem image
    free(mem_image->mem_adress); mem_image->mem_adress = NULL;
    mem_image->mem_size = -1;
    mem_image->mem_ptr = -1;
    printf("OK\n");
}

//Free all memImages
void free_mem_image_reg()
{
    for (int i = 0; i < mem_image_reg_p; ++i) 
    {
        //Free all the memImages
        printf("Freeing memory : Memory size : %lld, Memory adress : %lld, ptr : %lld", mem_image_registry[i].mem_size, mem_image_registry[i].mem_adress, mem_image_registry[i].mem_ptr);
        free_mem_image(&mem_image_registry[i]);
    }

    //Reset the pointer
    mem_image_reg_p = 0;
}

//Test
int main() 
{
    train_reg();

    memImage images[10];
    for (int i = 0; i<10; ++i) 
    {
        images[i] = make_mem_image(100000);     //100 000 bytes
        printf("\nMemory size : %lld, Memory adress : %lld, ptr : %lld", images[i].mem_size, images[i].mem_adress, images[i].mem_ptr);
    } 

    free_mem_image(&images[5]);
    free_mem_image(&images[7]);
    free_mem_image(&images[3]);

    free_mem_image_reg();
    return 0;
}*/

int main() 
{
    memReg reg;
    reg.image_count = 10;
    
    memImage images[10] = {{0, 10, malloc(10)}, {1, 10, malloc(10)}, {-1, -1, NULL}, {3, 10, malloc(10)}, {3, 10, malloc(10)}, {3, 10, malloc(10)}, {3, 10, malloc(10)}, {-1, -1, NULL}, {-1, -1, NULL},{-1, -1, NULL}};
    reg.memImages = images;

    printf("\n----Before----");
    for (int i = 0; i < 10; ++i) 
    {
        printf("\nReg %d:", i);
        printf("\nID: %lld", reg.memImages[i].id);
        printf("\nSize: %lld", reg.memImages[i].size);
        printf("\nPointer: 0x%p\n", reg.memImages[i].mem_adress);
    }
    
    reg_free_garbage(&reg);
    printf("\n----After----");
    for (int i = 0; i < 10; ++i) 
    {
        printf("\nReg %d:", i);
        printf("\nID: %lld", reg.memImages[i].id);
        printf("\nSize: %lld", reg.memImages[i].size);
        printf("\nPointer: 0x%p\n", reg.memImages[i].mem_adress);

        if (images[i].mem_adress != NULL) free(images[i].mem_adress);
    }

    return 0;
    /*memImage images[2];
    images[0] = (memImage) {2, 10, NULL};
    images[1] = (memImage) {10, 20, NULL};
    reg_add_img(&reg, images, 2);
    reg_remove_img(&reg, &EMPTY_IMAGE, 10);*/
}