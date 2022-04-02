/* calledbind.cpp */
#include <boost/bind.hpp>
#include <iostream>
#include <string>

void func()
{
    std::cout << "Binding Function" << std::endl;
}

void cubevolume(float f) {
    std::cout << "Volume of the cube is " << f * f * f << std::endl;
}

void identity(std::string name, int age, float height)
{
    std::cout << "Name : " << name << std::endl;
    std::cout << "Age : " << age << " years old" << std::endl;
    std::cout << "Height : " << height << " inch" << std::endl;
}

class TheClass
{
public:
    void identity(std::string name, int age, float height)
    {
        std::cout << "Name : " << name << std::endl;
        std::cout << "Age : " << age << " years old" << std::endl;
        std::cout << "Height : " << height << " inch" << std::endl;
    }
};

int main(void)
{
    boost::bind(&func)();
    boost::bind(&cubevolume, 4.23f)();
    boost::bind(&identity, "John", 25, 68.89f)();
    TheClass cls;
    boost::bind(&TheClass::identity, &cls, "John", 25, 68.89f)();
    return 0;
}