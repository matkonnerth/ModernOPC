#include <gtest/gtest.h>
#include <modernOpc/Server.h>
#include <modernOpc/events/BaseEventType.h>
#include <modernOpc/nodes/ObjectNode.h>
#include <modernOpc/types/NodeId.h>
#include <modernOpc/types/QualifiedName.h>

using opc::Variant;
using opc::ObjectNode;
using namespace std::string_literals;
TEST(Events, BaseEventType)
{
    opc::BaseEventType event;
    event.setMessage(opc::LocalizedText{"de", "Nachricht"});

    opc::Server s { 4844 };
    s.getObjectsFolder()->setEvent(event);
}

TEST(Events, wrong_eventField)
{
    opc::BaseEventType event;
    event.setMessage(opc::LocalizedText{"de", "Nachricht"});

    event.setEventField(
        std::vector<opc::QualifiedName>{opc::QualifiedName(34, "dasdf"s)},
        opc::Variant());

    opc::Server s{4844};
    s.getObjectsFolder()->setEvent(event);
}

TEST(Events, BaseEventType2)
{
    opc::BaseEventType event;
    event.setMessage(opc::LocalizedText{"de", "Nachricht"});

    opc::Server s{4844};
    s.getObjectsFolder()->setEvent(event);
}
