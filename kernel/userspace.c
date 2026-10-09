#include "kernel/userspace.h"

void user_code()
{
    char *mesg = "Hello, This is from userspace!\n";
    asm volatile("int 0x80"
                 :
                 : "a"(1), "b"(mesg) : "memory");
    mesg = "Hello, This is from userspace again!\n";
    asm volatile("int 0x80"
                 :
                 : "a"(1), "b"(mesg) : "memory");
    for (;;)
    {
    }
}