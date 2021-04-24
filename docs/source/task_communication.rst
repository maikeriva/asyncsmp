.. asyncsmp documentation master file, created by
   sphinx-quickstart on Sun Jan 24 12:36:47 2021.
   You can adapt this file completely to your liking, but it should at least
   contain the root `toctree` directive.

.. highlight:: c

Asynchronous task-to-task communication
=======================================

AsyncSMP allows to easily implement asynchronous and non-blocking task-to-task communication. Tasks communicate by sending and receiving requests or messages.

Requests can carry arbitrary data between tasks the same way they carry parameters in :doc:`asynchronous execution <parallel_exec>` scenarios.

Sending/receiving async requests
--------------------------------

The simpliest form of task communication is between a main task and a receiver task accepting arbitrary requests.

Note how the receiver task is completely oblivious to the kind of incoming request: they can all be handled with a single logic.

Multiple requests can be sent sequentially and it is not necessary to await them in order.

.. literalinclude:: ../../examples/task_communication_simple/main/main.c

Sending/receiving async requests (multiple behavior)
----------------------------------------------------

Additionally, we may want a permanent task to perform different actions depending on different inputs. This can be accomplished by sending **messages** rather than requests.

Messages are a lightweight wrapper around requests which allow to specify a **message type** which the receiving task can use to determine the operation to perform.

Messages can also carry other kinds of data aside of requests, which makes them useful for other scenarios. You may want to check out the :doc:`request types <request_types>` page.

.. literalinclude:: ../../examples/task_communication_messages/main/main.c

Sending/receiving async requests (non-blocking)
-----------------------------------------------

In many scenarios (such as networking) it may be useful for a task to be responsive even when waiting for an operation to complete. The message pattern shown above can be applied to awaiting tasks too, in order to keep them responsive to new requests.

For this scenario it is particularly useful the **qmsg** request type. Read more about it in the :doc:`request types <request_types>` page.

.. literalinclude:: ../../examples/task_communication_nonblocking/main/main.c

Chaining and forwarding requests
--------------------------------

If an operation requires multiple requests to take place before being considered complete, it is useful to **chain** them. Chaining removes the necessity of tracking the requests themselves within tasks.

This is particularly useful when applications are structured as groups of tasks controlling each other hierarchically. For example, let's assume an application made of the following non-blocking tasks:

- An **input_controller** task, which handles inputs from a button and turns on an LED when the corresponding operation is complete.
- A **handler** task, which listens for requests from *input_controller* and performs an action depending on the current application state.
- An **output_controller** task, which on a request from *handler* performs an action.

In the above example, *input_controller* sends requests to *handler*, which in turn sends new requests to *output_controller*. Without chaining, *handler* would need to keep track of each request from *input controller* until a corresponding request from *output controller* is returned. Chaining requests frees *handler* from this burden, as parent requests can simply be retrieved when a child request from *output_handler* is returned.

.. literalinclude:: ../../examples/task_communication_chaining/main/main.c