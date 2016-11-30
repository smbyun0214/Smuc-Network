#include <iostream>
#include <cstdlib>
#include <pthread.h>

using namespace std;

class A{
    private:
    int num;

    public:
    A(int num)
    {
        this->num = num;
    }

    void Show()
    {
        cout<<num<<endl;
        cout<<this->num<<endl;
    }
};
#define NUM_THREADS 5

int main()
{
    A a(10);
    a.Show();

    return 0;


}