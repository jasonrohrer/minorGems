echo "First link order (myFunctionA linked first)"

g++ -o testProg testProg.cpp myFunctionA.cpp myFunctionB.cpp

./testProg

echo
echo

echo "Second link order (myFunctionB linked first)"

g++ -o testProg testProg.cpp myFunctionB.cpp myFunctionA.cpp

./testProg