#include <cstddef>
#include <modernOpc/Server.h>
#include <modernOpc/events/BaseEventType.h>
#include <modernOpc/methods/Method.h>
#include <modernOpc/nodes/ObjectNode.h>
#include <modernOpc/nodes/MethodNode.h>
#include <open62541/plugin/log_stdout.h>
#include <optional>

namespace opc
{
std::shared_ptr<ObjectNode>
ObjectNode::addObject(const NodeId &objId, const QualifiedName &browseName,
                      void *context, const NodeId &typeId)
{
    return server->createObject(id, objId, typeId, browseName, context);
}

UA_StatusCode ObjectNode::setUpEvent(UA_NodeId *outId, const BaseEventType &event)
{
    UA_StatusCode retval =
        UA_Server_createEvent(server->getUAServer(), fromNodeId(event.getEventType()), outId);
    if (retval != UA_STATUSCODE_GOOD)
    {
        UA_LOG_WARNING(UA_Log_Stdout, UA_LOGCATEGORY_SERVER,
                       "createEvent failed. StatusCode %s",
                       UA_StatusCode_name(retval));
        return retval;
    }

    /* Set the Event Attributes */
    /* Setting the Time is required or else the event will not show up in
     * UAExpert! */
    UA_DateTime eventTime = UA_DateTime_now();
    UA_Server_writeObjectProperty_scalar(
        server->getUAServer(), *outId, UA_QUALIFIEDNAME(0, (char *)"Time"), &eventTime,
        &UA_TYPES[UA_TYPES_DATETIME]);

    UA_UInt16 eventSeverity = 100;
    UA_Server_writeObjectProperty_scalar(
        server->getUAServer(), *outId, UA_QUALIFIEDNAME(0, (char *)"Severity"),
        &eventSeverity, &UA_TYPES[UA_TYPES_UINT16]);

    UA_LocalizedText eventMessage =
        UA_LOCALIZEDTEXT(nullptr, (char *)"An event has been generated.");
    UA_Server_writeObjectProperty_scalar(server->getUAServer(), *outId,
                                         UA_QUALIFIEDNAME(0, (char *)"Message"),
                                         &eventMessage,
                                         &UA_TYPES[UA_TYPES_LOCALIZEDTEXT]);

    UA_String eventSourceName = UA_STRING((char *)"Server");
    UA_Server_writeObjectProperty_scalar(
        server->getUAServer(), *outId,
        UA_QUALIFIEDNAME(0, (char *)"SourceName"), &eventSourceName,
        &UA_TYPES[UA_TYPES_STRING]);

    for (const auto &field : event.getEventFields())
    {
        NodeId pathId;
        auto status = server->translatePathToNodeId(fromUaNodeId(*outId), field.first, pathId);
        if (status != UA_STATUSCODE_GOOD)
        {
            UA_LOG_WARNING(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND,
                           "Cannot resolve browsepath. StatusCode %s",
                           UA_StatusCode_name(retval));
            continue;
        }

        status =
            UA_Server_writeValue(server->getUAServer(), fromNodeId(pathId), *field.second.getUAVariant());
        if (status != UA_STATUSCODE_GOOD)
        {
            UA_LOG_WARNING(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND,
                           "Writing Event property failed. StatusCode %s",
                           UA_StatusCode_name(retval));
        }
    }

    return UA_STATUSCODE_GOOD;
}

void ObjectNode::setEvent(BaseEventType &event)
{
    event.setSourceNode(id);

    UA_NodeId eventNodeId;
    UA_StatusCode retval = setUpEvent(&eventNodeId, event);
    if (retval != UA_STATUSCODE_GOOD)
    {
        UA_LOG_WARNING(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND,
                       "Creating event failed. StatusCode %s",
                       UA_StatusCode_name(retval));
    }

    retval = UA_Server_triggerEvent(server->getUAServer(), eventNodeId, fromNodeId(id),
                                    nullptr, UA_TRUE);
    if (retval != UA_STATUSCODE_GOOD)
    {
        UA_LOG_WARNING(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND,
                       "Trigger event failed. StatusCode %s",
                       UA_StatusCode_name(retval));
    }
}

UA_StatusCode ObjectNode::eventNotifier(UA_Byte& value)
{
    return UA_Server_readEventNotifier(server->getUAServer(), fromNodeId(id), &value);
}

} // namespace opc