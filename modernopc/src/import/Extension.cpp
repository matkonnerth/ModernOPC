#include "import/Extension.h"
#include <map>
#include <NodesetLoader/NodesetLoader.h>
#include <string>

struct Extension
{
    explicit Extension(const TNode *n) : node{n} {}
    Extension(const Extension &) = delete;
    Extension &operator=(const Extension &) = delete;
    const TNode *node;
    std::map<std::string, std::string> values{};
};
namespace modernopc
{
Extension *Extension_new(const TNode *n) { return new Extension(n); }

void Extension_start(Extension *ext, const char *localname) {}
void Extension_end(Extension *val, const char *localname, char *value)
{
    val->values.insert(
        std::make_pair<std::string, std::string>(localname, value));
}
void Extension_finish(Extension *val) { delete val; }

} // namespace modernopc