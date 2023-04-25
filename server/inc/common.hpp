#pragma once
#ifndef COMMON
#define COMMON

#include <thread>
#include <stack>
#include <iostream>

namespace server::common
{

    using ThreadPtr = std::unique_ptr<std::thread, void (*)(std::thread *t)>;
	using ThreadPool = std::stack<ThreadPtr>;

};


#endif /* COMMON */
