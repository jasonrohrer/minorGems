#include "MyTemplate.h"

typedef struct TestStruct {
        int c, d;
    } TestStruct;


void myFunctionA() {
    printf( "myFunctionA called, internal TestStruct size = %d\n",
            (int)sizeof( TestStruct ) );

    MyTemplate<TestStruct> t;
    
    t.myFunction();
    }

