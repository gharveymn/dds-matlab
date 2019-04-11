#include <stdint.h>
#include <random>
#include <chrono>

class A
{
public:
	size_t _x;
	size_t _y;
	size_t _xy;
	A(size_t x, size_t y) : _x(x), _y(y), _xy(x * y) {}
};

class B
{
public:
	size_t _x;
	size_t _y;
	constexpr inline size_t _xy()
	{
		return this->_x * this->_y;
	}
	B(size_t x, size_t y) : _x(x), _y(y) {}
};

void test1()
{
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<size_t> rng;
	A testc(rng(gen), rng(gen));
	auto t1 = std::chrono::high_resolution_clock::now();
	for(size_t i = 0; i < (1 << 26); i++)
	{
		auto x = testc._xy;
	}
	auto t2 = std::chrono::high_resolution_clock::now();
	printf("A: %llu\n", std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count());
}

void test2()
{
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<size_t> rng;
	B testc(rng(gen), rng(gen));
	auto t1 = std::chrono::high_resolution_clock::now();
	for(size_t i = 0; i < (1 << 26); i++)
	{
		auto x = testc._xy();
	}
	auto t2 = std::chrono::high_resolution_clock::now();
	printf("B: %llu\n", std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count());
}

int main(int argc, char* argv[])
{
	test1();
	test2();
}

