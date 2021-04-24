Asynchronous parallel execution
===============================

.. highlight:: c

Asynchronous execution in a new task
-------------------------------------

The simpliest way to execute an asynchronous operation is to spawn a new task which will execute it on the inactive cores. You can do so by following these steps:

- Create an async function with signature :code:`void fn(asyncsmp_req_t *req)`.
- In your main execution flow, create a request. Several kinds of requests are available for different uses, you can find out more :doc:`here <request_types>`.
- Spawn the asynchronous execution with :code:`asyncsmp_exec()`.
- Wait for its completion with the corresponding awaiter when you choose to do so (read more on awaiters on the :doc:`request types <request_types>` page)

.. literalinclude:: ../../examples/parallel_execution/main/main.c

Asynchronous execution in a new task (with parameters)
------------------------------------------------------

If you need to pass some parameters to the asynchronous operation, you can attach them to the request. To do so, define a parameters structure (for example :code:`do_stuff_params_t`) and pass its size as a parameter to the request allocator. You can then cast :code:`req->data` to access them.

.. literalinclude:: ../../examples/parallel_execution_withparams/main/main.c
