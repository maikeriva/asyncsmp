.. asyncsmp documentation master file, created by
   sphinx-quickstart on Sun Jan 24 12:36:47 2021.
   You can adapt this file completely to your liking, but it should at least
   contain the root `toctree` directive.

Welcome to AsyncSMP!
====================

AsyncSMP is a tiny framework for developing asynchronous and multithreaded
applications on top of multicore FreeRTOS SMP variants such as ESP-IDF.
When used alongside a specific set of design patterns, asyncsmp can greatly simplify the development 
of thread-safe and responsive applications while making full use of all available 
cores.

Some of the advantages provided include:

* Asynchronous programming through async/await and request/response style directives (similar to C#, Node, etc.)
* Thread safe by default (if design patterns are followed)
* Easy parallelization of processes
* Tasks are automatically run on any core available, minimizing idle time
* Light memory footprint (7 bytes of RAM average overhead per async operation)
* Tiny codebase (<1000 lines of code)
* Reduces necessity of establishing clear task priorities
* Unintrusive, can be used as necessary in specific components

.. toctree::
   :maxdepth: 1
   :caption: Contents:

   quickstart
   parallel_exec
   task_communication
   request_types
   code_reference
