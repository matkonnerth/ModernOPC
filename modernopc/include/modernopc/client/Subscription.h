#pragma once
#include <modernopc/types/NodeId.h>
#include <open62541/client_subscriptions.h>
#include <open62541/plugin/log_stdout.h>
#include <vector>



namespace modernopc
{

class MonitoredItem
{
public:
    MonitoredItem(const NodeId& id, UA_UInt32 monitoredItemId):m_nodeId{id}, m_id{monitoredItemId}
    {

    }

    const NodeId& Node_Id() const
    {
        return m_nodeId;
    }

    UA_UInt32 Id() const
    {
        return m_id;
    }

private:
    NodeId m_nodeId;
    UA_UInt32 m_id;
};


class Subscription
{
public:
    void create(UA_Client* client)
    {
        UA_CreateSubscriptionRequest request =
            UA_CreateSubscriptionRequest_default();
        request.requestedPublishingInterval = 50.0;
        UA_CreateSubscriptionResponse response = UA_Client_Subscriptions_create(
            client, request, NULL, NULL, deleteSubscriptionCallback);
        if (response.responseHeader.serviceResult == UA_STATUSCODE_GOOD)
        {
            m_subscriptionId = response.subscriptionId;
            UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND,
                        "Create subscription succeeded, id %u",
                        response.subscriptionId);
        }
        else
        {
            UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND,
                        "Create subscription failed");
        }
    }

    void createMonitoredItem(UA_Client* client, const NodeId& id)
    {
        UA_MonitoredItemCreateRequest monRequest =
            UA_MonitoredItemCreateRequest_default(fromNodeId(id));

        UA_MonitoredItemCreateResult monResponse =
            UA_Client_MonitoredItems_createDataChange(
                client, m_subscriptionId, UA_TIMESTAMPSTORETURN_BOTH,
                monRequest, NULL, valueChangedCallback, NULL);
        if (monResponse.statusCode == UA_STATUSCODE_GOOD)
        {
            UA_LOG_INFO(
                UA_Log_Stdout, UA_LOGCATEGORY_USERLAND,
                "Monitoring, id %u",
                monResponse.monitoredItemId);

            m_monitoredItems.push_back(MonitoredItem(id, monResponse.monitoredItemId));
        }
    }

    

    void clearAllMonitoredItems(UA_Client* client)
    {
        for(const auto& m : m_monitoredItems)
        {
            deleteMonitoredItem(client, m.Id());
        }
        //TODO: check statuscode
        m_monitoredItems.clear();
    }

private:
    //lets remove one after the other, each one has its own request, not the smartest way
    UA_StatusCode deleteMonitoredItem(UA_Client* client, UA_UInt32 monitoredItemId)
    {
        UA_DeleteMonitoredItemsRequest req;
        UA_DeleteMonitoredItemsRequest_init(&req);

        req.subscriptionId = m_subscriptionId;
        req.monitoredItemIds = &monitoredItemId;
        req.monitoredItemIdsSize = 1;

        UA_DeleteMonitoredItemsResponse deleteResponse =
        UA_Client_MonitoredItems_delete(client, req);

        if(deleteResponse.responseHeader.serviceResult!=UA_STATUSCODE_GOOD)
        {
            UA_LOG_WARNING(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND,
                        "removing monitoredItem failed");
        }
        return deleteResponse.responseHeader.serviceResult;
    }

    static void deleteSubscriptionCallback(UA_Client *client,
                                            UA_UInt32 subscriptionId,
                                            void *subscriptionContext)
    {
        UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND,
                    "Subscription Id %u was deleted", subscriptionId);
    }

    static void valueChangedCallback(UA_Client *client, UA_UInt32 subId,
                                           void *subContext, UA_UInt32 monId,
                                           void *monContext,
                                           UA_DataValue *value)
    {
        
    }
    UA_UInt32 m_subscriptionId{0u};

    std::vector<MonitoredItem> m_monitoredItems{};
};
}