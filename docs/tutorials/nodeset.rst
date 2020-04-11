Importing a nodeset.xml
-----------------------
By providing a path a valid nodeset xml can be loaded into the addresspace of the server.

.. code-block:: c

   
   #include <opc/Server.h>
   #include <functional>
   #include <opc/nodes/MethodNode.h>
   
   int main()
   {
       opc::Server s;
   
   
       if(!s.loadNodeset("../models/serviceobject.xml"))
       {
           return -1;
       }
       std::function load = [&](std::string path) { s.loadNodeset(path); };
   
       s.getMethod(opc::NodeId(2, 7003))->bindCallable(load);
   
       s.run();
   }
   
Used methods

.. doxygenfunction:: opc::Server::loadNodeset
.. doxygenfunction:: opc::Server::getMethod
