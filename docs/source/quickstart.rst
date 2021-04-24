.. asyncsmp documentation master file, created by
   sphinx-quickstart on Sun Jan 24 12:36:47 2021.
   You can adapt this file completely to your liking, but it should at least
   contain the root `toctree` directive.

.. highlight:: sh

Quick start
===========

For ESP-IDF projects
--------------------

Clone asyncsmp's repository into your project's **components** directory. You can take advantage of **git submodules** to do so while keeping your repository clean. Read more about git submodules `here <https://git-scm.com/book/en/v2/Git-Tools-Submodules>`_.

::
   
   git submodule add git://github.com/maikeriva/asyncsmp

You can now include it as a dependency in your files wherever needed. Check out some of the examples below.

Examples
--------

- :doc:`parallel_exec`

- :doc:`task_communication`
