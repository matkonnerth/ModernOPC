#include <functional>
#include <iostream>
#include <modernopc/Server.h>
#include <modernopc/nodes/MethodNode.h>
#include <modernopc/nodes/ObjectNode.h>
#include <modernopc/events/BaseEventType.h>

using modernopc::ObjectNode;
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
            modernopc::TransitionEventType tEvent2;
            tEvent2.setMessage(modernopc::LocalizedText("de"s, "transitionEvent"));
            tEvent2.setTransition(
                modernopc::LocalizedText("de"s, "FromManualToAutomatic"),
                modernopc::NodeId(1, 5015));
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
            modernopc::TransitionEventType tEvent2;
            tEvent2.setMessage(modernopc::LocalizedText("de"s, "transitionEvent"));
            tEvent2.setTransition(
                modernopc::LocalizedText("de"s, "FromAutomaticToManual"),
                modernopc::NodeId(1, 5019));
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
    modernopc::Server s{4844};
    Callable c1;
    // adding of an object and 2 member functions
    auto obj = s.getObjectsFolder()->addObject(modernopc::NodeId(1, "IMM.OpMode"),
                                               modernopc::QualifiedName(1, "OpMode"),
                                               &c1, modernopc::NodeId(0, 0));
    obj->addMethod(modernopc::NodeId(1, "IMM.OpMode.SwitchToAutomatic"),
                   modernopc::QualifiedName(1, "RequestAutomatic"),
                   &Callable::switchToAutomatic);

    obj->addMethod(modernopc::NodeId(1, "IMM.OpMode.SwitchToManual"),
                   modernopc::QualifiedName(1, "requestManual"),
                   &Callable::switchToManual);
    c1.setObjectNode(obj);
    s.run();
}