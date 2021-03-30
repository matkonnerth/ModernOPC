#pragma once
#include <opc/Variant.h>
#include <opc/types/NodeId.h>
#include <opc/types/LocalizedText.h>
#include <opc/types/QualifiedName.h>
#include <utility>
#include <vector>

namespace opc
{

class BaseEventType
{
  public:
    BaseEventType(const NodeId &eventTypeId = NodeId(0, 2041))
        : eventType{eventTypeId}
    {
    }
    virtual ~BaseEventType() = default;
    using EventFieldsType =
        std::vector<std::pair<std::vector<QualifiedName>, Variant>>;
    void setMessage(const LocalizedText &m)
    {
        Variant var{m};
        eventFields.emplace_back(std::make_pair(
            std::vector<QualifiedName>{QualifiedName(0, "Message")},
            std::move(var)));
    }

    void setSourceNode(const NodeId& id)
    {
      Variant var{id};
      eventFields.emplace_back(std::make_pair(
          std::vector<QualifiedName>{QualifiedName(0, "SourceNode")},
          std::move(var)));
    }

    const NodeId &getEventType() const { return eventType; }

    virtual const EventFieldsType &getEventFields() const
    {
        return eventFields;
    }

    void setEventField(const std::vector<QualifiedName>& path, Variant&& var)
    {
      
      eventFields.emplace_back(std::make_pair(
            path, std::move(var)));
    }

  private:
    const NodeId eventType{0, 0};

  protected:
    EventFieldsType eventFields{};
};

class TransitionEventType : public BaseEventType
{
  public:
    TransitionEventType() : BaseEventType(NodeId(0, 2311)) {}

    void setTransition(const LocalizedText &transition, const NodeId& transitionId)
    {
        Variant var{transition};
        eventFields.emplace_back(std::make_pair(
            std::vector<QualifiedName>{QualifiedName(0, "Transition")},
            std::move(var)));
          
        eventFields.emplace_back(std::make_pair(
            std::vector<QualifiedName>{QualifiedName(0, "Transition"), QualifiedName(0, "Id")},
            Variant{transitionId}));
    }
};

} // namespace opc