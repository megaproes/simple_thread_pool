# SimpleThreadPool

SimpleThreadPool is a C++ header-only simple library that provides a basic implementation of a thread pool. It allows you to easily distribute tasks across multiple threads for concurrent execution.

## Usage

To use SimpleThreadPool in your project, follow these steps:

1. Copy the `SimpleThreadPool.h` file into your project directory.

2. Include the `SimpleThreadPool.h` header in your source file where you want to use the thread pool:

   ```cpp
   #include "SimpleThreadPool.h"
   ```

3. Create an instance of the `SimpleThreadPool` class by specifying the desired number of threads in the thread pool:

   ```cpp
   SimpleThreadPool threadPool(threadCount);
   ```

4. Define a task that you want to execute concurrently. The task can be any callable object (e.g., function, lambda, functor). Make sure the task returns a value.

5. Submit the task to the thread pool using the `Post` member function:

   ```cpp
   auto future = threadPool.Post(task);
   ```

   The `Post` function returns a `std::future` object that can be used to retrieve the result of the task.

6. Wait for the tasks to complete and retrieve the results using the `get` function of the `std::future` objects.

7. After using the thread pool, call the `Destroy` member function to stop all threads and wait for them to finish their current tasks:

   ```cpp
   threadPool.Destroy();
   ```
