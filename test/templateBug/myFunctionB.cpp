#include "MyTemplate.h"

typedef struct TestStruct {
        int c, d, e, f;
    } TestStruct;


void myFunctionB() {
    printf( "myFunctionB called, internal TestStruct size = %d\n",
            (int)sizeof( TestStruct ) );
    
    MyTemplate<TestStruct> t;
    
    t.myFunction();
    }

