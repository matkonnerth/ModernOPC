#pragma once
struct TNode;

namespace opc
{

int addNamespaceCallback(void *userContext, const char *namespaceUri);
void importNodesCallback(void *userContext, const struct TNode *node);

} // namespace opc