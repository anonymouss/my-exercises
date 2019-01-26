`defer` like golang

[blog](https://jjcong.com/2019/01/24/C-%E5%AE%9E%E7%8E%B0%E7%AE%80%E5%8D%95%E7%9A%84-golang-defer/)

usage:

```cpp
#include "Defer.h"

int main() {
    defer(
        // expr 1
        // expr 2
        // ...
    ); // execute after main exit

    printf("Exit %s.", __func__);
}
```

run:

- `cmake .`
- `make`
- `./mydefer.out`

output:

```
bye...
entering function
exiting function
deferred something 2 in function
deferred something 1 in function
deferred something... 2
deferred something... 1
```