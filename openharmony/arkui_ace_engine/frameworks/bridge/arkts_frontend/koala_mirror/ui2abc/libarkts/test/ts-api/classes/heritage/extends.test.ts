/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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

import * as util from "../../../test-util"
import * as ts from "../../../../src/ts-api"
import { factory } from "../../../../src/ts-api"

// TODO:
suite.skip(util.basename(__filename), () => {
    test("sample-1", function() {
        const sample_in =
        `
        abstract class A {};
        abstract class C {};
        interface D {};
        class B extends A implements C, D {};
        `

        let sourceFile = factory.createSourceFile(sample_in)
        util.assert(ts.isSourceFile(sourceFile))

        // sourceFile = factory.updateSourceFile(
        //     sourceFile,
        //     [
        //         factory.createClassDeclaration(
        //             [
        //                 factory.createToken(ts.SyntaxKind.AbstractKeyword)
        //             ],
        //             factory.createIdentifier("A"),
        //             undefined,
        //             undefined,
        //             []
        //         ),
        //         factory.createClassDeclaration(
        //             undefined,
        //             factory.createIdentifier("B"),
        //             undefined,
        //             [
        //                 factory.createHeritageClause(
        //                     ts.SyntaxKind.ExtendsKeyword,
        //                     [
        //                         factory.createExpressionWithTypeArguments(
        //                             factory.createIdentifier("A"),
        //                             undefined
        //                         )
        //                     ]
        //                 )
        //             ],
        //             []
        //         )
        //     ]
        // )

        util.TS_TEST_ASSERTION(
            sourceFile,
            `
            abstract class A {};
            class B extends A {};
            `
        )
    })
})
