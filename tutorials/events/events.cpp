#include <functional>
#include <opc/Server.h>
#include <opc/events/BaseEventType.h>
#include <opc/nodes/MethodNode.h>
#include <opc/nodes/ObjectNode.h>
#include <iostream>

using opc::ObjectNode;
using namespace std::string_literals;

struct Callable
{
    void setObjectNode(std::shared_ptr<ObjectNode> objNode) { obj = objNode; }

    void switchToAutomatic()
    {
        std::cout << "automatic requested" << std::endl;
        if (!state)
        {
            //setup Event
            opc::TransitionEventType tEvent2;
            tEvent2.setMessage(opc::LocalizedText("de"s, "transitionEvent"));
            tEvent2.setTransition(
                opc::LocalizedText("de"s, "FromManualToAutomatic"),
                opc::NodeId(1, 5015));
            //trigger it through the object
            obj->setEvent(tEvent2);
            state = 1;
            return;
        }
        std::cout << "already in automatic" << std::endl;
    }

    void switchToManual()
    {
        std::cout << "manual requested" << std::endl;
        if (state)
        {
            opc::TransitionEventType tEvent2;
            tEvent2.setMessage(opc::LocalizedText("de"s, "transitionEvent"));
            tEvent2.setTransition(
                opc::LocalizedText("de"s, "FromAutomaticToManual"),
                opc::NodeId(1, 5019));
            obj->setEvent(tEvent2);
            state = 0;
            return;
        }
        std::cout << "already in manual" << std::endl;
    }

  private:
    std::shared_ptr<ObjectNode> obj{};
    int state{0};
};

int main()
{
    opc::Server s{4844};
    Callable c1;
    // adding of an object and 2 member functions
    auto obj = s.getObjectsFolder()->addObject(opc::NodeId(1, "IMM.OpMode"),
                                               opc::QualifiedName(1, "OpMode"),
                                               &c1, opc::NodeId(0, 0));
    obj->addMethod(opc::NodeId(1, "IMM.OpMode.SwitchToAutomatic"),
                   opc::QualifiedName(1, "RequestAutomatic"),
                   &Callable::switchToAutomatic);

    obj->addMethod(opc::NodeId(1, "IMM.OpMode.SwitchToManual"),
                   opc::QualifiedName(1, "requestManual"),
                   &Callable::switchToManual);
    c1.setObjectNode(obj);
    s.run();
}