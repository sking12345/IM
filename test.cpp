
#include <iostream>
using namespace std;


//懒汉式
class singleton
{
public:
    static pthread_mutex_t mutex;
    static singleton * get_instance();

private:
    singleton();
    static singleton * ptr;

};
singleton *singleton::ptr = NULL;
pthread_mutex_t singleton::mutex;

singleton::singleton()
{
    pthread_mutex_init(&mutex, NULL);


}
singleton *singleton::get_instance()
{
    if (ptr == NULL)
    {
        pthread_mutex_lock(&mutex);
        if (ptr == NULL)
        {
            ptr = new singleton();
            pthread_mutex_unlock(&mutex);
        }

    }
    return ptr;
}


int main()
{
    singleton *s1 = singleton::get_instance();
    singleton *s2 = singleton::get_instance();
    if (s1 == s2)
    {
        fprintf(stderr, "s1 == s2");
    }

    system("pause");
    return 0;
}
