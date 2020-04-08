#include <functional>
#include <opc/Server.h>
#include <opc/events/BaseEventType.h>
#include <opc/nodes/MethodNode.h>
#include <opc/nodes/ObjectNode.h>

using namespace std::string_literals;

struct Callable
{
    Callable(const std::string &s, opc::Server &server)
        : state{s}, server{server}
    {
    }
    void switchToAutomatic()
    {
        std::cout << "automatic requested" << std::endl;

        opc::TransitionEventType tEvent2;
        tEvent2.setMessage(opc::LocalizedText("de"s, "transitionEvent"));
        tEvent2.setTransition(
            opc::LocalizedText("de"s, "FromManualToAutomatic"),
            opc::NodeId(1, 5015));
        tEvent2.setSourceNode(opc::NodeId(1, "IMM.OpMode"));
        server.setEvent(tEvent2, opc::NodeId(1, "IMM.OpMode"));
    }

    void switchToManual()
    {
        std::cout << "manual requested" << std::endl;
        opc::TransitionEventType tEvent2;
        tEvent2.setMessage(opc::LocalizedText("de"s, "transitionEvent"));
        tEvent2.setTransition(
            opc::LocalizedText("de"s, "FromAutomaticToManual"),
            opc::NodeId(1, 5019));
        tEvent2.setSourceNode(opc::NodeId(1, "IMM.OpMode"));
        server.setEvent(tEvent2, opc::NodeId(1, "IMM.OpMode"));
    }

  private:
    std::string state;
    opc::Server &server;
};

int main()
{
    opc::Server s{4844};
    Callable c1{"Hello", s};
    s.getObjectsFolder()
        ->addObject(opc::NodeId(1, "IMM.OpMode"),
                    opc::QualifiedName(1, "OpMode"), &c1, opc::NodeId(0, 0))
        ->addMethod(opc::NodeId(1, "IMM.OpMode.SwitchToAutomatic"),
                    opc::QualifiedName(1, "RequestAutomatic"),
                    &Callable::switchToAutomatic);

    s.getObject(opc::NodeId(1, "IMM.OpMode"))
        ->addMethod(opc::NodeId(1, "IMM.OpMode.SwitchToManual"),
                    opc::QualifiedName(1, "requestManual"),
                    &Callable::switchToManual);
    s.run();
}