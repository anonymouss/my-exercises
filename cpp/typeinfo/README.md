output exact type name like `boost::typeindex::typeid_with_cvr`

reference: https://www.cnblogs.com/zfyouxi/p/5060288.html

```cpp
#include "type_info.h"

#include <iostream>

class Foo {};

int main() {
    int value = 0;
    int *ptr = &value;
    const int &ref = value;

    std::cout << my_type_info<decltype(value)>() << std::endl;
    std::cout << my_type_info<decltype(ptr)>() << std::endl;
    std::cout << my_type_info<decltype(ref)>() << std::endl;

    std::cout << my_type_info<int(*)[]>() << std::endl;
    std::cout << my_type_info<const volatile void * const*&>() << std::endl;
    std::cout << my_type_info<const volatile void *(&)[10]>() << std::endl;
    std::cout << my_type_info<int [1][2][3]>() << std::endl;
    std::cout << my_type_info<char(* (* const)(const int(&)[10]) )[10]>() << std::endl;
    std::cout << my_type_info<int (Foo::* const)(int, Foo&&, int) volatile>() << std::endl;
}
```

output results
```
int
int *
int const &
int (*) []
void const volatile * const * &
void const volatile * (&) [10]
int [1] [2] [3]
char (* (* const) (int const (&) [10])) [10]
int (Foo:: const) (int, Foo &&, int) volatile
```