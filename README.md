# W5N Rope

C++ implementation of the [Rope](https://en.wikipedia.org/wiki/Rope_\(data_structure\)) data structure with UTF-8 support.

## Example Usage

```cpp
#include <iostream>
#include <w5n/Rope.hpp>

int main()
{
    w5n::Rope rope;

    rope.append("World");
    std::cout << "Append: " << rope.toString() << std::endl; // World

    rope.prepend("Hello, ");
    std::cout << "Prepend: " << rope.toString() << std::endl; // Hello, World

    rope.erase(1, 7);
    std::cout << "Erase: " << rope.toString() << std::endl; // Horld

    rope.insert(1, "ello, W");
    std::cout << "Insert: " << rope.toString() << std::endl; // Hello, World

    auto ch = rope.at(7);
    std::cout << "At: " << ch << std::endl; // W

    rope.clear();
    std::cout << "Clear: " << rope.toString() << std::endl; // <empty string>

    rope.append("😀😁😂"); // unicode supported

    std::cout << "Emojis: " << rope.toString() << std::endl; // 😀😁😂
    std::cout << "Size in bytes: " << rope.size() << std::endl; // 12
    std::cout << "Char count: " << rope.charCount() << std::endl; // 3

    for (size_t i = 0; i < rope.charCount(); ++i) {
        std::cout << rope.at(i) << '\n';
        // Output:
        // 😀
        // 😁
        // 😂
    }
}
```

## Dependencies

- [uni-algo](https://github.com/uni-algo/uni-algo): used to count and extract graphemes on a UTF-8 `std::string`.

## Build with CMake

You can use cmake to build this library:

```bash
git clone https://github.com/waldson/w5n-rope
cd w5n-rope
mkdir build
cd build
cmake ..
cmake --build .
```

### Building and Running the Examples

Optionally, you can build and run the examples, by setting the `BUILD_EXAMPLES` configuration option while using `cmake`:

```bash
cmake -DBUILD_EXAMPLES=1 ..
cmake --build .
./examples/basic-example
./examples/emoji-example
```


### Building and Running the Tests

To build the tests set the `BUILD_TESTS` configuration option:

```bash
cmake -DBUILD_TESTS=1 ..
cmake --build .
```

This will create the test executable under the `tests` folder:

```
./tests/RopeTest
```

If you're using `make` as your build system, you can also call `make test` to run them as well.


## TODO

- [ ] Implement `indexOf`
- [ ] Make it an iterator
