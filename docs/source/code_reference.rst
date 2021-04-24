.. asyncsmp documentation master file, created by
   sphinx-quickstart on Sun Jan 24 12:36:47 2021.
   You can adapt this file completely to your liking, but it should at least
   contain the root `toctree` directive.

Code reference
==============

Request
-------

.. doxygentypedef:: asyncsmp_req_t
   :outline:
.. doxygenstruct:: asyncsmp_req
   :members:
.. doxygenfunction:: asyncsmp_cb

Semaphore requests
------------------

.. doxygenfunction:: asyncsmp_req_alloc_sem
.. doxygenfunction:: asyncsmp_await_sem
.. doxygenfunction:: asyncsmp_req_free_sem

Queue message requests
----------------------

.. doxygenfunction:: asyncsmp_req_alloc_qmsg
.. doxygenfunction:: asyncsmp_req_free_qmsg

Task notification requests
--------------------------

.. doxygenfunction:: asyncsmp_req_alloc_tn
.. doxygenfunction:: asyncsmp_await_tn
.. doxygenfunction:: asyncsmp_req_free_tn

Event group requests
--------------------

.. doxygenfunction:: asyncsmp_req_alloc_eg
.. doxygenfunction:: asyncsmp_await_eg_all
.. doxygenfunction:: asyncsmp_await_eg_any
.. doxygenfunction:: asyncsmp_req_free_eg

Noawait requests
----------------

.. doxygenfunction:: asyncsmp_req_alloc_noawait

Custom requests
----------------

.. doxygenfunction:: asyncsmp_req_alloc_custom
.. doxygenfunction:: asyncsmp_req_free_custom

Task message
------------

.. doxygentypedef:: asyncsmp_msg_t
   :outline:
.. doxygenstruct:: asyncsmp_msg
   :members:

Asynchronous execution
----------------------

.. doxygentypedef:: asyncsmp_fn_t
.. doxygenfunction:: asyncsmp_exec
