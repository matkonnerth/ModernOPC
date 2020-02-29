#pragma once
struct TNode;
int addNamespaceCallback(void* userContext, const char *namespaceUri);
void importNodesCallback(void *userContext, const struct TNode *node);