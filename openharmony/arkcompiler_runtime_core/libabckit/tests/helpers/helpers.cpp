/**
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "helpers/helpers.h"

#include "libabckit/include/c/abckit.h"
#include "libabckit/include/c/isa/isa_dynamic.h"
#include "libabckit/include/c/metadata_core.h"
#include "src/metadata_inspect_impl.h"
#include "libabckit/src/logger.h"
#include "libabckit/include/c/ir_core.h"
#include "libabckit/include/c/statuses.h"

#include <cassert>
#include <regex>
#include <vector>
#include <unordered_map>

#include <gtest/gtest.h>

namespace libabckit::test::helpers {

bool Match(const std::string &actual, const std::string &expected)
{
    std::smatch m;
    auto res = std::regex_match(actual, m, std::regex(expected));
    if (!res) {
        LIBABCKIT_LOG_TEST(DEBUG) << "Doesn't match:\n";
        LIBABCKIT_LOG_TEST(DEBUG) << "Actual:\n" << actual << '\n';
        LIBABCKIT_LOG_TEST(DEBUG) << "Expected:\n" << expected << '\n';
    }
    return res;
}

struct UserTransformerData {
    AbckitCoreFunction *method;
    const std::function<void(AbckitFile *, AbckitCoreFunction *, AbckitGraph *)> &userTransformer;
};

static void TransformMethodImpl(AbckitFile *file, AbckitCoreFunction *method, void *data)
{
    auto userTransformerData = reinterpret_cast<UserTransformerData *>(data);

    AbckitGraph *graph = g_implI->createGraphFromFunction(method);
    ASSERT_EQ(g_impl->getLastError(), ABCKIT_STATUS_NO_ERROR);

    userTransformerData->userTransformer(file, method, graph);

    g_implM->functionSetGraph(method, graph);
    g_impl->destroyGraph(graph);
    ASSERT_EQ(g_impl->getLastError(), ABCKIT_STATUS_NO_ERROR);
}

void TransformMethod(AbckitFile *file, const std::string &methodSignature,
                     const std::function<void(AbckitFile *, AbckitCoreFunction *, AbckitGraph *)> &userTransformer)
{
    auto *method = FindMethodByName(file, methodSignature);
    ASSERT_NE(method, nullptr);

    UserTransformerData utd({method, userTransformer});

    TransformMethodImpl(g_implI->functionGetFile(method), method, &utd);
    ASSERT_EQ(g_impl->getLastError(), ABCKIT_STATUS_NO_ERROR);
}

void TransformMethod(const std::string &inputPath, const std::string &outputPath, const std::string &methodSignature,
                     const std::function<void(AbckitFile *, AbckitCoreFunction *, AbckitGraph *)> &userTransformer,
                     const std::function<void(AbckitGraph *)> &validateResult)
{
    LIBABCKIT_LOG_TEST(DEBUG) << "TransformMethod: " << inputPath << '\n';

    // Open file
    AbckitFile *file = g_impl->openAbc(inputPath.c_str());
    ASSERT_EQ(g_impl->getLastError(), ABCKIT_STATUS_NO_ERROR);

    // Transform method
    auto *method = FindMethodByName(file, methodSignature);
    ASSERT_NE(method, nullptr);
    UserTransformerData utd({method, userTransformer});
    TransformMethodImpl(g_implI->functionGetFile(method), method, &utd);
    ASSERT_EQ(g_impl->getLastError(), ABCKIT_STATUS_NO_ERROR);

    // Validate method
    AbckitGraph *graph = g_implI->createGraphFromFunction(method);
    ASSERT_EQ(g_impl->getLastError(), ABCKIT_STATUS_NO_ERROR);
    validateResult(graph);
    g_impl->destroyGraph(graph);

    // Write output
    g_impl->writeAbc(file, outputPath.c_str());
    g_impl->closeFile(file);
    ASSERT_EQ(g_impl->getLastError(), ABCKIT_STATUS_NO_ERROR);
}

void TransformMethod(const std::string &inputPath, const std::string &outputPath, const std::string &methodSignature,
                     const std::function<void(AbckitFile *, AbckitCoreFunction *, AbckitGraph *)> &userTransformer)
{
    LIBABCKIT_LOG_TEST(DEBUG) << "TransformMethod: " << inputPath << '\n';

    // Open file
    AbckitFile *file = g_impl->openAbc(inputPath.c_str());
    ASSERT_EQ(g_impl->getLastError(), ABCKIT_STATUS_NO_ERROR);

    // Transform method
    auto *method = FindMethodByName(file, methodSignature);
    ASSERT_NE(method, nullptr);
    UserTransformerData utd({method, userTransformer});
    TransformMethodImpl(g_implI->functionGetFile(method), method, &utd);
    ASSERT_EQ(g_impl->getLastError(), ABCKIT_STATUS_NO_ERROR);

    // Write output
    g_impl->writeAbc(file, outputPath.c_str());
    g_impl->closeFile(file);
    ASSERT_EQ(g_impl->getLastError(), ABCKIT_STATUS_NO_ERROR);
}

struct UserInspectorData {
    AbckitCoreFunction *method;
    const std::function<void(AbckitFile *, AbckitCoreFunction *, AbckitGraph *)> &userInspector;
};

static void InspectMethodImpl(AbckitFile *file, AbckitCoreFunction *method, void *data)
{
    auto userInspectorData = reinterpret_cast<UserInspectorData *>(data);

    AbckitGraph *graph = g_implI->createGraphFromFunction(method);
    ASSERT_EQ(g_impl->getLastError(), ABCKIT_STATUS_NO_ERROR);

    userInspectorData->userInspector(file, method, graph);

    g_impl->destroyGraph(graph);
}

void InspectMethod(AbckitFile *file, const std::string &methodSignature,
                   const std::function<void(AbckitFile *, AbckitCoreFunction *, AbckitGraph *)> &userInspector)
{
    auto *method = FindMethodByName(file, methodSignature);
    ASSERT_NE(method, nullptr);

    UserInspectorData uid({method, userInspector});

    InspectMethodImpl(g_implI->functionGetFile(method), method, &uid);
    ASSERT_EQ(g_impl->getLastError(), ABCKIT_STATUS_NO_ERROR);
}

void InspectMethod(const std::string &inputPath, const std::string &methodSignature,
                   const std::function<void(AbckitFile *, AbckitCoreFunction *, AbckitGraph *)> &userInspector)
{
    LIBABCKIT_LOG_TEST(DEBUG) << "InspectMethod: " << inputPath << '\n';

    // Open file
    AbckitFile *file = g_impl->openAbc(inputPath.c_str());
    ASSERT_EQ(g_impl->getLastError(), ABCKIT_STATUS_NO_ERROR);

    // Inspect method
    auto *method = FindMethodByName(file, methodSignature);
    ASSERT_NE(method, nullptr);
    UserInspectorData uid({method, userInspector});
    InspectMethodImpl(file, method, &uid);
    ASSERT_EQ(g_impl->getLastError(), ABCKIT_STATUS_NO_ERROR);

    g_impl->closeFile(file);
}

std::vector<AbckitBasicBlock *> BBgetPredBlocks(AbckitBasicBlock *bb)
{
    std::vector<AbckitBasicBlock *> predBBs;
    g_implG->bbVisitPredBlocks(bb, &predBBs, [](AbckitBasicBlock *succBasicBlock, void *d) {
        auto *preds = reinterpret_cast<std::vector<AbckitBasicBlock *> *>(d);
        preds->emplace_back(succBasicBlock);
    });
    return predBBs;
}

std::vector<AbckitBasicBlock *> BBgetSuccBlocks(AbckitBasicBlock *bb)
{
    std::vector<AbckitBasicBlock *> succBBs;
    g_implG->bbVisitSuccBlocks(bb, &succBBs, [](AbckitBasicBlock *succBasicBlock, void *d) {
        auto *succs = reinterpret_cast<std::vector<AbckitBasicBlock *> *>(d);
        succs->emplace_back(succBasicBlock);
    });
    return succBBs;
}

std::vector<AbckitInst *> BBgetAllInsts(AbckitBasicBlock *bb)
{
    std::vector<AbckitInst *> insts;
    for (auto *inst = g_implG->bbGetFirstInst(bb); inst != nullptr; inst = g_implG->iGetNext(inst)) {
        insts.emplace_back(inst);
    }

    return insts;
}

AbckitInst *FindFirstInst(AbckitGraph *graph, AbckitIsaApiStaticOpcode opcode,
                          const std::function<bool(AbckitInst *)> &findIf)
{
    std::vector<AbckitBasicBlock *> bbs;
    g_implG->gVisitBlocksRpo(graph, &bbs, [](AbckitBasicBlock *bb, void *data) {
        reinterpret_cast<std::vector<AbckitBasicBlock *> *>(data)->emplace_back(bb);
    });
    for (auto *bb : bbs) {
        auto *curInst = g_implG->bbGetFirstInst(bb);
        while (curInst != nullptr) {
            if (g_gStat->iGetOpcode(curInst) == opcode && findIf(curInst)) {
                return curInst;
            }
            curInst = g_implG->iGetNext(curInst);
        }
    }
    return nullptr;
}

AbckitInst *FindFirstInst(AbckitGraph *graph, AbckitIsaApiDynamicOpcode opcode,
                          const std::function<bool(AbckitInst *)> &findIf)
{
    std::vector<AbckitBasicBlock *> bbs;
    g_implG->gVisitBlocksRpo(graph, &bbs, [](AbckitBasicBlock *bb, void *data) {
        reinterpret_cast<std::vector<AbckitBasicBlock *> *>(data)->emplace_back(bb);
    });
    for (auto *bb : bbs) {
        auto *curInst = g_implG->bbGetFirstInst(bb);
        while (curInst != nullptr) {
            if (g_gDyn->iGetOpcode(curInst) == opcode && findIf(curInst)) {
                return curInst;
            }
            curInst = g_implG->iGetNext(curInst);
        }
    }
    return nullptr;
}

AbckitInst *FindLastInst(AbckitGraph *graph, AbckitIsaApiDynamicOpcode opcode,
                         const std::function<bool(AbckitInst *)> &findIf)
{
    std::vector<AbckitBasicBlock *> bbs;
    g_implG->gVisitBlocksRpo(graph, &bbs, [](AbckitBasicBlock *bb, void *data) {
        reinterpret_cast<std::vector<AbckitBasicBlock *> *>(data)->emplace_back(bb);
    });
    AbckitInst *res {nullptr};
    for (auto *bb : bbs) {
        auto *curInst = g_implG->bbGetFirstInst(bb);
        while (curInst != nullptr) {
            if (g_gDyn->iGetOpcode(curInst) == opcode && findIf(curInst)) {
                res = curInst;
            }
            curInst = g_implG->iGetNext(curInst);
        }
    }
    return res;
}

AbckitInst *FindLastInst(AbckitGraph *graph, AbckitIsaApiStaticOpcode opcode,
                         const std::function<bool(AbckitInst *)> &findIf)
{
    std::vector<AbckitBasicBlock *> bbs;
    g_implG->gVisitBlocksRpo(graph, &bbs, [](AbckitBasicBlock *bb, void *data) {
        reinterpret_cast<std::vector<AbckitBasicBlock *> *>(data)->emplace_back(bb);
    });
    AbckitInst *res {nullptr};
    for (auto *bb : bbs) {
        auto *curInst = g_implG->bbGetFirstInst(bb);
        while (curInst != nullptr) {
            if (g_gStat->iGetOpcode(curInst) == opcode && findIf(curInst)) {
                res = curInst;
            }
            curInst = g_implG->iGetNext(curInst);
        }
    }
    return res;
}

void ReplaceInst(AbckitInst *what, AbckitInst *with)
{
    ASSERT_EQ(g_impl->getLastError(), ABCKIT_STATUS_NO_ERROR);

    std::vector<AbckitInst *> users;

    struct ReplaceContext {
        AbckitInst *inst = nullptr;
        AbckitInst *oldInput = nullptr;
        AbckitInst *newInput = nullptr;
        std::vector<AbckitInst *> *users = nullptr;
    };
    ReplaceContext ctx = {what, what, with, &users};

    g_implG->iVisitUsers(what, &ctx, [](AbckitInst *user, void *data) {
        auto *users = static_cast<ReplaceContext *>(data)->users;
        users->push_back(user);
    });
    ASSERT_EQ(g_impl->getLastError(), ABCKIT_STATUS_NO_ERROR);

    for (auto *user : users) {
        ctx.inst = user;
        g_implG->iVisitInputs(user, &ctx, [](AbckitInst *input, size_t inputIdx, void *data) {
            auto *ctx = reinterpret_cast<ReplaceContext *>(data);

            if (input == ctx->oldInput) {
                g_implG->iSetInput(ctx->inst, ctx->newInput, inputIdx);
            }
        });
    }
    ASSERT_EQ(g_impl->getLastError(), ABCKIT_STATUS_NO_ERROR);

    g_implG->iInsertBefore(with, what);
    ASSERT_EQ(g_impl->getLastError(), ABCKIT_STATUS_NO_ERROR);

    g_implG->iRemove(what);
    ASSERT_EQ(g_impl->getLastError(), ABCKIT_STATUS_NO_ERROR);
}

static void EnumerateAllMethodsInModule(AbckitFile *file, std::function<void(AbckitCoreNamespace *)> &cbNamespace,
                                        std::function<void(AbckitCoreClass *)> &cbClass,
                                        std::function<void(AbckitCoreFunction *)> &cbFunc)
{
    std::function<void(AbckitCoreModule *)> cbModule = [&](AbckitCoreModule *m) {
        g_implI->moduleEnumerateNamespaces(m, &cbNamespace, [](AbckitCoreNamespace *n, void *cb) {
            (*reinterpret_cast<std::function<void(AbckitCoreNamespace *)> *>(cb))(n);
            return true;
        });
        g_implI->moduleEnumerateClasses(m, &cbClass, [](AbckitCoreClass *c, void *cb) {
            (*reinterpret_cast<std::function<void(AbckitCoreClass *)> *>(cb))(c);
            return true;
        });
        g_implI->moduleEnumerateTopLevelFunctions(m, &cbFunc, [](AbckitCoreFunction *m, void *cb) {
            (*reinterpret_cast<std::function<void(AbckitCoreFunction *)> *>(cb))(m);
            return true;
        });
    };

    g_implI->fileEnumerateModules(file, &cbModule, [](AbckitCoreModule *m, void *cb) {
        (*reinterpret_cast<std::function<void(AbckitCoreModule *)> *>(cb))(m);
        return true;
    });
}

void EnumerateAllMethods(AbckitFile *file, const std::function<void(AbckitCoreFunction *)> &cbUserFunc)
{
    std::function<void(AbckitCoreFunction *)> cbFunc;
    std::function<void(AbckitCoreClass *)> cbClass;

    cbFunc = [&](AbckitCoreFunction *f) {
        cbUserFunc(f);
        g_implI->functionEnumerateNestedFunctions(f, &cbFunc, [](AbckitCoreFunction *f, void *cb) {
            (*reinterpret_cast<std::function<void(AbckitCoreFunction *)> *>(cb))(f);
            return true;
        });
        g_implI->functionEnumerateNestedClasses(f, &cbClass, [](AbckitCoreClass *f, void *cb) {
            (*reinterpret_cast<std::function<void(AbckitCoreClass *)> *>(cb))(f);
            return true;
        });
    };

    cbClass = [&](AbckitCoreClass *c) {
        g_implI->classEnumerateMethods(c, &cbFunc, [](AbckitCoreFunction *m, void *cb) {
            (*reinterpret_cast<std::function<void(AbckitCoreFunction *)> *>(cb))(m);

            return true;
        });
    };

    std::function<void(AbckitCoreNamespace *)> cbNamespace = [&](AbckitCoreNamespace *n) {
        g_implI->namespaceEnumerateNamespaces(n, &cbNamespace, [](AbckitCoreNamespace *n, void *cb) {
            (*reinterpret_cast<std::function<void(AbckitCoreNamespace *)> *>(cb))(n);
            return true;
        });
        g_implI->namespaceEnumerateClasses(n, &cbClass, [](AbckitCoreClass *c, void *cb) {
            (*reinterpret_cast<std::function<void(AbckitCoreClass *)> *>(cb))(c);
            return true;
        });
        g_implI->namespaceEnumerateTopLevelFunctions(n, &cbFunc, [](AbckitCoreFunction *f, void *cb) {
            (*reinterpret_cast<std::function<void(AbckitCoreFunction *)> *>(cb))(f);

            return true;
        });
    };

    EnumerateAllMethodsInModule(file, cbNamespace, cbClass, cbFunc);
}

void AssertModuleVisitor([[maybe_unused]] AbckitCoreModule *module, [[maybe_unused]] void *data)
{
    EXPECT_TRUE(module != nullptr);
    EXPECT_TRUE(module->file != nullptr);
    EXPECT_TRUE(data != nullptr);
}

void AssertImportVisitor([[maybe_unused]] AbckitCoreImportDescriptor *id, [[maybe_unused]] void *data)
{
    EXPECT_TRUE(id != nullptr);
    EXPECT_TRUE(id->importingModule != nullptr);
    EXPECT_TRUE(id->importedModule != nullptr);
    EXPECT_TRUE(data != nullptr);
}

void AssertExportVisitor([[maybe_unused]] AbckitCoreExportDescriptor *ed, [[maybe_unused]] void *data)
{
    EXPECT_TRUE(ed != nullptr);
    EXPECT_TRUE(ed->exportingModule != nullptr);
    EXPECT_TRUE(ed->exportedModule != nullptr);
    EXPECT_TRUE(data != nullptr);
}

void AssertClassVisitor([[maybe_unused]] AbckitCoreClass *klass, [[maybe_unused]] void *data)
{
    EXPECT_TRUE(klass != nullptr);
    [[maybe_unused]] AbckitFile *file = g_implI->classGetFile(klass);
    EXPECT_TRUE(file != nullptr);
    EXPECT_TRUE(data != nullptr);
}

void AssertNamespaceVisitor([[maybe_unused]] AbckitCoreNamespace *n, [[maybe_unused]] void *data)
{
    EXPECT_TRUE(n != nullptr);
    EXPECT_TRUE(data != nullptr);
}

void AssertMethodVisitor([[maybe_unused]] AbckitCoreFunction *method, [[maybe_unused]] void *data)
{
    EXPECT_TRUE(method != nullptr);
    [[maybe_unused]] AbckitFile *file = g_implI->functionGetFile(method);
    EXPECT_TRUE(file != nullptr);
    EXPECT_TRUE(data != nullptr);
}

void AssertOpenAbc(const char *fname, AbckitFile **file)
{
    ASSERT_NE(g_impl, nullptr);
    ASSERT_NE(g_implI, nullptr);
    *file = g_impl->openAbc(fname);
    ASSERT_EQ(g_impl->getLastError(), ABCKIT_STATUS_NO_ERROR);
    ASSERT_NE(*file, nullptr);
}

bool ClassByNameFinder(AbckitCoreClass *klass, void *data)
{
    AssertClassVisitor(klass, data);

    auto ctxFinder = reinterpret_cast<ClassByNameContext *>(data);
    auto name = helpers::AbckitStringToString(g_implI->classGetName(klass));
    if (name == ctxFinder->name) {
        ctxFinder->klass = klass;
        return false;
    }

    return true;
}

bool NamespaceByNameFinder(AbckitCoreNamespace *n, void *data)
{
    AssertNamespaceVisitor(n, data);

    auto ctxFinder = reinterpret_cast<NamepsaceByNameContext *>(data);
    auto name = helpers::AbckitStringToString(g_implI->namespaceGetName(n));
    if (name == ctxFinder->name) {
        ctxFinder->n = n;
        return false;
    }
    return true;
}

bool ModuleByNameFinder(AbckitCoreModule *module, void *data)
{
    AssertModuleVisitor(module, data);

    auto ctxFinder = reinterpret_cast<ModuleByNameContext *>(data);
    auto name = helpers::AbckitStringToString(g_implI->moduleGetName(module));
    if (name == ctxFinder->name) {
        ctxFinder->module = module;
        return false;
    }

    return true;
}

bool ImportByAliasFinder(AbckitCoreImportDescriptor *id, void *data)
{
    AssertImportVisitor(id, data);

    auto ctxFinder = reinterpret_cast<ImportByAliasContext *>(data);
    auto name = helpers::AbckitStringToString(g_implI->importDescriptorGetAlias(id));
    if (name == ctxFinder->name) {
        ctxFinder->id = id;
        return false;
    }

    return true;
}

bool ExportByAliasFinder(AbckitCoreExportDescriptor *ed, void *data)
{
    AssertExportVisitor(ed, data);

    auto ctxFinder = reinterpret_cast<ExportByAliasContext *>(data);
    auto name = helpers::AbckitStringToString(g_implI->exportDescriptorGetAlias(ed));
    if (name == ctxFinder->name) {
        ctxFinder->ed = ed;
        return false;
    }

    return true;
}

std::string GetCropFuncName(const std::string &fullSig)
{
    auto fullName = fullSig.substr(0, fullSig.find(':'));
    auto pos = fullName.rfind('.');
    return fullName.substr(pos + 1);
}

struct NamespaceAndName final {
    AbckitCoreNamespace *foundNamespace;
    const std::string &namespaceName;
};

struct MethodAndName final {
    std::vector<AbckitCoreFunction *> foundMethods;
    const std::string &methodName;
    bool fullSign = true;
};

static std::pair<std::string, std::string> SplitFunctionName(const std::string &fullName)
{
    size_t colonPos = fullName.find(':');
    std::string croppedName = fullName.substr(0, colonPos);
    size_t dotPos = croppedName.find('.');
    if (dotPos == std::string::npos) {
        return {"", fullName};
    }
    return {fullName.substr(0, dotPos), fullName.substr(dotPos + 1)};
}

static void FindFunctionInTopLevel(AbckitCoreFunction *func, MethodAndName *mn)
{
    auto currmname = g_implI->functionGetName(func);
    auto currMethodNameStr = helpers::AbckitStringToString(currmname);
    auto currMethodName = mn->fullSign ? currMethodNameStr.data() : GetCropFuncName(currMethodNameStr.data());
    auto [methodModule, methodName] = SplitFunctionName(mn->methodName);
    if (currMethodName == methodName) {
        mn->foundMethods.emplace_back(func);
    }
}

static void FindMethodInClass(AbckitCoreClass *klass, MethodAndName *mn)
{
    g_implI->classEnumerateMethods(klass, mn, [](AbckitCoreFunction *method, void *data) {
        auto mn = static_cast<MethodAndName *>(data);
        auto currmname = g_implI->functionGetName(method);
        auto currMethodNameStr = helpers::AbckitStringToString(currmname);
        auto currMethodName = mn->fullSign ? currMethodNameStr.data() : GetCropFuncName(currMethodNameStr.data());
        auto [methodModule, methodName] = SplitFunctionName(mn->methodName);
        if (currMethodName == methodName) {
            mn->foundMethods.emplace_back(method);
        }
        return true;
    });
}

static void FindMethodInNamespace(AbckitCoreNamespace *n, MethodAndName *mn)
{
    g_implI->namespaceEnumerateClasses(n, mn, [](AbckitCoreClass *klass, void *data) {
        FindMethodInClass(klass, static_cast<MethodAndName *>(data));
        return true;
    });
    g_implI->namespaceEnumerateTopLevelFunctions(n, mn, [](AbckitCoreFunction *func, void *data) {
        FindFunctionInTopLevel(func, static_cast<MethodAndName *>(data));
        return true;
    });
    g_implI->namespaceEnumerateNamespaces(n, mn, [](AbckitCoreNamespace *n, void *data) {
        FindMethodInNamespace(n, static_cast<MethodAndName *>(data));
        return true;
    });
}

void FindMethodByNameImpl(AbckitFile *file, void *data)
{
    g_implI->fileEnumerateModules(file, data, [](AbckitCoreModule *m, void *data) {
        auto moduleName = helpers::AbckitStringToString(g_implI->moduleGetName(m));
        auto mn = static_cast<MethodAndName *>(data);
        auto [funcModule, funcName] = SplitFunctionName(mn->methodName);
        if (!funcModule.empty()) {
            if (funcModule != moduleName) {
                return true;
            }
        }
        g_implI->moduleEnumerateClasses(m, data, [](AbckitCoreClass *klass, void *data) {
            FindMethodInClass(klass, static_cast<MethodAndName *>(data));
            return true;
        });

        g_implI->moduleEnumerateTopLevelFunctions(m, data, [](AbckitCoreFunction *method, void *data) {
            FindFunctionInTopLevel(method, static_cast<MethodAndName *>(data));
            return true;
        });

        g_implI->moduleEnumerateNamespaces(m, data, [](AbckitCoreNamespace *n, void *data) {
            FindMethodInNamespace(n, static_cast<MethodAndName *>(data));
            return true;
        });
        return true;
    });
    auto mn = static_cast<MethodAndName *>(data);
    LIBABCKIT_LOG_TEST(DEBUG) << "Method found by name: " << mn->methodName << '\n';
}

static void FindNamespaceInNamespaces(AbckitCoreNamespace *n, NamespaceAndName *nn)
{
    g_implI->namespaceEnumerateNamespaces(n, nn, [](AbckitCoreNamespace *n, void *data) {
        auto nn = static_cast<NamespaceAndName *>(data);
        auto currNamespaceName = g_implI->namespaceGetName(n);
        auto currNamespaceNameStr = helpers::AbckitStringToString(currNamespaceName);
        if (currNamespaceNameStr == nn->namespaceName) {
            nn->foundNamespace = n;
        }
        FindNamespaceInNamespaces(n, nn);
        return true;
    });
}

void FindNamespaceByNameImpl(AbckitFile *file, void *data)
{
    g_implI->fileEnumerateModules(file, data, [](AbckitCoreModule *m, void *data) {
        g_implI->moduleEnumerateNamespaces(m, data, [](AbckitCoreNamespace *n, void *data) {
            auto nn = static_cast<NamespaceAndName *>(data);
            FindNamespaceInNamespaces(n, nn);
            return true;
        });

        g_implI->moduleEnumerateNamespaces(m, data, [](AbckitCoreNamespace *n, void *data) {
            auto nn = static_cast<NamespaceAndName *>(data);
            auto currNamespaceName = g_implI->namespaceGetName(n);
            auto currNamespaceNameStr = helpers::AbckitStringToString(currNamespaceName);
            if (currNamespaceNameStr == nn->namespaceName) {
                nn->foundNamespace = n;
            }
            return true;
        });
        return true;
    });
}

AbckitCoreNamespace *FindNamespaceByName(AbckitFile *file, const std::string &name)
{
    NamespaceAndName nn {nullptr, name};
    FindNamespaceByNameImpl(file, &nn);
    if (nn.foundNamespace != nullptr) {
        LIBABCKIT_LOG_TEST(DEBUG) << "Namespace found by name: " << nn.namespaceName << '\n';
    }
    return nn.foundNamespace;
}

/*
    Returns pointer to AbckitCoreFunction if found,
    nullptr otherwise
*/
AbckitCoreFunction *FindMethodByName(AbckitFile *file, const std::string &name)
{
    MethodAndName mn {{}, name};
    LIBABCKIT_LOG_TEST(DEBUG) << "Method searching: " << name << '\n';
    FindMethodByNameImpl(file, &mn);
    if (!mn.foundMethods.empty()) {
        EXPECT_EQ(mn.foundMethods.size(), 1);
        return mn.foundMethods[0];
    }

    mn.fullSign = false;
    FindMethodByNameImpl(file, &mn);
    EXPECT_EQ(mn.foundMethods.size(), 1);
    return mn.foundMethods[0];
}

bool MethodByNameFinder(AbckitCoreFunction *method, void *data)
{
    AssertMethodVisitor(method, data);

    auto ctxFinder = reinterpret_cast<MethodByNameContext *>(data);
    auto fullName = helpers::AbckitStringToString(g_implI->functionGetName(method));
    auto name = ctxFinder->fullSign ? fullName.data() : GetCropFuncName(fullName.data());
    if (name == ctxFinder->name) {
        ctxFinder->method = method;
        return false;
    }

    return true;
}

bool NameToModuleCollector(AbckitCoreModule *module, void *data)
{
    AssertModuleVisitor(module, data);

    auto modules = reinterpret_cast<std::unordered_map<std::string, AbckitCoreModule *> *>(data);
    auto moduleName = g_implI->moduleGetName(module);
    EXPECT_TRUE(g_impl->getLastError() == ABCKIT_STATUS_NO_ERROR);

    auto name = helpers::AbckitStringToString(moduleName);
    LIBABCKIT_LOG_TEST(DEBUG) << "module name: " << name << std::endl;
    EXPECT_TRUE(modules->find(name.data()) == modules->end());
    modules->emplace(name, module);

    return true;
}

bool ModuleImportsCollector(AbckitCoreImportDescriptor *id, void *data)
{
    helpers::AssertImportVisitor(id, data);

    auto imports = reinterpret_cast<std::set<AbckitCoreImportDescriptor *> *>(data);
    imports->insert(id);

    return true;
}

bool ModuleExportsCollector(AbckitCoreExportDescriptor *ed, void *data)
{
    helpers::AssertExportVisitor(ed, data);

    auto exports = reinterpret_cast<std::set<AbckitCoreExportDescriptor *> *>(data);
    exports->insert(ed);

    return true;
}

std::string_view AbckitStringToString(AbckitString *str)
{
    EXPECT_TRUE(str != nullptr);

    auto name = g_implI->abckitStringToString(str);
    EXPECT_TRUE(g_impl->getLastError() == ABCKIT_STATUS_NO_ERROR);

    return name;
}

}  // namespace libabckit::test::helpers
