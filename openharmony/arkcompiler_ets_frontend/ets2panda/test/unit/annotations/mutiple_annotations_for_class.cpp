/*
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
#include <gtest/gtest.h>
#include <string>
#include <utility>
#include <vector>
#include "assembly-program.h"
#include "test/unit/annotations/annotations_emit_test.h"

namespace ark::es2panda::compiler::test {

class MutipleAnnotationsforClass : public AnnotationEmitTest {
public:
    MutipleAnnotationsforClass() = default;

    ~MutipleAnnotationsforClass() override = default;

    void RunAnnotationEmitTest(const std::string_view text)
    {
        auto program = GetCurrentProgram(text);
        ASSERT_NE(program, nullptr);

        CheckAnnotations(program.get());
        CheckClassAnnotations(program.get());
        CheckLiteralArrayTable(program.get());
    }

    void CheckAnnotations(pandasm::Program *program)
    {
        const std::string annoName = "Anno2";
        const std::vector<std::pair<std::string, std::string>> expectedAnnotations = {
            {
                {"favorColor", "1"},
                {"color", "ETSGLOBAL$Anno2$color$0"},
                {"reviewers", "ETSGLOBAL$Anno2$reviewers$1"},
            },
        };
        CheckAnnoDecl(program, annoName, expectedAnnotations);
    }

    void CheckClassAnnotations(pandasm::Program *program)
    {
        const std::string className = "A";
        const AnnotationMap expectedClassAnnotations = {
            {"Anno1",
             {
                 {"authorName", "Mike"},
                 {"authorAge", "18"},
                 {"testBool", "1"},
             }},
            {"Anno2",
             {
                 {"favorColor", "1"},
                 {"color", "A$Anno2$color$2"},
                 {"reviewers", "A$Anno2$reviewers$3"},
             }},
            {"Anno3",
             {
                 {"reviewersAge", "A$Anno3$reviewersAge$4"},
                 {"testBools", "A$Anno3$testBools$5"},
                 {"mutiArray", "A$Anno3$mutiArray$9"},
             }},
        };
        AnnotationEmitTest::CheckClassAnnotations(program, className, expectedClassAnnotations);
    }

    void CheckLiteralArrayTable(pandasm::Program *program)
    {
        std::vector<std::pair<std::string, std::vector<AnnotationValueType>>> expectedLiteralArrayTable = {
            {"ETSGLOBAL$Anno2$color$0", std::vector<AnnotationValueType> {COLOR_0, COLOR_1}},
            {"ETSGLOBAL$Anno2$reviewers$1",
             std::vector<AnnotationValueType> {std::string("Bob"), std::string("Jim"), std::string("Tom")}},
            {"A$Anno2$color$2", std::vector<AnnotationValueType> {COLOR_0, COLOR_1}},
            {"A$Anno2$reviewers$3",
             std::vector<AnnotationValueType> {std::string("Bob"), std::string("Jim"), std::string("Tom")}},
            {"A$Anno3$reviewersAge$4",
             std::vector<AnnotationValueType> {REVIEWER_AGE_19, REVIEWER_AGE_20, REVIEWER_AGE_24}},
            {"A$Anno3$testBools$5", std::vector<AnnotationValueType> {true, true, true}},
            {"A$Anno3$mutiArray$6", std::vector<AnnotationValueType> {VALUE_9, VALUE_8, VALUE_7}},
            {"A$Anno3$mutiArray$7", std::vector<AnnotationValueType> {VALUE_6, VALUE_5, VALUE_4}},
            {"A$Anno3$mutiArray$8", std::vector<AnnotationValueType> {VALUE_3, VALUE_2, VALUE_1}},
            {"A$Anno3$mutiArray$9",
             std::vector<AnnotationValueType> {std::string("A$Anno3$mutiArray$6"), std::string("A$Anno3$mutiArray$7"),
                                               std::string("A$Anno3$mutiArray$8")}}};

        AnnotationEmitTest::CheckLiteralArrayTable(program, expectedLiteralArrayTable);
    }

private:
    NO_COPY_SEMANTIC(MutipleAnnotationsforClass);
    NO_MOVE_SEMANTIC(MutipleAnnotationsforClass);
};

TEST_F(MutipleAnnotationsforClass, mutiple_annotations_for_class)
{
    std::string_view text = R"(
        enum Color{RED, BLUE, GREEN}

        @interface Anno1 {
            authorName: string
            authorAge: number
            testBool: boolean
        }

        @interface Anno2 {
            favorColor: Color = Color.BLUE
            color: Color[] = [Color.RED, Color.BLUE]
            reviewers: string[] = ["Bob", "Jim", "Tom"]
        }

        @interface Anno3 {
            reviewersAge: number[]
            testBools: boolean[]
            mutiArray: number[][]
        }

        @Anno1({
            authorName: "Mike",
            authorAge: 18,
            testBool: true
        })
        @Anno2
        @Anno3({
            reviewersAge: [19, 20, 24],
            testBools: [true, true, true],
            mutiArray: [
                [9, 8, 7],
                [6, 5, 4],
                [3, 2, 1]
            ]
        })
        class A {}
    )";

    RunAnnotationEmitTest(text);
}

}  // namespace ark::es2panda::compiler::test