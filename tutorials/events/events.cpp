#include <opc/Server.h>
#include <functional>
#include <opc/events/BaseEventType.h>

using namespace std::string_literals;

struct Callable
{
    Callable(const std::string s, opc::Server& server) : state{s}, server{server} {}
    int run(int test)
    {
        std::cout << state << std::endl;

        opc::BaseEventType event1;
        event1.setMessage(opc::LocalizedText("de"s, "testMessage"s));

        opc::TransitionEventType tEvent2;
        tEvent2.setMessage(opc::LocalizedText("de"s, "transitionEvent"));
        tEvent2.setTransition(opc::LocalizedText("de"s, "FromManualToAutomatic"));

        server.setEvent(event1);
        server.setEvent(tEvent2);
        server.setEvent(tEvent2);
        return 12;
    }

  private:
    std::string state;
    opc::Server& server;
};

int main()
{
    opc::Server s;

    Callable c1{"Hello", s};
    Callable c2{"world", s};

    s.addObject(opc::NodeId(0,85), opc::NodeId(1,"hello"), opc::NodeId(0,0), "Hello", &c1);
    s.addObject(opc::NodeId(0, 85), opc::NodeId(1, "world"), opc::NodeId(0, 0),
                "World", &c2);

    s.addMethod(opc::NodeId(1, "hello"), "run", &Callable::run);
    s.addMethod(opc::NodeId(1, "world"), "run", &Callable::run);

    std::function f = [](int a, int b) { return std::vector<int>{a, b}; };
    s.addMethod(opc::NodeId(0, 85), "addMethod2", f);

    s.run();
}