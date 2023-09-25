#pragma once
#include <modernopc/StatusCode.h>

template<typename T>
class Result
{
    Result(StatusCode status, T&& value):m_status{status}, m_value{std::forward<T>(value)}{}
private:
    StatusCode m_status;
    T m_value;
};