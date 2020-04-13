/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 *    Copyright 2019 (c) Matthias Konnerth
 */

#include "backend.h"
#include "conversion.h"
#include "stdio.h"
#include "DataType.h"
#include <unordered_map>

int addNamespace(void *userContext, const char *uri) { return 1; }

void addNode(void *userContext, const TNode *node)
{

    auto types = static_cast<std::unordered_map<opc::NodeId, gen::DataType> *>(userContext);
    if (node->nodeClass == NODECLASS_DATATYPE)
    {
        const TDataTypeNode *n = (const TDataTypeNode *)node;

        gen::DataType type{n->browseName.name, extractNodeId(n->id.idString), n->definition->isEnum};
        for (size_t i = 0; i < n->definition->fieldCnt; i++)
        {
            type.fields.emplace_back(gen::DataTypeDefinitionField{
                n->definition->fields[i].name,
                extractNodeId(n->definition->fields[i].dataType.idString),
                n->definition->fields[i].valueRank,
                n->definition->fields[i].value});
        }
        types->insert(std::make_pair(type.id, type));
    }
}

struct Value *Value_new(const TNode *node) { return NULL; }

void Value_start(Value *val, const char *localname) {}
void Value_end(Value *val, const char *localname, char *value) {}
void Value_finish(Value *val) {}
void Value_delete(Value **val) {}
