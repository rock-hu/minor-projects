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

import * as ts from '@koalaui/ets-tsc'
import { AbstractVisitor } from "./AbstractVisitor"
import { Importer } from './Importer'
import { IssueTable, adaptorComponentName } from './utils'
import { id } from './ApiUtils'


export class NewTransformer extends AbstractVisitor {

    private static constructableEts = ['Path', 'Rect', 'Circle', 'Ellipse', 'Line', 'Polygon', 'Polyline', 'Shape']

    constructor(
        sourceFile: ts.SourceFile,
        ctx: ts.TransformationContext,
        private importer: Importer,
        private issueTable: IssueTable
    ) {
        super(sourceFile, ctx)
    }

    visitor(beforeChildren: ts.Node): ts.Node {
        const node = this.visitEachChild(beforeChildren)

        if (ts.isNewExpression(node)) {
            const constructible = node.expression
            if (ts.isIdentifier(constructible)) {
                const component = ts.idText(constructible)
                if (NewTransformer.constructableEts.includes(component)) {
                    const arkComponent = adaptorComponentName(component)
                    this.importer.addAdaptorImport(arkComponent)

                    return ts.factory.updateNewExpression(
                        node,
                        id(arkComponent),
                        node.typeArguments,
                        node.arguments,
                    )
                }
            }
        }

        return node
    }
}
