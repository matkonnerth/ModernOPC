#pragma once
#include <open62541/types.h>

namespace modernopc
{

class StatusCode
{
public:
    explicit StatusCode(UA_StatusCode status):m_status{status}{}
    StatusCode(const StatusCode&) = default;
    StatusCode& operator=(const StatusCode&)=default;
    StatusCode(StatusCode&&)=default;
    StatusCode& operator=(StatusCode &&)=default;

    bool IsBad() const { return UA_StatusCode_isBad(m_status);}
    bool IsGood() const { return !IsBad();}
    explicit operator bool() const { return IsGood();}
    bool operator==(const StatusCode& other) const { return m_status==other.m_status;}
private:
    UA_StatusCode m_status;
};

};