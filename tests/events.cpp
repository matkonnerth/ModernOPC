#include <opc/Server.h>
#include <gtest/gtest.h>
#include <opc/events/BaseEventType.h>
#include <opc/types/NodeId.h>
#include <opc/types/Types.h>

using opc::Variant;
using namespace std::string_literals;
TEST(Events, BaseEventType)
{
    opc::BaseEventType event;
    event.setMessage(opc::LocalizedText{"de", "Nachricht"});

    opc::Server s { 4844 };
    s.setEvent(event, opc::NodeId(0, 0));
}
