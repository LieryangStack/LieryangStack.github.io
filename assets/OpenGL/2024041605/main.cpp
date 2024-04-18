#include <iostream>

void func (float *a) {
    if ( *a > 0) {
        std::cout << "hello" << std::endl;
    }
}

int main(int argc, char const *argv[])
{
    float a = 0;

    func (&a);
    
    return 0;
}
