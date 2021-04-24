.. asyncsmp documentation master file, created by
   sphinx-quickstart on Sun Jan 24 12:36:47 2021.
   You can adapt this file completely to your liking, but it should at least
   contain the root `toctree` directive.

.. highlight:: c

Request types
=============

Semaphore request
-----------------

Semaphore requests suit a wide range of scenarios. When unsure on which kind to use, start with this.

::

   /**
   * Allocate
   * - data_size: size of data to carry
   */
   asyncsmp_req_t *req = asyncsmp_req_alloc_sem(0);

   /**
   * Await
   * - req: request to await
   * - ticks: maximum tick time to await before giving up
   */
   asyncsmp_await_sem(req,portMAX_DELAY);

   /**
   * Deallocate
   * - req: request to deallocate
   */
   asyncsmp_req_free_sem(req);

Queue message request
---------------------

Queue message requests are used for task-to-task communication. Check out the :doc:`relevant page <task_communication>` for examples.

.. Queue message requests are transparently wrapped in a *message* of type *msg_type* and sent to the receiving task's queue *queue*.

If the sending task's queue is protected by a semaphore (for example, to allow safe deinitialization) you have to pass the semaphore in the allocation parameters.

::

   /**
   * Allocate
   * - queue: sending task queue handle
   * - msg_type: message type
   * - queue_guard: sending task queue guard (semaphore)
   * - data_size: size of data to carry
   */
   asyncsmp_req_t *req = asyncsmp_req_alloc_qmsg(queue, msg_type, queue_guard, data_size);

   /**
   * Await
   * - queue: awaiting task queue
   * - msg: pointer to message holder structure
   * - ticks: maximum tick time to await before giving up
   */
   xQueueReceive(queue,msg,ticks);

   /**
   * Deallocate
   * - req: request to deallocate
   */
   asyncsmp_req_free_qmsg(req);


Task notification request
-------------------------

Task notification requests are a more lightweight alternative to semaphore requests, but only one can be sent and awaited at any given time within the same task.

::

   /**
   * Allocate
   * - data_size: size of data to carry
   */
   asyncsmp_req_t *req = asyncsmp_req_alloc_tn(data_size);

   /**
   * Await
   * - ticks: maximum tick time to await before giving up
   */
   asyncsmp_await_tn(portMAX_DELAY);

   /**
   * Deallocate
   * - req: request to deallocate
   */
   asyncsmp_req_free_tn(req);


Event group request
--------------------------------

Event group requests can be used to await on multiple requests at once. This may be useful in some scenarios, even though most of the time the same result can be achieved with simple *semaphore* and *queue message* requests.

It is possible to await for either all requests to return before continuing execution or any of them by using the appropriate function.

::

   /**
   * Allocate
   * - eg: event group handle
   * - eb: event bit to be used by the request
   * - data_size: size of data to carry
   */
   asyncsmp_req_t *req = asyncsmp_req_alloc_eg(eg, eb, data_size);

   /**
   * Await ALL requests
   * - eg: event group handle
   * - eb: event bitmask of the event group requests to be awaited
   * - ticks: maximum tick time to await before giving up
   */
   asyncsmp_await_eg_all(eg, eb, ticks);

   /**
   * Await ANY request
   * - eg: event group handle
   * - eb: event bitmask of the event group requests to be awaited
   * - ticks: maximum tick time to await before giving up
   */
   asyncsmp_await_eg_any(eg, eb, ticks);

   /**
   * Deallocate
   * - req: request to deallocate
   */
   asyncsmp_req_free_eg(req);
