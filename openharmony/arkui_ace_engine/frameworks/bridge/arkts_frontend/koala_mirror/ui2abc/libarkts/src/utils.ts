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

export function throwError(error: string): never {
    throw new Error(error)
}

export function withWarning<T>(value: T, message: string): T {
    // console.warn(message)
    return value
}

export function isNumber(value: any): value is number {
    return typeof value === `number`
}

function replacePercentOutsideStrings(code: string): string  {
    const stringPattern = /("[^"]*"|'[^']*'|`[^`]*`)/g;
    const percentPattern = /(?<!["'`])(%)(?![\d\s])/g;
    const strings = code.match(stringPattern) || [];

    let placeholderCounter = 0;
    const placeholderMap = new Map<string, string>();
    strings.forEach((string) => {
        const placeholder = `__STRING_PLACEHOLDER_${placeholderCounter++}__`;
        placeholderMap.set(placeholder, string);
        code = code.replace(string, placeholder);
    });

    code = code.replace(percentPattern, '_');

    placeholderMap.forEach((originalString, placeholder) => {
        code = code.replace(new RegExp(placeholder, 'g'), originalString);
    });

    return code;
}

function replaceIllegalHashes(code: string): string {
    const stringPattern = /("[^"]*"|'[^']*'|`[^`]*`)/g;
    const strings = code.match(stringPattern) || [];

    let placeholderCounter = 0;
    const placeholderMap = new Map<string, string>();
    strings.forEach((string) => {
        const placeholder = `__STRING_PLACEHOLDER_${placeholderCounter++}__`;
        placeholderMap.set(placeholder, string);
        code = code.replace(string, placeholder);
    });

    code = code.replace(/#/g, '_');

    placeholderMap.forEach((originalString, placeholder) => {
        code = code.replace(new RegExp(placeholder, 'g'), originalString);
    });

    return code;
}

function replaceGensymWrappers(code: string): string {
    const indices = [...code.matchAll(/\({let/g)].map(it => it.index)
    const replacements: string[][] = []
    for (var i of indices) {
        if (!i) {
            continue
        }
        var j = i + 1, depth = 1
        while (j < code.length) {
            if (code[j] == '(') {
                depth++
            }
            if (code[j] == ')') {
                depth--
            }
            if (depth == 0) {
                break
            }
            j++
        }

        if (j == code.length) {
            continue
        }

        const base = code.substring(i, j + 1)
        if (base.match(/\({let/)?.length! > 1) { // don't touch if contains nested constructions
            continue
        }
        const fixed = base.replaceAll(/^\({let ([_%a-zA-Z0-9]+?) = (?!\({let)([\s\S]*?);\n([\s\S]*?)}\)$/g,
            (match, name: string, val: string, expr: string) => {
                let rightExpr = expr.slice(expr.lastIndexOf(name) + name.length, -1)
                if (rightExpr[0] != '.') {
                    rightExpr = `.${rightExpr}`
                }
                return `(${val}?${rightExpr})`
            }
        )
        replacements.push([base, fixed])
    }
    for (var [b, f] of replacements) {
        code = code.replace(b, f)
    }
    return code
}


function addExports(code: string): string {
    const exportAstNodes = ["  enum", "let", "const", "class", "abstract class", "@Entry() @Component() final class", "@Component() final class", "interface", "@interface", "type", "enum", "final class", "function",
        "declare interface", "@memo_stable() declare interface",  "@memo_stable() interface",
        "@Retention({policy:\"SOURCE\"}) @interface", "@memo() function", "@memo_entry() function", "@memo_intrinsic() function",
    ]
    exportAstNodes.forEach((astNodeText) => {
            code = code.replaceAll(`\n${astNodeText}`, `\nexport ${astNodeText}`)
        }
    )
    // TODO this is a temporary workaround and should be replaced with a proper import/export handling in future
    code = code.replaceAll(/\n(@memo\(\) @BuilderLambda\(\{value:"\w+"\}\)) function/g, '\n$1 export function')
    const fix = [
        ["export @memo() function", "@memo() export function"],
        ["export @memo_entry() function", "@memo_entry() export function"],
        ["export @memo_intrinsic() function", "@memo_intrinsic() export function"],
        ["export @memo_stable()", "@memo_stable() export"],
        ["export class OhosRouter", "export default class OhosRouter"]
    ]
    for (var [f, t] of fix) {
        code = code.replaceAll(f, t)
    }
    return code.replaceAll("\nexport function main()", "\nfunction main()")
}

function removeAbstractFromInterfaces(code: string): string {
    const interfaces = [...code.matchAll(/interface([\s\S]*?){([\s\S]*?)\n}\n/g)].map(it => it[0])
    interfaces.forEach((content) => {
        const newContent = content.replaceAll('abstract ', '')
        code = code.replaceAll(content, newContent)
    })
    return code
}

function removeInvalidLambdaTyping(code: string): string {
    const knownTypes = ['boolean', 'int64', 'void']
    return code.replaceAll(/\(([^\n\(\)]*?)\): ([\S]*?) => {/g, (match, p1, p2) => {
        if (knownTypes.includes(p2)) {
            return match
        }
        return `(${p1}) => {`
    })
}

function returnOptionalParams(code: string): string {
    const reduce = (line: string): string => {
        if (line.includes("constructor")) {
            return line
        }
        for (var i = 0; i < line.length; i++) {
            if (line[i] == '(') {
                let ignore = false
                for (var k = i; k >= 0; k--) {
                    if (line[k].match(/[a-zA-Z<>_0-9]/)) {
                        break
                    }
                    if (line[k] == ':') {
                        ignore = true
                    }
                }
                if (ignore) {
                    continue
                }
                const initi = i
                let j = i + 1, depth = 1
                let parts: string[] = []
                while (j < line.length) {
                    if (line[j] == '(') {
                        depth++
                    }
                    if (line[j] == ')') {
                        depth--
                        if (depth == 0) {
                            parts.push(line.substring(i + 1, j))
                            break
                        }
                    }
                    if (line[j] == ',' && depth == 1) {
                        parts.push(line.substring(i + 1, j))
                        i = j + 1
                    }
                    j++
                }
                if (depth == 0 && parts.length && parts.every(it => it.includes(": ") && !it.includes("? "))) {
                    let k = parts.length - 1
                    while (k >= 0) {
                        if (parts[k].endsWith(" | undefined")) {
                            let w = parts[k].substring(0, parts[k].length - " | undefined".length)
                            let i = w.indexOf(':')
                            if (w[i - 1] != '?') {
                                parts[k] = w.substring(0, i) + "?" + w.substring(i)
                            }
                        } else {
                            break
                        }
                        k--
                    }
                    if (k != parts.length - 1) {
                        let nline = line.substring(0, initi + 1) + parts.join(', ') + line.substring(j)
                        return nline
                    }
                }
                i = initi
            }
        }
        return line
    }
    return code.split('\n').map((line) => {
        let nline = reduce(line)
        while (nline != line) {
            line = nline
            nline = reduce(line)
        }
        return line
    }).join('\n')
}

function fixPropertyLines(code: string): string {
    /*

    for some properties the following construction is generated:

    private readonly <property>name = false;

    public name(<property>name) { // for readonly properties "setter" is also generated
        (this).<property>name = <property>name = false; // sometimes there is a typing here
        return;
    }

    public name() {
        return (this).<property>name;
    }

    this function wraps this back to `readonly name = false;`

    */
    return code.replaceAll(/(private|public)(.*)?<property>(.*?)\n((.*?)\n){9}/g, (match, p1, p2, p3) => {
        return `public ${p2} ${p3}`
    })
}

function fixDuplicateSettersInInterfaces(code: string): string {
    /*

    sometimes interfaces contains duplicate setters, this functions fixes it

    */
    code = code.replaceAll(/\n[ ]*(.*)interface(.*){\n([\s\S]*?)\n[ ]*}\n/g, (match, modifiers, p1, p2: string) => {
        const keep = p2.split('\n').filter((it) => !it.trimStart().startsWith(`set`))
        const setters = [...new Set(p2.split('\n').filter((it) => it.trimStart().startsWith(`set`)))]
        return `\n${modifiers}interface${p1}{\n${keep.join('\n')}\n${setters.join('\n')}\n}\n`
    })
    return code
}

function excludePartialInterfaces(code: string): string {
    return code
    .replaceAll(/export interface (.*)\$partial<>([\s\S]*?)}/g, '')
    .replaceAll(/interface (.*)\$partial<>([\s\S]*?)}/g, '')
}

function fixNamespace(code: string) {
    /*
    namespaces become abstract classes, and enums become final classes
    enum in namespace -> class in class -> not supported

    we have only one such place, so fix manually
    */
    code = code.replaceAll(/export (declare )?abstract class (Profiler|GestureControl|text|common2D) {/g, `export $1 namespace $2 {`)
    code = code.replaceAll(`public static _$init$_() {}`, ``)
    code = code.replaceAll(`public static _$initializerBlockInit$_() {}`, ``)
    code = code.replaceAll(/public static ((?:un)?registerVsyncCallback)/g, "export function $1")
    code = code.replaceAll(/public static (setCursor)/g, "export function $1")
    code = code.replaceAll(/public static (restoreDefault)/g, "export function $1")
    code = code.replaceAll(/public static (requestFocus\(value)/g, "export function $1")

    return code
}

function fixEnums(code: string) {
    const lines = code.split('\n')
    const enums = []
    for (let i = 0; i + 1 < lines.length; i++) {
        if (lines[i].trimStart().startsWith(`export final class`)
            && lines[i + 1].trimStart().startsWith(`private readonly _ordinal`)
        ) {
            const name = lines[i].split(' ')[3]
            enums.push(name)
        }
    }
    enums.forEach((name) => {
        const regexp = new RegExp(`${name}\\.(\\w+)(.)`, `g`)
        code = code.replaceAll(regexp, (match, p1, p2) => {
            if (!p1.startsWith('_') && p2 == ":") { // this colon is for switch case, not for type
                return `${name}.${p1}.valueOf()${p2}`
            }
            return match
        })
        const idents = [...code.matchAll(new RegExp(`(\\w+?)([\\W])(\\w+?): ${name}`, `g`))].filter(it => it[1] != "readonly" && it[1] != "_get").map(it => it[3])
        // work manually with a couple of cases not to write one more bracket parser
        if (code.includes(`const eventKind = (deserializer.readInt32() as CallbackEventKind);`)) {
            // this is for file arkui/src/generated/peers/CallbacksChecker.ts
            idents.push(`eventKind`)
            code = code.replace(`const eventKind = (deserializer.readInt32() as CallbackEventKind);`, `const eventKind = CallbackEventKind.fromValue(deserializer.readInt32());`)
        }
        if (code.includes(`switch ((type as EventType))`)) {
            // this is for file arkui/src/Application.ts
            code = code.replace(`switch ((type as EventType))`, `switch (type)`)
        }
        idents.forEach((id) => {
            code = code.replaceAll(`${id} as int32`, `${id}.valueOf()`)
            code = code.replaceAll(`switch (${id})`, `switch (${id}.valueOf())`)
        })
    })
    return code
}

/*
    TODO:
     The lowerings insert %% and other special symbols into names of temporary variables.
     Until we keep feeding ast dumps back to the parser this function is needed.
 */
export function filterSource(text: string): string {
    //console.error("====")
    // console.error(text.split('\n').map((it, index) => `${`${index + 1}`.padStart(4)} |${it}`).join('\n'))
    const dumperUnwrappers = [
        addExports,
        fixNamespace,
        fixEnums,
        fixDuplicateSettersInInterfaces,
        removeAbstractFromInterfaces,
        replaceGensymWrappers, // nested
        replaceGensymWrappers, // nested
        replaceGensymWrappers,
        replaceIllegalHashes,
        replacePercentOutsideStrings,
        excludePartialInterfaces,
        (code: string) => code.replaceAll("<cctor>", "_cctor_"),
        (code: string) => code.replaceAll("public constructor() {}", ""),
        (code: string) => code.replaceAll("@Module()", ""),
        (code: string) => code.replaceAll("export * as  from", "export * from"),
        fixPropertyLines
    ]
    // console.error("====")
    // console.error(dumperUnwrappers.reduceRight((code, f) => f(code), text).split('\n').map((it, index) => `${`${index + 1}`.padStart(4)} |${it}`).join('\n'))
    return dumperUnwrappers.reduceRight((code, f) => f(code), text)
}
