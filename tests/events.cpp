#include <gtest/gtest.h>
#include <modernOpc/Server.h>
#include <modernOpc/events/BaseEventType.h>
#include <modernOpc/nodes/ObjectNode.h>
#include <modernOpc/types/NodeId.h>
#include <modernOpc/types/QualifiedName.h>

using modernopc::Variant;
using modernopc::ObjectNode;
using namespace std::string_literals;
TEST(Events, BaseEventType)
{
    modernopc::BaseEventType event;
    event.setMessage(modernopc::LocalizedText{"de", "Nachricht"});

    modernopc::Server s { 4844 };
    s.getObjectsFolder()->setEvent(event);
}

TEST(Events, wrong_eventField)
{
    modernopc::BaseEventType event;
    event.setMessage(modernopc::LocalizedText{"de", "Nachricht"});

    event.setEventField(
        std::vector<modernopc::QualifiedName>{modernopc::QualifiedName(34, "dasdf"s)},
        modernopc::Variant());

    modernopc::Server s{4844};
    s.getObjectsFolder()->setEvent(event);
}

TEST(Events, BaseEventType2)
{
    modernopc::BaseEventType event;
    event.setMessage(modernopc::LocalizedText{"de", "Nachricht"});

    modernopc::Server s{4844};
    s.getObjectsFolder()->setEvent(event);
}
