
struct UA_Server;   
namespace opc
{
    class Server
    {
        public:
            Server();
            ~Server();
            void run();
        private:
            UA_Server *server;
            bool isRunning;
    };
} // namespace opc