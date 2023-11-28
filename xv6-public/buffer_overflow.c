#include "types.h"
#include "user.h"
#include "fcntl.h"

void foo()
{
    printf(1, "SECRET_STRING");
}

void vulnerable_func(char *payload)
{
    char buffer[4];
    strcpy(buffer, payload);

    /*
    // ### DON'T delete this comment
    // Save value of esp register in ptr_to_esp and then move 9 to address pointed ato by ptr_to_esp
    uint ptr_to_esp;
    asm volatile("movl %%esp, %0;"
                    "movl $9, (%0)" : "=r" (ptr_to_esp));

    // Copy the contents AT esp to ptr
    uint val_at_esp;
    asm volatile("movl (%%esp), %0" : "=r" (val_at_esp));
    printf(1, "Value at esp: %d\n", val_at_esp);

    // Copy the address stored IN esp register to cal_of_esp
    uint val_of_esp;
    asm volatile("movl %%esp, %0" : "=r" (val_of_esp));
    printf(1, "Value OF esp register: %d\n", val_of_esp);
    */


    
    // char* a1;
    /*
    uint val_at_buffer;
    asm volatile("movl %1, %0" : "=r" (val_at_buffer):"m" (buffer));
    printf(1, "Value at derived buffer address: %d\n", val_at_buffer);
    printf(1, "Value at buffer address: %x\n", *((int*)((void*)buffer)));

    // IS SAME AS
    asm volatile("movl (%1), %0" : "=r" (val_at_buffer):"r" (buffer));
    printf(1, "Value at derived buffer address: %d\n", val_at_buffer);
    printf(1, "Value at buffer address: %d\n", *((int*)((void*)buffer)));
    */
   
    // printf(1, "buffer add = %d\n", &buffer);
    // uint val_at_buffer;
    // asm volatile("movl %1, %0;"
    //              "addl $4, %0;" : "=r" (val_at_buffer):"r" (buffer));
    // uint val_at_buffer_off4;
    // asm volatile("movl (%1), %0;" : "=r" (val_at_buffer_off4): "r"(val_at_buffer));
    
    // printf(1, "Value at buffer address: %x\n", val_at_buffer_off4);

    // uint ptr_to_ebp;
    // asm volatile("movl -0x4(%%ebp), %0;" : "=r" (ptr_to_ebp));
    // printf(1, "Value at ebp-4: %d\n", ptr_to_ebp);
    // uint val_at_ebp_4;
    // asm volatile("movl %%ebp, %0;"
    //                 "subl $3, %0;" : "=r" (ptr_to_ebp));
    // asm volatile("movl (%1), %0;" : "=r" (val_at_ebp_4) : "r" (ptr_to_ebp));
                    
    // printf(1, "Value at ebp-4 after: %x\n", val_at_ebp_4);

    
    

    
    // uint *pot;
    // // asm("movl %%ebp, %0;"
    // asm("mov %%ebp, %0" : "=r" (pot));
    // asm("addl $4, %0" : "=r" (pot));

    // printf(1, "Value at EBP+4: %d\n", *(pot));

    

    // uint* ptr_to_payload;
    // asm volatile   ("movl %%ebp, %0\n\t"
    //                 "addl $-4, %0\n\t"
    //                 "movl [%0], %0\n\t" : "=r" (ptr_to_payload));
    // printf(1, "Value at EBP+4: %d\n", *ptr_to_payload);


    // uint *pot1;
    // asm volatile("mov %%ebp, %0" : "=r" (pot1));
    // // printf(1, "Value at EBP+4: %d\n", *(pot1));
    // printf(1, "Value at EBP+4: %d\n", *(int *)(pot1 + 1));
    
    // asm volatile("movl $0x00000050, %0" : "=m" (*(int *)(pot1 + 1)));

    // asm volatile("movl $0x00000050, %0" : "=m" (*(int *)(esp + 2)));
    // printf(1, "esp val = %d\n", *esp);
    // printf(1, "esp val = %d\n", *(esp+1));
}

int main(int argc, char *argv[])
{
    int fd;

    fd = open("payload", O_RDONLY);
    char payload[100];

    read(fd, payload, 100);
    
    vulnerable_func(payload);

    close(fd);

    exit();
}