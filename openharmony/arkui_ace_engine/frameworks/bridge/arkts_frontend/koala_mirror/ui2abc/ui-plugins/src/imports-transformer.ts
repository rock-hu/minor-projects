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

import { Importer } from "./utils"
import * as arkts from "@koalaui/libarkts"

export class ImportsTransformer extends arkts.AbstractVisitor {
    constructor(private imports: Importer) {
        super()
    }

    visitor(node: arkts.AstNode): arkts.AstNode {
        if (arkts.isETSModule(node)) {
            return arkts.updateETSModuleByStatements(
                node,
                this.imports.emit(node.statements)
            )
        }
        throw new Error(`Must not be there`)
    }
}