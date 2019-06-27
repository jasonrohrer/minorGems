#include <stdio.h>

template <class Type>
class MyTemplate {
    public:
        void myFunction();
    };


template <class Type>
void MyTemplate<Type>::myFunction() {
    printf( "Size of Type as seen by MyTemplate = %d\n", (int)sizeof( Type ) );
    }
