/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

export const BinaryExpression_Expect_IR = {
    stmts: [
        {
            text: 'this = this: @expression/BinaryExpressionTest.ts: %dflt',
            operandOriginalPositions: [
                null, null,
            ],
        },
        {
            text: 'a = 0',
            operandOriginalPositions: [
                [16, 5, 16, 6], [16, 9, 16, 10],
            ],
        },
        {
            text: 'b = 0',
            operandOriginalPositions: [
                [17, 5, 17, 6], [17, 9, 17, 10],
            ],
        },
        {
            text: '%0 = a + b',
            operandOriginalPositions: [
                [19, 10, 19, 15], [19, 10, 19, 15], [19, 10, 19, 11], [19, 14, 19, 15],
            ],
        },
        {
            text: '%1 = a - b',
            operandOriginalPositions: [
                [19, 20, 19, 25], [19, 20, 19, 25], [19, 20, 19, 21], [19, 24, 19, 25],
            ],
        },
        {
            text: 'c = %0 / %1',
            operandOriginalPositions: [
                [19, 5, 19, 6], [19, 9, 19, 26], [19, 10, 19, 15], [19, 20, 19, 25],
            ],
        },
        {
            text: 'd = a & b',
            operandOriginalPositions: [
                [20, 5, 20, 6], [20, 9, 20, 14], [20, 9, 20, 10], [20, 13, 20, 14],
            ],
        },
        {
            text: '%2 = a || b',
            operandOriginalPositions: [
                [21, 10, 21, 16], [21, 10, 21, 16], [21, 10, 21, 11], [21, 15, 21, 16],
            ],
        },
        {
            text: 'e = %2 && a',
            operandOriginalPositions: [
                [21, 5, 21, 6], [21, 9, 21, 22], [21, 10, 21, 16], [21, 21, 21, 22],
            ],
        },
        {
            text: 'f = a ** b',
            operandOriginalPositions: [
                [23, 5, 23, 6], [23, 9, 23, 15], [23, 9, 23, 10], [23, 14, 23, 15],
            ],
        },
        {
            text: 'f = a / b',
            operandOriginalPositions: [
                [24, 1, 24, 2], [24, 5, 24, 10], [24, 5, 24, 6], [24, 9, 24, 10],
            ],
        },
        {
            text: 'f = a + b',
            operandOriginalPositions: [
                [25, 1, 25, 2], [25, 5, 25, 10], [25, 5, 25, 6], [25, 9, 25, 10],
            ],
        },
        {
            text: 'f = a - b',
            operandOriginalPositions: [
                [26, 1, 26, 2], [26, 5, 26, 10], [26, 5, 26, 6], [26, 9, 26, 10],
            ],
        },
        {
            text: 'f = a * b',
            operandOriginalPositions: [
                [27, 1, 27, 2], [27, 5, 27, 10], [27, 5, 27, 6], [27, 9, 27, 10],
            ],
        },
        {
            text: 'f = a % b',
            operandOriginalPositions: [
                [28, 1, 28, 2], [28, 5, 28, 10], [28, 5, 28, 6], [28, 9, 28, 10],
            ],
        },
        {
            text: 'f = a << b',
            operandOriginalPositions: [
                [30, 1, 30, 2], [30, 5, 30, 11], [30, 5, 30, 6], [30, 10, 30, 11],
            ],
        },
        {
            text: 'f = a >> b',
            operandOriginalPositions: [
                [31, 1, 31, 2], [31, 5, 31, 11], [31, 5, 31, 6], [31, 10, 31, 11],
            ],
        },
        {
            text: 'f = a >>> b',
            operandOriginalPositions: [
                [32, 1, 32, 2], [32, 5, 32, 12], [32, 5, 32, 6], [32, 11, 32, 12],
            ],
        },
        {
            text: 'f = a & b',
            operandOriginalPositions: [
                [34, 1, 34, 2], [34, 5, 34, 10], [34, 5, 34, 6], [34, 9, 34, 10],
            ],
        },
        {
            text: 'f = a | b',
            operandOriginalPositions: [
                [35, 1, 35, 2], [35, 5, 35, 10], [35, 5, 35, 6], [35, 9, 35, 10],
            ],
        },
        {
            text: 'f = a ^ b',
            operandOriginalPositions: [
                [36, 1, 36, 2], [36, 5, 36, 10], [36, 5, 36, 6], [36, 9, 36, 10],
            ],
        },
        {
            text: 'g = a < b',
            operandOriginalPositions: [
                [38, 5, 38, 6], [38, 9, 38, 14], [38, 9, 38, 10], [38, 13, 38, 14],
            ],
        },
        {
            text: 'g = a <= b',
            operandOriginalPositions: [
                [39, 1, 39, 2], [39, 5, 39, 11], [39, 5, 39, 6], [39, 10, 39, 11],
            ],
        },
        {
            text: 'g = a > b',
            operandOriginalPositions: [
                [40, 1, 40, 2], [40, 5, 40, 10], [40, 5, 40, 6], [40, 9, 40, 10],
            ],
        },
        {
            text: 'g = a >= b',
            operandOriginalPositions: [
                [41, 1, 41, 2], [41, 5, 41, 11], [41, 5, 41, 6], [41, 10, 41, 11],
            ],
        },
        {
            text: 'g = a == b',
            operandOriginalPositions: [
                [42, 1, 42, 2], [42, 5, 42, 11], [42, 5, 42, 6], [42, 10, 42, 11],
            ],
        },
        {
            text: 'g = a != b',
            operandOriginalPositions: [
                [43, 1, 43, 2], [43, 5, 43, 11], [43, 5, 43, 6], [43, 10, 43, 11],
            ],
        },
        {
            text: 'g = a === b',
            operandOriginalPositions: [
                [44, 1, 44, 2], [44, 5, 44, 12], [44, 5, 44, 6], [44, 11, 44, 12],
            ],
        },
        {
            text: 'g = a !== b',
            operandOriginalPositions: [
                [45, 1, 45, 2], [45, 5, 45, 12], [45, 5, 45, 6], [45, 11, 45, 12],
            ],
        },
        {
            text: 'h = true',
            operandOriginalPositions: [
                [47, 5, 47, 6], [47, 9, 47, 13],
            ],
        },
        {
            text: 'h = g && h',
            operandOriginalPositions: [
                [48, 1, 48, 2], [48, 5, 48, 11], [48, 5, 48, 6], [48, 1, 48, 2],
            ],
        },
        {
            text: 'h = g || h',
            operandOriginalPositions: [
                [49, 1, 49, 2], [49, 5, 49, 11], [49, 5, 49, 6], [49, 1, 49, 2],
            ],
        },
        {
            text: 'return',
            operandOriginalPositions: [],
        },
    ],
};

export const UnaryExpression_Expect_IR = {
    stmts: [
        {
            text: 'this = this: @expression/UnaryExpressionTest.ts: %dflt',
            operandOriginalPositions: [
                null, null,
            ],
        },
        {
            text: 'a = 1',
            operandOriginalPositions: [
                [16, 7, 16, 8], [16, 11, 16, 12],
            ],
        },
        {
            text: 'b = -a',
            operandOriginalPositions: [
                [17, 7, 17, 8], [17, 11, 17, 13], [17, 12, 17, 13],
            ],
        },
        {
            text: 'c = ~a',
            operandOriginalPositions: [
                [18, 7, 18, 8], [18, 11, 18, 13], [18, 12, 18, 13],
            ],
        },
        {
            text: 'd = !a',
            operandOriginalPositions: [
                [19, 7, 19, 8], [19, 11, 19, 13], [19, 12, 19, 13],
            ],
        },
        {
            text: 'return',
            operandOriginalPositions: [],
        },
    ],

};

export const NewExpression_Expect_IR = {
    stmts: [
        {
            text: 'this = this: @expression/NewExpressionTest.ts: %dflt',
            operandOriginalPositions: [
                null, null,
            ],
        },
        {
            text: 'sz1 = 1',
            operandOriginalPositions: [
                [16, 7, 16, 10], [16, 13, 16, 14],
            ],
        },
        {
            text: '%0 = newarray (any)[sz1]',
            operandOriginalPositions: [
                [17, 14, 17, 28], [17, 14, 17, 28], [17, 24, 17, 27],
            ],
        },
        {
            text: 'arr1 = %0',
            operandOriginalPositions: [
                [17, 7, 17, 11], [17, 14, 17, 28],
            ],
        },
        {
            text: '%1 = newarray (any)[10]',
            operandOriginalPositions: [
                [18, 14, 18, 27], [18, 14, 18, 27], [18, 24, 18, 26],
            ],
        },
        {
            text: 'arr2 = %1',
            operandOriginalPositions: [
                [18, 7, 18, 11], [18, 14, 18, 27],
            ],
        },
        {
            text: '%2 = newarray (number)[3]',
            operandOriginalPositions: [
                [19, 14, 19, 32], [19, 14, 19, 32], [-1, -1, -1, -1],
            ],
        }, {
            text: '%2[0] = 1',
            operandOriginalPositions: [
                [19, 14, 19, 32], [19, 14, 19, 32], [-1, -1, -1, -1], [19, 24, 19, 25],
            ],
        }, {
            text: '%2[1] = 2',
            operandOriginalPositions: [
                [19, 14, 19, 32], [19, 14, 19, 32], [-1, -1, -1, -1], [19, 27, 19, 28],
            ],
        }, {
            text: '%2[2] = 3',
            operandOriginalPositions: [
                [19, 14, 19, 32], [19, 14, 19, 32], [-1, -1, -1, -1], [19, 30, 19, 31],
            ],
        },
        {
            text: 'arr3 = %2',
            operandOriginalPositions: [
                [19, 7, 19, 11], [19, 14, 19, 32],
            ],
        },
        {
            text: '%3 = newarray (any)[0]',
            operandOriginalPositions: [
                [20, 14, 20, 25], [20, 14, 20, 25], [-1, -1, -1, -1],
            ],
        }, {
            text: 'arr4 = %3',
            operandOriginalPositions: [
                [20, 7, 20, 11], [20, 14, 20, 25],
            ],
        },
        {
            text: '%4 = newarray (any)[1]',
            operandOriginalPositions: [
                [21, 12, 21, 28], [21, 12, 21, 28], [-1, -1, -1, -1],
            ],
        },
        {
            text: '%4[0] = \'str\'',
            operandOriginalPositions: [
                [21, 12, 21, 28], [21, 12, 21, 28], [-1, -1, -1, -1], [21, 22, 21, 27],
            ],
        },
        {
            text: 'arr5 = %4',
            operandOriginalPositions: [
                [21, 5, 21, 9], [21, 12, 21, 28],
            ],
        },
        {
            text: '%5 = newarray (any)[unkownValue]',
            operandOriginalPositions: [
                [22, 12, 22, 34], [22, 12, 22, 34], [22, 22, 22, 33],
            ],
        },
        {
            text: 'arr6 = %5',
            operandOriginalPositions: [
                [22, 5, 22, 9], [22, 12, 22, 34],
            ],
        },
        {
            text: '%6 = newarray (number)[2]',
            operandOriginalPositions: [
                [24, 24, 24, 30], [24, 24, 24, 30], [-1, -1, -1, -1],
            ],
        },
        {
            text: '%6[0] = 4',
            operandOriginalPositions: [
                [24, 24, 24, 30], [24, 24, 24, 30], [-1, -1, -1, -1], [24, 25, 24, 26],
            ],
        },
        {
            text: '%6[1] = 5',
            operandOriginalPositions: [
                [24, 24, 24, 30], [24, 24, 24, 30], [-1, -1, -1, -1], [24, 28, 24, 29],
            ],
        },
        {
            text: 'arrFromLiteral = %6',
            operandOriginalPositions: [
                [24, 7, 24, 21], [24, 24, 24, 30],
            ],
        },
        {
            text: 'return',
            operandOriginalPositions: [],
        },
    ],
};

export const LiteralExpression_Expect_IR = {
    stmts: [{
        text: 'this = this: @expression/LiteralExpressionTest.ts: %dflt',
        operandOriginalPositions: [null, null],
    },
        {
            text: '%0 = newarray (number)[3]',
            operandOriginalPositions: [[16, 14, 16, 23], [16, 14, 16, 23], [-1, -1, -1, -1]],
        },
        {
            text: '%0[0] = 1',
            operandOriginalPositions: [[16, 14, 16, 23], [16, 14, 16, 23], [-1, -1, -1, -1], [16, 15, 16, 16]],
        },
        {
            text: '%0[1] = 2',
            operandOriginalPositions: [[16, 14, 16, 23], [16, 14, 16, 23], [-1, -1, -1, -1], [16, 18, 16, 19]],
        },
        {
            text: '%0[2] = 3',
            operandOriginalPositions: [[16, 14, 16, 23], [16, 14, 16, 23], [-1, -1, -1, -1], [16, 21, 16, 22]],
        },
        {
            text: 'arr1 = %0',
            operandOriginalPositions: [[16, 7, 16, 11], [16, 14, 16, 23]],
        },
        {
            text: '%1 = newarray (any)[0]',
            operandOriginalPositions: [[17, 14, 17, 16], [17, 14, 17, 16], [-1, -1, -1, -1]],
        },
        {
            text: 'arr2 = %1',
            operandOriginalPositions: [[17, 7, 17, 11], [17, 14, 17, 16]],
        },
        {
            text: '%2 = newarray (number|string)[3]',
            operandOriginalPositions: [[18, 12, 18, 23], [18, 12, 18, 23], [-1, -1, -1, -1]],
        },
        {
            text: '%2[0] = 1',
            operandOriginalPositions: [[18, 12, 18, 23], [18, 12, 18, 23], [-1, -1, -1, -1], [18, 13, 18, 14]],
        },
        {
            text: '%2[1] = 2',
            operandOriginalPositions: [[18, 12, 18, 23], [18, 12, 18, 23], [-1, -1, -1, -1], [18, 16, 18, 17]],
        },
        {
            text: '%2[2] = \'3\'',
            operandOriginalPositions: [[18, 12, 18, 23], [18, 12, 18, 23], [-1, -1, -1, -1], [18, 19, 18, 22]],
        },
        {
            text: 'arr3 = %2',
            operandOriginalPositions: [[18, 5, 18, 9], [18, 12, 18, 23]],
        },
        {
            text: 'noSubstitutionTemplateLiteral = \'no substitution template literal\'',
            operandOriginalPositions: [[21, 7, 21, 36], [21, 39, 21, 73]],
        },
        {
            text: 'placeholder1 = 1',
            operandOriginalPositions: [[22, 7, 22, 19], [22, 30, 22, 31]],
        },
        {
            text: '%3 = instanceinvoke placeholder1.<@%unk/%unk: .toString()>()',
            operandOriginalPositions: [[23, 40, 23, 52], [23, 40, 23, 52], [23, 40, 23, 52]],
        },
        {
            text: '%4 = \'raw text1, \' + %3',
            operandOriginalPositions: [[23, 26, 23, 52], [23, 26, 23, 52], [23, 26, 23, 40], [23, 40, 23, 52]],
        },
        {
            text: 'templateLiteral1 = %4',
            operandOriginalPositions: [[23, 7, 23, 23], [23, 26, 23, 52]],
        },
        {
            text: 'placeholder2 = \'placeholder2\'',
            operandOriginalPositions: [[24, 7, 24, 19], [24, 30, 24, 44]],
        },
        {
            text: '%5 = instanceinvoke placeholder2.<@%unk/%unk: .toString()>()',
            operandOriginalPositions: [[25, 29, 25, 41], [25, 29, 25, 41], [25, 29, 25, 41]],
        },
        {
            text: '%6 = %5 + \', raw text2\'',
            operandOriginalPositions: [[25, 29, 25, 54], [25, 29, 25, 54], [25, 29, 25, 41], [25, 41, 25, 54]],
        },
        {
            text: 'templateLiteral2 = %6',
            operandOriginalPositions: [[25, 7, 25, 23], [25, 29, 25, 54]],
        },
        {
            text: 'placeholder3 = 3',
            operandOriginalPositions: [[32, 7, 32, 19], [32, 30, 32, 31]],
        },
        {
            text: 'placeholder4 = \'placeholder4\'',
            operandOriginalPositions: [[33, 7, 33, 19], [33, 30, 33, 44]],
        },
        {
            text: '%7 = newarray (string)[3]',
            operandOriginalPositions: [[-1, -1, -1, -1], [-1, -1, -1, -1], [-1, -1, -1, -1]],
        },
        {
            text: '%7[0] = \'raw text3, \'',
            operandOriginalPositions: [[-1, -1, -1, -1], [-1, -1, -1, -1], [-1, -1, -1, -1], [34, 21, 34, 35]],
        },
        {
            text: '%7[1] = \' raw text4 \'',
            operandOriginalPositions: [[-1, -1, -1, -1], [-1, -1, -1, -1], [-1, -1, -1, -1], [34, 47, 34, 61]],
        },
        {
            text: '%7[2] = \'.\'',
            operandOriginalPositions: [[-1, -1, -1, -1], [-1, -1, -1, -1], [-1, -1, -1, -1], [34, 73, 34, 76]],
        },
        {
            text: '%8 = newarray (any)[2]',
            operandOriginalPositions: [[-1, -1, -1, -1], [-1, -1, -1, -1], [-1, -1, -1, -1]],
        },
        {
            text: '%8[0] = placeholder3',
            operandOriginalPositions: [[-1, -1, -1, -1], [-1, -1, -1, -1], [-1, -1, -1, -1], [34, 35, 34, 47]],
        },
        {
            text: '%8[1] = placeholder4',
            operandOriginalPositions: [[-1, -1, -1, -1], [-1, -1, -1, -1], [-1, -1, -1, -1], [34, 61, 34, 73]],
        },
        {
            text: 'output = staticinvoke <@expression/LiteralExpressionTest.ts: %dflt.myTag(any, any[])>(%7, %8)',
            operandOriginalPositions: [[34, 7, 34, 13], [34, 16, 34, 76], [-1, -1, -1, -1], [-1, -1, -1, -1]],
        },
        {
            text: 'return',
            operandOriginalPositions: [],
        },
    ],
};

export const Operator_Expect_IR = {
    stmts: [{
        text: 'this = this: @expression/OperatorTest.ts: %dflt',
        operandOriginalPositions: [null, null],
    },
        {
            text: '%0 = new @%unk/%unk: Point',
            operandOriginalPositions: [[17, 9, 17, 20], [17, 9, 17, 20]],
        },
        {
            text: 'instanceinvoke %0.<@%unk/%unk: Point.constructor()>()',
            operandOriginalPositions: [[17, 9, 17, 20], [17, 9, 17, 20]],
        },
        {
            text: 'p = %0',
            operandOriginalPositions: [[17, 5, 17, 6], [17, 9, 17, 20]],
        },
        {
            text: '%1 = delete p.<@%unk/%unk: Point.y>',
            operandOriginalPositions: [[18, 1, 18, 11], [18, 1, 18, 11], [18, 8, 18, 11], [18, 8, 18, 9]],
        },
        {
            text: 'a = 0',
            operandOriginalPositions: [[21, 5, 21, 6], [21, 9, 21, 10]],
        },
        {
            text: '%2 = a',
            operandOriginalPositions: [[22, 6, 22, 7], [22, 6, 22, 7]],
        },
        {
            text: 'b = 1',
            operandOriginalPositions: [[23, 5, 23, 6], [23, 9, 23, 10]],
        },
        {
            text: '%3 = a + b',
            operandOriginalPositions: [[24, 7, 24, 12], [24, 7, 24, 12], [24, 7, 24, 8], [24, 11, 24, 12]],
        },
        {
            text: '%4 = await 10',
            operandOriginalPositions: [[27, 1, 27, 9], [27, 1, 27, 9], [27, 7, 27, 9]],
        },
        {
            text: 'x = await 11',
            operandOriginalPositions: [[28, 7, 28, 8], [28, 11, 28, 19], [28, 17, 28, 19]],
        },
        {
            text: '%5 = yield 20',
            operandOriginalPositions: [[31, 1, 31, 9], [31, 1, 31, 9], [31, 7, 31, 9]],
        },
        {
            text: 'isCat = cat instanceof Cat',
            operandOriginalPositions: [[34, 5, 34, 10], [34, 13, 34, 31], [34, 13, 34, 16]],
        },
        {
            text: 'foo1 = <Foo>bar',
            operandOriginalPositions: [[37, 5, 37, 9], [37, 12, 37, 22], [37, 12, 37, 15]],
        },
        {
            text: 'foo2 = <Foo>bar',
            operandOriginalPositions: [[38, 5, 38, 9], [38, 12, 38, 20], [38, 17, 38, 20]],
        },
        {
            text: 'return',
            operandOriginalPositions: [],
        },
    ],
};

export const CallExpression_Expect_IR = {
    stmts: [
        {
            text: 'this = this: @expression/CallExpressionTest.ts: %dflt',
            operandOriginalPositions: [
                null, null,
            ],
        },
        {
            text: 'instanceinvoke a1.<@%unk/%unk: .call1()>()',
            operandOriginalPositions: [
                [17, 1, 17, 11], [17, 1, 17, 3],
            ],
        },
        {
            text: 'a2 = instanceinvoke a3.<@%unk/%unk: .call2()>()',
            operandOriginalPositions: [
                [18, 5, 18, 7], [18, 10, 18, 20], [18, 10, 18, 12],
            ],
        },
        {
            text: 'a4 = instanceinvoke a3.<@%unk/%unk: .call3()>(para1, para2)',
            operandOriginalPositions: [
                [19, 5, 19, 7], [19, 10, 19, 32], [19, 10, 19, 12], [19, 19, 19, 24], [19, 26, 19, 31],
            ],
        },
        {
            text: 'staticinvoke <@%unk/%unk: .call10()>()',
            operandOriginalPositions: [
                [22, 1, 22, 9],
            ],
        },
        {
            text: 'a20 = staticinvoke <@%unk/%unk: .call20()>()',
            operandOriginalPositions: [
                [23, 5, 23, 8], [23, 11, 23, 19],
            ],
        },
        {
            text: 'a40 = staticinvoke <@%unk/%unk: .call30()>(para10, para20)',
            operandOriginalPositions: [
                [24, 5, 24, 8], [24, 11, 24, 33], [24, 18, 24, 24], [24, 26, 24, 32],
            ],
        },
        {
            text: '%0 = staticinvoke <@%unk/%unk: .call200()>()',
            operandOriginalPositions: [
                [27, 12, 27, 21], [27, 12, 27, 21],
            ],
        },
        {
            text: 'a200 = instanceinvoke %0.<@%unk/%unk: .call300()>(para100, para200)',
            operandOriginalPositions: [
                [27, 5, 27, 9], [27, 12, 27, 47], [27, 12, 27, 21], [27, 30, 27, 37], [27, 39, 27, 46],
            ],
        },
        {
            text: 'return',
            operandOriginalPositions: [],
        },
    ],
};

export const ExpressionStatements_Expect_IR = {
    stmts: [
        {
            text: 'this = this: @expression/ExpressionStatementsTest.ts: %dflt',
            operandOriginalPositions: [null, null],
        },
        {
            text: 'i = 1',
            operandOriginalPositions: [[17, 5, 17, 6], [17, 9, 17, 10]],
        },
        {
            text: '%0 = i',
            operandOriginalPositions: [[18, 1, 18, 2], [18, 1, 18, 2]],
        },
        {
            text: '%1 = new @expression/ExpressionStatementsTest.ts: Foo',
            operandOriginalPositions: [[32, 11, 32, 20], [32, 11, 32, 20]],
        },
        {
            text: 'instanceinvoke %1.<@expression/ExpressionStatementsTest.ts: Foo.constructor()>()',
            operandOriginalPositions: [[32, 11, 32, 20], [32, 11, 32, 20]],
        },
        {
            text: 'foo = %1',
            operandOriginalPositions: [[32, 5, 32, 8], [32, 11, 32, 20]],
        },
        {
            text: '%2 = foo.<@expression/ExpressionStatementsTest.ts: Foo.b>',
            operandOriginalPositions: [[33, 1, 33, 6], [33, 1, 33, 6], [33, 1, 33, 4]],
        },
        {
            text: 'instanceinvoke foo.<@expression/ExpressionStatementsTest.ts: Foo.bar()>()',
            operandOriginalPositions: [[34, 1, 34, 10], [34, 1, 34, 4]],
        },
        {
            text: '%3 = 1',
            operandOriginalPositions: [[37, 1, 37, 2], [37, 1, 37, 2]],
        },
        {
            text: '%4 = \'str\'',
            operandOriginalPositions: [[38, 1, 38, 6], [38, 1, 38, 6]],
        },
        {
            text: '%5 = \'no substitution template literal\'',
            operandOriginalPositions: [[39, 1, 39, 35], [39, 1, 39, 35]],
        },
        {
            text: '%6 = false',
            operandOriginalPositions: [[40, 1, 40, 6], [40, 1, 40, 6]],
        },
        {
            text: '%7 = /"/',
            operandOriginalPositions: [[41, 1, 41, 4], [41, 1, 41, 4]],
        },
        {
            text: 'i1 = 1',
            operandOriginalPositions: [[44, 5, 44, 7], [44, 10, 44, 11]],
        },
        {
            text: '%8 = i1 + 2',
            operandOriginalPositions: [[45, 1, 45, 7], [45, 1, 45, 7], [45, 1, 45, 3], [45, 6, 45, 7]],
        },
        {
            text: '%9 = 3',
            operandOriginalPositions: [[46, 6, 46, 7], [46, 6, 46, 7]],
        },
        {
            text: '%10 = 3',
            operandOriginalPositions: [[47, 6, 47, 7], [47, 6, 47, 7]],
        },
        {
            text: '%11 = undefined === \'3\'',
            operandOriginalPositions: [[47, 1, 47, 15], [47, 1, 47, 15], [-1, -1, -1, -1], [47, 12, 47, 15]],
        },
        {
            text: '%12 = new @expression/ExpressionStatementsTest.ts: Foo',
            operandOriginalPositions: [[48, 1, 48, 10], [48, 1, 48, 10]],
        },
        {
            text: 'instanceinvoke %12.<@expression/ExpressionStatementsTest.ts: Foo.constructor()>()',
            operandOriginalPositions: [[48, 1, 48, 10], [48, 1, 48, 10]],
        },
        {
            text: 'return',
            operandOriginalPositions: [],
        },
    ],
};