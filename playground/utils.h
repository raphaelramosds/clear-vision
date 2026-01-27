#include <iostream>

template <typename Func, typename... Args>
static void TIMEIT(std::string label, Func func, Args &&...args)
{
    auto start = std::chrono::high_resolution_clock::now();

    // Executa a função passando os argumentos
    std::invoke(func, std::forward<Args>(args)...);

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;

    std::cout << label << elapsed.count() << "s" << std::endl;
}
