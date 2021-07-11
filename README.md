[![Documentation Status](https://readthedocs.org/projects/asyncsmp/badge/?version=latest)](https://asyncsmp.readthedocs.io/en/latest/?badge=latest)
[![Gitter](https://badges.gitter.im/asyncsmp/community.svg)](https://gitter.im/asyncsmp/community?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge)
# Welcome to AsyncSMP!

[![Join the chat at https://gitter.im/asyncsmp/community](https://badges.gitter.im/asyncsmp/community.svg)](https://gitter.im/asyncsmp/community?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)

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

Currently asyncsmp supports ESP-IDF, but should be drop-in compatible with other FreeRTOS SMP implementations.

## How do I get started?

Read the documentation on [Read the Docs](https://asyncsmp.readthedocs.io/en/latest/).

## How do I contribute?

Feel free to open an issue on Github or get in touch on [Gitter](https://gitter.im/asyncsmp/community).
