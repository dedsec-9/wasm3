#include <cstdio>
#include <fstream>
#include "wasm3_cpp.h"
#include "wasm/test_prog.wasm.h"

int sum(int a, int b)
{
    return a + b;
}

void * ext_memcpy (void* dst, const void* arg, int32_t size)
{
    return memcpy(dst, arg, (size_t) size);
}

int main(void)
{
    std::cout << "Loading WebAssembly..." << std::endl;

    /* Wasm module can be loaded from a file */
    try {
        wasm3::environment env;
        wasm3::runtime runtime = env.new_runtime(1024);
        const char* file_name = "wasm/test_prog.wasm";
        std::ifstream wasm_file(file_name, std::ios::binary | std::ios::in);
        if (!wasm_file.is_open()) {
            throw std::runtime_error("Failed to open wasm file");
        }
        wasm3::module mod = env.parse_module(wasm_file);
        runtime.load(mod);
    }
    catch(std::runtime_error &e) {
        std::cerr << "WASM3 error: " << e.what() << std::endl;
        return 1;
    }

    /* Wasm module can also be loaded from an array */
    try {
        wasm3::environment env;
        wasm3::runtime runtime = env.new_runtime(1024);
        wasm3::module mod = env.parse_module(test_prog_wasm, test_prog_wasm_len);
        runtime.load(mod);

        mod.link("*", "sum", sum);
        mod.link("*", "ext_memcpy", ext_memcpy);

        {
            wasm3::function test_fn = runtime.find_function("test");
            auto res = test_fn.call<int>(20, 10);
            std::cout << "result: " << res << std::endl;
        }
        {
            wasm3::function memcpy_test_fn = runtime.find_function("test_memcpy");
            auto res = memcpy_test_fn.call<int64_t>();
            std::cout << "result: 0x" << std::hex << res << std::dec << std::endl;
        }
    }
    catch(wasm3::error &e) {
        std::cerr << "WASM3 error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
