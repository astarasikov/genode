#if 1
#include <base/env.h>
#include <base/printf.h>
#include <util/string.h>
using namespace Genode;
#else
#define PDBG	printf
#include <stdio.h>
#include <string.h>
#endif

void hello_smash()
{
    PDBG("Hello from smashed stack\n");
}

class MyClass
{ 
    private:
        int Buffer[8];
	int test;
    public:
        void SetBuffer(int *String)
        {
    	    test=0xa5;
    	    int i;
//	    strncpy(Buffer,String,40);
//	    memcpy(Buffer,String,40);
	    for(i=0;i<50;i++)
		Buffer[i]=String[i];
        }
        void PrintBuffer()
        {
            PDBG("%x\t%x\n", &Buffer[0],&Buffer[7]);
            PDBG("%x\t%x\n", test,&test);
        }
};

void test_smash()
{
     MyClass Object;
     int tmp_buff[50],i;
     for(i=0;i<50;i++)
    	tmp_buff[i]=(int)&hello_smash;
     Object.SetBuffer(tmp_buff);
     Object.PrintBuffer();
}

int main()
{
    test_smash();
}