/**
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

#include "lsp_api_test.h"
#include "lsp/include/completions.h"
#include "lsp/include/internal_api.h"

class LSPCompletionsTests : public LSPAPITests {};

using ark::es2panda::lsp::CompletionEntry;
using ark::es2panda::lsp::CompletionEntryKind;
using ark::es2panda::lsp::Initializer;
using ark::es2panda::lsp::sort_text::CLASS_MEMBER_SNIPPETS;
using ark::es2panda::lsp::sort_text::GLOBALS_OR_KEYWORDS;
using ark::es2panda::lsp::sort_text::MEMBER_DECLARED_BY_SPREAD_ASSIGNMENT;
using ark::es2panda::lsp::sort_text::SUGGESTED_CLASS_MEMBERS;

static void AssertCompletionsContainAndNotContainEntries(const std::vector<CompletionEntry> &entries,
                                                         const std::vector<CompletionEntry> &expectedEntries,
                                                         const std::vector<CompletionEntry> &unexpectedEntries)
{
    auto emptyCheck = expectedEntries.empty() && !entries.empty();
    ASSERT_FALSE(emptyCheck) << "Expected empty but the result is not. Actual account: " << entries.size();

    for (const auto &expectedEntry : expectedEntries) {
        bool found = false;
        for (const auto &entry : entries) {
            if (entry.GetName() == expectedEntry.GetName() &&
                entry.GetCompletionKind() == expectedEntry.GetCompletionKind()) {
                found = true;
                break;
            }
        }
        ASSERT_TRUE(found) << "Expected completion '" << expectedEntry.GetName() << "' not found";
    }

    for (const auto &unexpectedEntry : unexpectedEntries) {
        bool found = false;
        for (const auto &entry : entries) {
            if (entry.GetName() == unexpectedEntry.GetName() &&
                entry.GetCompletionKind() == unexpectedEntry.GetCompletionKind()) {
                found = true;
                break;
            }
        }
        ASSERT_FALSE(found) << "Unexpected completion '" << unexpectedEntry.GetName() << "' found";
    }
}

namespace {

TEST_F(LSPCompletionsTests, getCompletionsAtPosition19)
{
    std::vector<std::string> files = {"getCompletionsAtPosition20.ets"};
    std::vector<std::string> texts = {R"delimiter(
deep
)delimiter"};
    auto filePaths = CreateTempFile(files, texts);

    int const expectedFileCount = 1;
    ASSERT_EQ(filePaths.size(), expectedFileCount);

    LSPAPI const *lspApi = GetImpl();
    size_t const offset = 5;
    Initializer initializer = Initializer();
    auto ctx = initializer.CreateContext(filePaths[0].c_str(), ES2PANDA_STATE_CHECKED);
    auto res = lspApi->getCompletionsAtPosition(ctx, offset);
    auto expectedEntries = std::vector<CompletionEntry> {
        CompletionEntry("deepcopy", ark::es2panda::lsp::CompletionEntryKind::METHOD, std::string(GLOBALS_OR_KEYWORDS))};
    AssertCompletionsContainAndNotContainEntries(res.GetEntries(), expectedEntries, {});
    initializer.DestroyContext(ctx);
}

TEST_F(LSPCompletionsTests, getCompletionsAtPosition18)
{
    std::vector<std::string> files = {"getCompletionsAtPosition19.ets"};
    std::vector<std::string> texts = {R"delimiter(
Readonl
)delimiter"};
    auto filePaths = CreateTempFile(files, texts);

    int const expectedFileCount = 1;
    ASSERT_EQ(filePaths.size(), expectedFileCount);

    LSPAPI const *lspApi = GetImpl();
    size_t const offset = 8;
    Initializer initializer = Initializer();
    auto ctx = initializer.CreateContext(filePaths[0].c_str(), ES2PANDA_STATE_CHECKED);
    auto res = lspApi->getCompletionsAtPosition(ctx, offset);
    auto expectedEntries = std::vector<CompletionEntry> {CompletionEntry(
        "ReadonlyArray", ark::es2panda::lsp::CompletionEntryKind::INTERFACE, std::string(GLOBALS_OR_KEYWORDS))};
    AssertCompletionsContainAndNotContainEntries(res.GetEntries(), expectedEntries, {});
    initializer.DestroyContext(ctx);
}

TEST_F(LSPCompletionsTests, getCompletionsAtPosition17)
{
    std::vector<std::string> files = {"getCompletionsAtPosition18.ets"};
    std::vector<std::string> texts = {R"delimiter(
con
)delimiter"};
    auto filePaths = CreateTempFile(files, texts);

    int const expectedFileCount = 1;
    ASSERT_EQ(filePaths.size(), expectedFileCount);

    LSPAPI const *lspApi = GetImpl();
    size_t const offset = 4;
    Initializer initializer = Initializer();
    auto ctx = initializer.CreateContext(filePaths[0].c_str(), ES2PANDA_STATE_CHECKED);
    auto res = lspApi->getCompletionsAtPosition(ctx, offset);
    auto expectedEntries = std::vector<CompletionEntry> {
        CompletionEntry("console", ark::es2panda::lsp::CompletionEntryKind::PROPERTY, std::string(GLOBALS_OR_KEYWORDS)),
        CompletionEntry("Console", ark::es2panda::lsp::CompletionEntryKind::CLASS, std::string(GLOBALS_OR_KEYWORDS)),
        CompletionEntry("ConcurrentHashMap", ark::es2panda::lsp::CompletionEntryKind::CLASS,
                        std::string(GLOBALS_OR_KEYWORDS))};
    AssertCompletionsContainAndNotContainEntries(res.GetEntries(), expectedEntries, {});
    initializer.DestroyContext(ctx);
}

TEST_F(LSPCompletionsTests, getCompletionsAtPosition16)
{
    std::vector<std::string> files = {"getCompletionsAtPosition17.ets"};
    std::vector<std::string> texts = {R"delimiter(
struct MyClass {
  property: string = '1'
  get() {
    return this._WILDCARD
  }
}
)delimiter"};
    auto filePaths = CreateTempFile(files, texts);

    int const expectedFileCount = 1;
    ASSERT_EQ(filePaths.size(), expectedFileCount);

    LSPAPI const *lspApi = GetImpl();
    size_t const offset = 78;  // after 'j._WILDCARD'
    Initializer initializer = Initializer();
    auto ctx = initializer.CreateContext(filePaths[0].c_str(), ES2PANDA_STATE_CHECKED);
    auto res = lspApi->getCompletionsAtPosition(ctx, offset);
    auto expectedEntries = std::vector<CompletionEntry> {
        CompletionEntry("property", ark::es2panda::lsp::CompletionEntryKind::PROPERTY,
                        std::string(SUGGESTED_CLASS_MEMBERS)),
        CompletionEntry("get", ark::es2panda::lsp::CompletionEntryKind::METHOD, std::string(CLASS_MEMBER_SNIPPETS))};
    AssertCompletionsContainAndNotContainEntries(res.GetEntries(), expectedEntries, {});
    initializer.DestroyContext(ctx);
}

TEST_F(LSPCompletionsTests, getCompletionsAtPosition15)
{
    std::vector<std::string> files = {"getCompletionsAtPosition16.ets"};
    std::vector<std::string> texts = {R"delimiter(
export interface Method {
  get(value: number): this;
}
export interface CommonMethod {
  width(value: number): this;
  height(value: number): this;
}
export interface TextAttribute extends CommonMethod {
  font(value: number): this;
  fontColor(value: number): this;
}
export declare function Text(
  content?: string,
  value?: string
): TextAttribute
Text("Hello").font()._WILDCARD
)delimiter"};
    auto filePaths = CreateTempFile(files, texts);

    int const expectedFileCount = 1;
    ASSERT_EQ(filePaths.size(), expectedFileCount);

    LSPAPI const *lspApi = GetImpl();
    size_t const offset = 385;  // after 'j._WILDCARD'
    Initializer initializer = Initializer();
    auto ctx = initializer.CreateContext(filePaths[0].c_str(), ES2PANDA_STATE_CHECKED);
    auto res = lspApi->getCompletionsAtPosition(ctx, offset);
    auto expectedEntries = std::vector<CompletionEntry> {
        CompletionEntry("font", ark::es2panda::lsp::CompletionEntryKind::METHOD, std::string(CLASS_MEMBER_SNIPPETS)),
        CompletionEntry("fontColor", ark::es2panda::lsp::CompletionEntryKind::METHOD,
                        std::string(CLASS_MEMBER_SNIPPETS)),
        CompletionEntry("width", ark::es2panda::lsp::CompletionEntryKind::METHOD, std::string(CLASS_MEMBER_SNIPPETS)),
        CompletionEntry("height", ark::es2panda::lsp::CompletionEntryKind::METHOD, std::string(CLASS_MEMBER_SNIPPETS))};
    AssertCompletionsContainAndNotContainEntries(res.GetEntries(), expectedEntries, {});
    initializer.DestroyContext(ctx);
}

TEST_F(LSPCompletionsTests, getCompletionsAtPosition14)
{
    std::vector<std::string> files = {"getCompletionsAtPosition15.ets"};
    std::vector<std::string> texts = {R"delimiter(
export class MyClass0 {
  public property0: string = '0'
  public get0() {}
}
export class MyClass extends MyClass0 {
  public property: string = '1'
  public get() {}
}
export class MySonClass extends MyClass {
  public property2: string = '2'
}
let c = new MySonClass()
let p = c._WILDCARD
)delimiter"};
    auto filePaths = CreateTempFile(files, texts);

    int const expectedFileCount = 1;
    ASSERT_EQ(filePaths.size(), expectedFileCount);

    LSPAPI const *lspApi = GetImpl();
    size_t const offset = 292;  // after 'j._WILDCARD'
    Initializer initializer = Initializer();
    auto ctx = initializer.CreateContext(filePaths[0].c_str(), ES2PANDA_STATE_CHECKED);
    auto res = lspApi->getCompletionsAtPosition(ctx, offset);
    auto expectedEntries = std::vector<CompletionEntry> {
        CompletionEntry("property", ark::es2panda::lsp::CompletionEntryKind::PROPERTY,
                        std::string(SUGGESTED_CLASS_MEMBERS)),
        CompletionEntry("property", ark::es2panda::lsp::CompletionEntryKind::PROPERTY,
                        std::string(SUGGESTED_CLASS_MEMBERS)),
        CompletionEntry("property2", ark::es2panda::lsp::CompletionEntryKind::PROPERTY,
                        std::string(SUGGESTED_CLASS_MEMBERS)),
        CompletionEntry("get", ark::es2panda::lsp::CompletionEntryKind::METHOD, std::string(CLASS_MEMBER_SNIPPETS)),
        CompletionEntry("get0", ark::es2panda::lsp::CompletionEntryKind::METHOD, std::string(CLASS_MEMBER_SNIPPETS))};
    AssertCompletionsContainAndNotContainEntries(res.GetEntries(), expectedEntries, {});
    initializer.DestroyContext(ctx);
}

TEST_F(LSPCompletionsTests, getCompletionsAtPosition13)
{
    std::vector<std::string> files = {"getCompletionsAtPosition14.ets"};
    std::vector<std::string> texts = {R"delimiter(
export interface Method {
  get(value: number): this;
}
export interface CommonMethod {
  width(value: number): this;
  height(value: number): this;
}
export interface TextAttribute extends CommonMethod {
  font(value: number): this;
  fontColor(value: number): this;
}
export declare function Text(
  content?: string,
  value?: string
): TextAttribute
Text("Hello")._WILDCARD
)delimiter"};
    auto filePaths = CreateTempFile(files, texts);

    int const expectedFileCount = 1;
    ASSERT_EQ(filePaths.size(), expectedFileCount);

    LSPAPI const *lspApi = GetImpl();
    size_t const offset = 378;  // after 'j._WILDCARD'
    Initializer initializer = Initializer();
    auto ctx = initializer.CreateContext(filePaths[0].c_str(), ES2PANDA_STATE_CHECKED);
    auto res = lspApi->getCompletionsAtPosition(ctx, offset);
    auto expectedEntries = std::vector<CompletionEntry> {
        CompletionEntry("font", ark::es2panda::lsp::CompletionEntryKind::METHOD, std::string(CLASS_MEMBER_SNIPPETS)),
        CompletionEntry("fontColor", ark::es2panda::lsp::CompletionEntryKind::METHOD,
                        std::string(CLASS_MEMBER_SNIPPETS)),
        CompletionEntry("width", ark::es2panda::lsp::CompletionEntryKind::METHOD, std::string(CLASS_MEMBER_SNIPPETS)),
        CompletionEntry("height", ark::es2panda::lsp::CompletionEntryKind::METHOD, std::string(CLASS_MEMBER_SNIPPETS))};
    AssertCompletionsContainAndNotContainEntries(res.GetEntries(), expectedEntries, {});
    initializer.DestroyContext(ctx);
}

TEST_F(LSPCompletionsTests, getCompletionsAtPosition12)
{
    std::vector<std::string> files = {"getCompletionsAtPosition13.ets"};
    std::vector<std::string> texts = {R"delimiter(
class JSON {
  public static stringify(d: byte): String {
    return StringBuilder.toString(d)
  }
}
let j = new JSON()
let res = j._WILDCARD
)delimiter"};
    auto filePaths = CreateTempFile(files, texts);
    int const expectedFileCount = 1;
    ASSERT_EQ(filePaths.size(), expectedFileCount);

    LSPAPI const *lspApi = GetImpl();
    size_t const offset = 142;  // after 'j._WILDCARD'
    Initializer initializer = Initializer();
    auto ctx = initializer.CreateContext(filePaths[0].c_str(), ES2PANDA_STATE_CHECKED);
    auto res = lspApi->getCompletionsAtPosition(ctx, offset);
    auto expectedEntries = std::vector<CompletionEntry> {CompletionEntry(
        "stringify", ark::es2panda::lsp::CompletionEntryKind::METHOD, std::string(CLASS_MEMBER_SNIPPETS))};
    AssertCompletionsContainAndNotContainEntries(res.GetEntries(), expectedEntries, {});
    initializer.DestroyContext(ctx);
}

TEST_F(LSPCompletionsTests, getCompletionsAtPosition11)
{
    std::vector<std::string> files = {"getCompletionsAtPosition12.ets"};
    std::vector<std::string> texts = {R"delimiter(
interface Inner { key : string; }
let i: Inner
i.k
let a = 1;)delimiter"};
    auto filePaths = CreateTempFile(files, texts);
    int const expectedFileCount = 1;
    ASSERT_EQ(filePaths.size(), expectedFileCount);

    LSPAPI const *lspApi = GetImpl();
    size_t const offset = 51;  // after 'i.k'
    Initializer initializer = Initializer();
    auto ctx = initializer.CreateContext(filePaths[0].c_str(), ES2PANDA_STATE_CHECKED);
    auto res = lspApi->getCompletionsAtPosition(ctx, offset);
    auto expectedEntries = std::vector<CompletionEntry> {
        CompletionEntry("key", ark::es2panda::lsp::CompletionEntryKind::METHOD, std::string(CLASS_MEMBER_SNIPPETS))};
    AssertCompletionsContainAndNotContainEntries(res.GetEntries(), expectedEntries, {});
    initializer.DestroyContext(ctx);
}

TEST_F(LSPCompletionsTests, getCompletionsAtPosition10)
{
    std::vector<std::string> files = {"getCompletionsAtPosition11.ets"};
    std::vector<std::string> texts = {R"delimiter(
interface Inner { key : string; }
let i: Inner
i._WILDCARD
let a = 1;)delimiter"};
    auto filePaths = CreateTempFile(files, texts);
    int const expectedFileCount = 1;
    ASSERT_EQ(filePaths.size(), expectedFileCount);

    LSPAPI const *lspApi = GetImpl();
    size_t const offset = 59;  // after 'i._WILDCARD'
    Initializer initializer = Initializer();
    auto ctx = initializer.CreateContext(filePaths[0].c_str(), ES2PANDA_STATE_CHECKED);
    auto res = lspApi->getCompletionsAtPosition(ctx, offset);
    auto expectedEntries = std::vector<CompletionEntry> {
        CompletionEntry("key", ark::es2panda::lsp::CompletionEntryKind::METHOD, std::string(CLASS_MEMBER_SNIPPETS))};
    AssertCompletionsContainAndNotContainEntries(res.GetEntries(), expectedEntries, {});
    initializer.DestroyContext(ctx);
}

TEST_F(LSPCompletionsTests, getCompletionsAtPosition7)
{
    std::vector<std::string> files = {"getCompletionsAtPosition10.ets"};
    std::vector<std::string> texts = {R"delimiter(
class MyClass {
  public myProp: number = 0;
  public prop: number = 0;
}
let obj = new MyClass();
let p = obj._WILDCARD
let a = 1;)delimiter"};
    auto filePaths = CreateTempFile(files, texts);
    int const expectedFileCount = 1;
    ASSERT_EQ(filePaths.size(), expectedFileCount);

    LSPAPI const *lspApi = GetImpl();
    size_t const offset = 121;  // after 'obj._WILDCARD' in 'let p = obj._WILDCARD'
    Initializer initializer = Initializer();
    auto ctx = initializer.CreateContext(filePaths[0].c_str(), ES2PANDA_STATE_CHECKED);
    auto res = lspApi->getCompletionsAtPosition(ctx, offset);
    auto expectedEntries =
        std::vector<CompletionEntry> {CompletionEntry("myProp", ark::es2panda::lsp::CompletionEntryKind::PROPERTY,
                                                      std::string(SUGGESTED_CLASS_MEMBERS)),
                                      CompletionEntry("prop", ark::es2panda::lsp::CompletionEntryKind::PROPERTY,
                                                      std::string(SUGGESTED_CLASS_MEMBERS))};
    AssertCompletionsContainAndNotContainEntries(res.GetEntries(), expectedEntries, {});
    initializer.DestroyContext(ctx);
}

TEST_F(LSPCompletionsTests, getCompletionsAtPosition8)
{
    std::vector<std::string> files = {"getCompletionsAtPosition11.ets"};
    std::vector<std::string> texts = {R"delimiter(
enum Color {
  Red = "red",
  Blue = "blue"
}
let myColor: Color = Color._WILDCARD)delimiter"};
    auto filePaths = CreateTempFile(files, texts);
    int const expectedFileCount = 1;
    ASSERT_EQ(filePaths.size(), expectedFileCount);

    LSPAPI const *lspApi = GetImpl();
    size_t const offset = 83;  // after 'Color._WILDCARD'
    Initializer initializer = Initializer();
    auto ctx = initializer.CreateContext(filePaths[0].c_str(), ES2PANDA_STATE_CHECKED);
    auto res = lspApi->getCompletionsAtPosition(ctx, offset);
    auto expectedEntries = std::vector<CompletionEntry> {
        CompletionEntry("Red", CompletionEntryKind::ENUM_MEMBER, std::string(MEMBER_DECLARED_BY_SPREAD_ASSIGNMENT)),
        CompletionEntry("Blue", CompletionEntryKind::ENUM_MEMBER, std::string(MEMBER_DECLARED_BY_SPREAD_ASSIGNMENT))};
    AssertCompletionsContainAndNotContainEntries(res.GetEntries(), expectedEntries, {});
    initializer.DestroyContext(ctx);
}

}  // namespace
