#pragma once
#include <modernopc/types/NodeId.h>
#include <open62541/client_subscriptions.h>
#include <open62541/plugin/log_stdout.h>
#include <vector>
#include <unordered_map>

namespace modernopc
{

class MonitoredItem
{
public:
    MonitoredItem(const NodeId& id, UA_UInt32 monitoredItemId):m_nodeId{id}, m_id{monitoredItemId}
    {

    }

    MonitoredItem(const NodeId&id): m_nodeId{id}, m_id{0}{}

    const NodeId& Node_Id() const
    {
        return m_nodeId;
    }

    UA_UInt32 Id() const
    {
        return m_id;
    }

    void setId(UA_UInt32 id)
    {
        m_id=id;
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

    /*
    void createMonitoredItem(UA_Client* client, const NodeId& id)
    {
        UA_MonitoredItemCreateRequest monRequest =
            UA_MonitoredItemCreateRequest_default(fromNodeId(id));
        monRequest.requestedParameters.samplingInterval = 50;

        UA_MonitoredItemCreateResult monResponse =
            UA_Client_MonitoredItems_createDataChanges(
                client, m_subscriptionId, UA_TIMESTAMPSTORETURN_BOTH,
                monRequest, NULL, valueChangedCallback, NULL);
        if (monResponse.statusCode == UA_STATUSCODE_GOOD)
        {
            UA_LOG_DEBUG(
                UA_Log_Stdout, UA_LOGCATEGORY_USERLAND,
                "Monitoring, id %u",
                monResponse.monitoredItemId);

            m_monitoredItems.push_back(MonitoredItem(id, monResponse.monitoredItemId));
        }
    }*/

    void createMonitoredItemAsyncBegin(UA_Client *client, const NodeId &id)
    {
        /* monitor the server state */
        UA_MonitoredItemCreateRequest singleMonRequest =
            UA_MonitoredItemCreateRequest_default(fromNodeId(id));
        void *contexts = NULL;
        UA_Client_DataChangeNotificationCallback notifications =
            valueChangedCallback;
        UA_Client_DeleteMonitoredItemCallback deleteCallbacks = NULL;

        UA_CreateMonitoredItemsRequest monRequest;
        UA_CreateMonitoredItemsRequest_init(&monRequest);
        monRequest.subscriptionId = m_subscriptionId;
        monRequest.itemsToCreate = &singleMonRequest;
        monRequest.itemsToCreateSize = 1;
        UA_CreateMonitoredItemsResponse monResponse;
        UA_CreateMonitoredItemsResponse_init(&monResponse);
        UA_UInt32 requestId{};
        auto retval = UA_Client_MonitoredItems_createDataChanges_async(
            client, monRequest, &contexts, &notifications, &deleteCallbacks,
            createDataChangesCallback, this, &requestId);

        m_pendingRequests.emplace(std::make_pair(requestId, MonitoredItem{id}));

        if (retval != UA_STATUSCODE_GOOD)
        {
            UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND,
                        "sending async monitoring request failed");
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

    void createMonitoredItemAsyncEnd(UA_UInt32 requestId, UA_UInt32 monitoredItemId)
    {
        auto entry = m_pendingRequests.find(requestId);
        if(entry == m_pendingRequests.end())
        {
            return;
        }
        entry->second.setId(monitoredItemId);
        m_monitoredItems.push_back(entry->second);
        m_pendingRequests.erase(entry);
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

        return UA_Client_MonitoredItems_delete_async(client, req, nullptr, nullptr, nullptr);
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

    static void createDataChangesCallback(UA_Client *client, void *userdata,
                                          UA_UInt32 requestId, void *r){

        auto response = static_cast<UA_CreateMonitoredItemsResponse*>(r);

        if(response->responseHeader.serviceResult==UA_STATUSCODE_GOOD)
        {
            auto* subscription = static_cast<Subscription*>(userdata);
            for(auto i=0u; i<response->resultsSize; ++i)
            {
                subscription->createMonitoredItemAsyncEnd(requestId, response->results[i].monitoredItemId);
            }            
        }
    } 
    
    UA_UInt32 m_subscriptionId{0u};
    std::vector<MonitoredItem> m_monitoredItems{};
    std::unordered_map<UA_UInt32, MonitoredItem> m_pendingRequests{};
};
}