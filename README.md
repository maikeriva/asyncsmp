# Welcome to AsyncSMP!

AsyncSMP is a tiny framework for developing asynchronous and multithreaded applications on top of multicore FreeRTOS SMP variants such as ESP-IDF. When used alongside a specific set of design patterns, asyncsmp can greatly simplify the development of thread-safe and responsive applications while making full use of all available cores.

Some of the advantages provided include:

- Asynchronous programming through async/await and request/response style directives (similar to C#, Node, etc.)
- Thread safe by default (if design patterns are followed)
- Easy parallelization of processes
- Tasks are automatically run on any core available, minimizing idle time
- Light memory footprint (7 bytes of RAM average overhead per async operation)
- Tiny codebase (<1000 lines of code)
- Reduces necessity of establishing clear task priorities
- Unintrusive, can be used as necessary in specific components

## Supported platforms

Currently asyncsmp supports ESP-IDF, but should be drop-in compatible with other FreeRTOS SMP implementations.

## Roadmap
- Create an intuitive task-communication API to replace the current asynchronous task boilerplate.
- Evaluate ports to other platforms (such as Zephyr)

## Contributing
Feel free to open merge requests and use the issue tracker on Github!