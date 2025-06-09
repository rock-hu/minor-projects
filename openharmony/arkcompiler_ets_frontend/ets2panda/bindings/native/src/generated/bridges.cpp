/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <common.h>
#include "public/es2panda_lib.h"

// NOLINTBEGIN

// CC-OFFNXT(G.FUN.01-CPP) solid logic
KNativePointer impl_CreateMemberExpression(KNativePointer context, KNativePointer objectArg, KNativePointer property,
                                           KInt kind, KBoolean computed, KBoolean optionalArg)
{
    const auto ctx = reinterpret_cast<es2panda_Context *>(context);
    const auto objArg = reinterpret_cast<es2panda_AstNode *>(objectArg);
    const auto prop = reinterpret_cast<es2panda_AstNode *>(property);
    const auto expressionKind = static_cast<Es2pandaMemberExpressionKind>(kind);
    const auto isComputed = static_cast<KBoolean>(computed);
    const auto optArg = static_cast<KBoolean>(optionalArg);
    const auto result = GetPublicImpl()->CreateMemberExpression(ctx, objArg, prop, expressionKind, isComputed, optArg);
    return result;
}
// CC-OFFNXT(G.FUN.01-CPP) solid logic
TS_INTEROP_6(CreateMemberExpression, KNativePointer, KNativePointer, KNativePointer, KNativePointer, KInt, KBoolean,
             KBoolean)

// CC-OFFNXT(G.FUN.01-CPP) solid logic
KNativePointer impl_UpdateMemberExpression(KNativePointer context, KNativePointer original, KNativePointer object_arg,
                                           KNativePointer property, KInt kind, KBoolean computed, KBoolean optional_arg)
{
    const auto _context = reinterpret_cast<es2panda_Context *>(context);
    const auto _original = reinterpret_cast<es2panda_AstNode *>(original);
    const auto _object_arg = reinterpret_cast<es2panda_AstNode *>(object_arg);
    const auto _property = reinterpret_cast<es2panda_AstNode *>(property);
    const auto _kind = static_cast<Es2pandaMemberExpressionKind>(kind);
    const auto _computed = static_cast<KBoolean>(computed);
    const auto _optional_arg = static_cast<KBoolean>(optional_arg);
    const auto result = GetPublicImpl()->UpdateMemberExpression(_context, _original, _object_arg, _property, _kind,
                                                                _computed, _optional_arg);
    return result;
}
// CC-OFFNXT(G.FUN.01-CPP) solid logic
TS_INTEROP_7(UpdateMemberExpression, KNativePointer, KNativePointer, KNativePointer, KNativePointer, KNativePointer,
             KInt, KBoolean, KBoolean)

KNativePointer impl_MemberExpressionObject(KNativePointer context, KNativePointer receiver)
{
    const auto _context = reinterpret_cast<es2panda_Context *>(context);
    const auto _receiver = reinterpret_cast<es2panda_AstNode *>(receiver);
    const auto result = GetPublicImpl()->MemberExpressionObject(_context, _receiver);
    return result;
}
TS_INTEROP_2(MemberExpressionObject, KNativePointer, KNativePointer, KNativePointer)

KNativePointer impl_MemberExpressionProperty(KNativePointer context, KNativePointer receiver)
{
    const auto _context = reinterpret_cast<es2panda_Context *>(context);
    const auto _receiver = reinterpret_cast<es2panda_AstNode *>(receiver);
    const auto result = GetPublicImpl()->MemberExpressionProperty(_context, _receiver);
    return result;
}
TS_INTEROP_2(MemberExpressionProperty, KNativePointer, KNativePointer, KNativePointer)

KInt impl_MemberExpressionKindConst(KNativePointer context, KNativePointer receiver)
{
    const auto _context = reinterpret_cast<es2panda_Context *>(context);
    const auto _receiver = reinterpret_cast<es2panda_AstNode *>(receiver);
    const auto result = GetPublicImpl()->MemberExpressionKindConst(_context, _receiver);
    return result;
}
TS_INTEROP_2(MemberExpressionKindConst, KInt, KNativePointer, KNativePointer)

// NOLINTEND
