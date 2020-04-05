#pragma once
#include <NodeId.h>
#include <Types.h>
#include <Variant.h>
#include <utility>
#include <vector>

namespace opc
{

using opc::types::LocalizedText;
using opc::types::QualifiedName;
class BaseEventType
{
  public:
    BaseEventType(const NodeId &eventTypeId = NodeId(0, 2041)) : eventType{eventTypeId} {}
    using EventFieldsType = std::vector<std::pair<QualifiedName, Variant>>;
    void setMessage(const LocalizedText &m)
    {
        Variant var;
        var(m);
        eventFields.emplace_back(
            std::make_pair(QualifiedName(0, "Message"), std::move(var)));
    }

    const NodeId &getEventType() const { return eventType; }

    virtual const EventFieldsType &getEventFields() const { return eventFields; }

  private:
    const NodeId eventType{0, 0};
  protected:
    EventFieldsType eventFields{};
};

class TransitionEventType : public BaseEventType
{
  public:
    TransitionEventType() : BaseEventType(NodeId(0, 2311)) {}

    void setTransition(const LocalizedText& transition)
    {
      Variant var;
      var(transition);
      eventFields.emplace_back(std::make_pair(QualifiedName(0, "Transition"), std::move(var)));
    }
};

} // namespace opc