#include "import/Extension.h"
#include <nodesetLoader.h>
#include <map>
#include <string>
#include <iostream>

struct Extension
{
    explicit Extension(const TNode* n) : node{n}{}
    const TNode* node;
    std::map<std::string, std::string> values;
};

Extension *Extension_new(const TNode *n)
{
    return new Extension(n);
}

void Extension_start(Extension *ext, const char *localname)
{

}
void Extension_end(Extension *val, const char *localname, char *value)
{
    val->values.insert(std::make_pair<std::string, std::string>(localname, value));
}
void Extension_finish(Extension *val)
{
    for(auto& v: val->values)
    {
        std::cout << v.first << ": " << v.second << std::endl;
    }
    delete val;
}