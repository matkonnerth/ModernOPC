#pragma once
#include <functional>
#include <open62541/types.h>

namespace opc {
class Variant;
class NodeId;

class DataSource {
  public:
    using ReadFn = std::function<UA_StatusCode(const NodeId &, Variant &var)>;
    using WriteFn = std::function<UA_StatusCode(const NodeId &, const Variant &var)>;
    DataSource(
        const std::string &key, ReadFn read,
        WriteFn write)
        : key(key), m_read(read), m_write(write)
    {
    }

    inline UA_StatusCode
    read(const NodeId& id, Variant &var) {
        return m_read(id, var);
    }

    inline UA_StatusCode
    write(const NodeId& id, const Variant &var) {
        return m_write(id, var);
    }

    inline const std::string &
    getKey() const {
        return key;
    }

  private:
    std::string key;
    ReadFn m_read;
    WriteFn m_write;
};
}  // namespace opc