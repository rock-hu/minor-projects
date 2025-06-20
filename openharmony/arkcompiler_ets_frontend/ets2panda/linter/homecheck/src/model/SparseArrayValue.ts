/*
 * Copyright (c) 2024 - 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
export class SparseArrayValue {

    sparseArrayType: SparseArrayType;
    baseStr: string;
    valStr: string;
    fulBaseStr: string;
    fulStmtStr: string;
    constructor(StmtType: SparseArrayType, baseStr: string, valStr: string) {
        this.sparseArrayType = StmtType;
        this.baseStr = baseStr;
        this.valStr = valStr;
        this.fulBaseStr = this.baseStr + this.valStr;
        if (this.sparseArrayType === SparseArrayType.NEW_ARRAY) {
            this.fulStmtStr = this.fulBaseStr + ')';
        } else if (this.sparseArrayType === SparseArrayType.ARRAY_RIGHT) {
            this.fulStmtStr = this.fulBaseStr + ']';
        } else if (this.sparseArrayType === SparseArrayType.ARRAY_LEFT) {
            this.fulStmtStr = this.fulBaseStr + ']';
        } else {
            this.fulStmtStr = this.fulBaseStr;
        }
    }
}

export enum SparseArrayType {
    NEW_ARRAY,
    ARRAY_RIGHT,
    ARRAY_LEFT,
    UNKNOWN
}