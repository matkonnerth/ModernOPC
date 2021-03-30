/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 *    Copyright 2019 (c) Matthias Konnerth
 */

#include "backend.h"
#include "conversion.h"
#include "stdio.h"
#include <types/DataType.h>
#include <types/EnumDataType.h>
#include <unordered_map>

int addNamespace(void *userContext, const char *uri) { return 1; }

void addNode(void *userContext, const TNode *node)
{

    auto types = static_cast<std::unordered_map<modernopc::NodeId, std::unique_ptr<gen::DataType>> *>(
        userContext);
    if (node->nodeClass == NODECLASS_DATATYPE)
    {
        const TDataTypeNode *n = (const TDataTypeNode *)node;

        if (n->definition->isEnum)
        {
            auto type = std::make_unique<gen::EnumDataType>(n->browseName.name,
                                   extractNodeId(n->id.idString));
            for (size_t i = 0; i < n->definition->fieldCnt; i++)
            {
                type->fields.emplace_back(gen::EnumDefinitionField{
                    n->definition->fields[i].name,
                    n->definition->fields[i].valueRank, extractNodeId(
                        n->definition->fields[i].dataType.idString),
                    n->definition->fields[i].value});
            }
            types->insert(std::make_pair(type->id, std::move(type)));
        }
        else
        {
            auto type = std::make_unique<gen::StructureDataType>(n->browseName.name,
                                   extractNodeId(n->id.idString));
            for (size_t i = 0; i < n->definition->fieldCnt; i++)
            {
                type->fields.emplace_back(gen::StructureDefinitionField{
                    n->definition->fields[i].name,
                    n->definition->fields[i].valueRank, extractNodeId(
                        n->definition->fields[i].dataType.idString)});
            }
            types->insert(std::make_pair(type->id, std::move(type)));
        }
    }
}

struct Value *Value_new(const TNode *node) { return NULL; }

void Value_start(Value *val, const char *localname) {}
void Value_end(Value *val, const char *localname, char *value) {}
void Value_finish(Value *val) {}
void Value_delete(Value **val) {}
