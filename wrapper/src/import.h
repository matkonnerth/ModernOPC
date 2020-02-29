#ifndef IMPORT_H
#define IMPORT_H
struct TNode;
int addNamespaceCallback(void* userContext, const char *namespaceUri);
void importNodesCallback(void *userContext, const struct TNode *node);
#endif