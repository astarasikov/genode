/*
 * \brief  Test client for the Hello RPC interface
 * \author Björn Döbel
 * \date   2008-03-20
 */

/*
 * Copyright (C) 2008-2012 Genode Labs GmbH
 *
 * This file is part of the Genode OS framework, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

#include <base/env.h>
#include <base/printf.h>

using namespace Genode;

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
