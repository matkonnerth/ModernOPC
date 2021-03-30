#pragma once
struct TNode;
struct Extension;

namespace modernopc
{
Extension *Extension_new(const TNode *);
void Extension_start(Extension *ext, const char *localname);
void Extension_end(Extension *val, const char *localname, char *value);
void Extension_finish(Extension *val);

} // namespace modernopc