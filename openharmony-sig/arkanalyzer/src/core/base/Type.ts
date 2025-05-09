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

import { ClassSignature, LocalSignature, MethodSignature, NamespaceSignature } from '../model/ArkSignature';
import { ArkExport, ExportType } from '../model/ArkExport';
import { MODIFIER_TYPE_MASK, ModifierType } from '../model/ArkBaseModel';
import {
    ANY_KEYWORD,
    BOOLEAN_KEYWORD,
    NEVER_KEYWORD,
    NULL_KEYWORD,
    NUMBER_KEYWORD,
    STRING_KEYWORD,
    UNDEFINED_KEYWORD,
    UNKNOWN_KEYWORD,
    VOID_KEYWORD,
} from '../common/TSConst';
import { Local } from './Local';

/**
 * @category core/base/type
 */
export abstract class Type {
    toString(): string {
        return this.getTypeString();
    }

    abstract getTypeString(): string;
}

/**
 * any type
 * @category core/base/type
 */
export class AnyType extends Type {
    private static readonly INSTANCE = new AnyType();

    public static getInstance(): AnyType {
        return this.INSTANCE;
    }

    private constructor() {
        super();
    }

    public getTypeString(): string {
        return ANY_KEYWORD;
    }
}

/**
 * unknown type
 * @category core/base/type
 */
export class UnknownType extends Type {
    private static readonly INSTANCE = new UnknownType();

    public static getInstance(): UnknownType {
        return this.INSTANCE;
    }

    private constructor() {
        super();
    }

    public getTypeString(): string {
        return UNKNOWN_KEYWORD;
    }
}

/**
 * unclear type
 * @category core/base/type
 */
export class UnclearReferenceType extends Type {
    private name: string;
    private genericTypes: Type[];

    constructor(name: string, genericTypes: Type[] = []) {
        super();
        this.name = name;
        this.genericTypes = genericTypes;
    }

    public getName() {
        return this.name;
    }

    public getGenericTypes(): Type[] {
        return this.genericTypes;
    }

    public getTypeString(): string {
        let str = this.name;
        if (this.genericTypes.length > 0) {
            str += '<' + this.genericTypes.join(',') + '>';
        }
        return str;
    }
}

/**
 * primitive type
 * @category core/base/type
 */
export abstract class PrimitiveType extends Type {
    private name: string;

    constructor(name: string) {
        super();
        this.name = name;
    }

    public getName() {
        return this.name;
    }

    public getTypeString(): string {
        return this.name;
    }
}

export class BooleanType extends PrimitiveType {
    private static readonly INSTANCE = new BooleanType();

    private constructor() {
        super(BOOLEAN_KEYWORD);
    }

    public static getInstance() {
        return this.INSTANCE;
    }
}

export class NumberType extends PrimitiveType {
    private static readonly INSTANCE = new NumberType();

    private constructor() {
        super(NUMBER_KEYWORD);
    }

    public static getInstance() {
        return this.INSTANCE;
    }
}

export class StringType extends PrimitiveType {
    private static readonly INSTANCE = new StringType();

    private constructor() {
        super(STRING_KEYWORD);
    }

    public static getInstance() {
        return this.INSTANCE;
    }
}

/**
 * null type
 * @category core/base/type
 */
export class NullType extends PrimitiveType {
    private static readonly INSTANCE = new NullType();

    public static getInstance(): NullType {
        return this.INSTANCE;
    }

    private constructor() {
        super(NULL_KEYWORD);
    }
}

/**
 * undefined type
 * @category core/base/type
 */
export class UndefinedType extends PrimitiveType {
    private static readonly INSTANCE = new UndefinedType();

    public static getInstance(): UndefinedType {
        return this.INSTANCE;
    }

    private constructor() {
        super(UNDEFINED_KEYWORD);
    }
}

/**
 * literal type
 * @category core/base/type
 */
export class LiteralType extends PrimitiveType {
    public static readonly TRUE = new LiteralType(true);
    public static readonly FALSE = new LiteralType(false);

    private literalName: string | number | boolean;

    constructor(literalName: string | number | boolean) {
        super('literal');
        this.literalName = literalName;
    }

    public getLiteralName(): string | number | boolean {
        return this.literalName;
    }

    public getTypeString():string {
        return this.literalName.toString();
    }
}

/**
 * union type
 * @category core/base/type
 */
export class UnionType extends Type {
    private types: Type[];
    private currType: Type;  // The true type of the value at this time  
    constructor(types: Type[], currType: Type = UnknownType.getInstance()) {
        super();
        this.types = [...types];
        this.currType = currType;
    }

    public getTypes(): Type[] {
        return this.types;
    }

    public getCurrType(): Type {
        return this.currType;
    }

    public setCurrType(newType: Type): void {
        this.currType = newType;
    }

    public getTypeString(): string {
        return this.types.join('|');
    }

    public flatType(): Type[] {
        const result: Type[] = [];
        this.types.forEach(t => {
            if (t instanceof UnionType) {
                t.flatType().forEach(e => result.push(e));
            } else {
                result.push(t);
            }
        });
        return result;
    }
}

/**
 * types for function void return type
 * @category core/base/type
 */
export class VoidType extends Type {
    private static readonly INSTANCE = new VoidType();

    public static getInstance(): VoidType {
        return this.INSTANCE;
    }

    private constructor() {
        super();
    }

    public getTypeString(): string {
        return VOID_KEYWORD;
    }
}

export class NeverType extends Type {
    private static readonly INSTANCE = new NeverType();

    public static getInstance(): NeverType {
        return this.INSTANCE;
    }

    private constructor() {
        super();
    }

    public getTypeString(): string {
        return NEVER_KEYWORD;
    }
}

/**
 * function type
 * @category core/base/type
 */
export class FunctionType extends Type {
    private methodSignature: MethodSignature;
    private realGenericTypes?: Type[];

    constructor(methodSignature: MethodSignature, realGenericTypes?: Type[]) {
        super();
        this.methodSignature = methodSignature;
        this.realGenericTypes = realGenericTypes;
    }

    public getMethodSignature(): MethodSignature {
        return this.methodSignature;
    }

    public getRealGenericTypes(): Type[] | undefined {
        return this.realGenericTypes;
    }

    public getTypeString(): string {
        return this.methodSignature.toString();
    }
}

/**
 * types for closures which is a special FunctionType with a lexical env
 * @category core/base/type
 */
export class ClosureType extends FunctionType {
    private lexicalEnv: LexicalEnvType;

    constructor(lexicalEnv: LexicalEnvType, methodSignature: MethodSignature, realGenericTypes?: Type[]) {
        super(methodSignature, realGenericTypes);
        this.lexicalEnv = lexicalEnv;
    }

    public getLexicalEnv(): LexicalEnvType {
        return this.lexicalEnv;
    }

    public getTypeString(): string {
        return 'closures: ' + super.getTypeString();
    }
}

/**
 * type of an object
 * @category core/base/type
 */
export class ClassType extends Type {
    private classSignature: ClassSignature;
    private realGenericTypes?: Type[];

    constructor(classSignature: ClassSignature, realGenericTypes?: Type[]) {
        super();
        this.classSignature = classSignature;
        this.realGenericTypes = realGenericTypes;
    }

    public getClassSignature(): ClassSignature {
        return this.classSignature;
    }

    public setClassSignature(newClassSignature: ClassSignature): void {
        this.classSignature = newClassSignature;
    }

    public getRealGenericTypes(): Type[] | undefined {
        return this.realGenericTypes;
    }

    public setRealGenericTypes(types: Type[] | undefined): void {
        this.realGenericTypes = types;
    }

    public getTypeString(): string {
        let temp = this.classSignature.toString();
        let generic = this.realGenericTypes?.join(',');
        if (generic) {
            temp += `<${generic}>`;
        }
        return temp;
    }
}

export class ArrayType extends Type {
    private baseType: Type;
    private dimension: number;

    constructor(baseType: Type, dimension: number) {
        super();
        this.baseType = baseType;
        this.dimension = dimension;
    }

    /**
     * Returns the base type of this array, such as `Any`, `Unknown`, `TypeParameter`, etc.
     * @returns The base type of array.
     */
    public getBaseType(): Type {
        return this.baseType;
    }

    public setBaseType(newType: Type): void {
        this.baseType = newType;
    }

    public getDimension(): number {
        return this.dimension;
    }

    public getTypeString(): string {
        const strs: string[] = [];
        if (this.baseType instanceof UnionType) {
            strs.push('(' + this.baseType.toString() + ')');
        } else if (this.baseType) {
            strs.push(this.baseType.toString());
        }
        for (let i = 0; i < this.dimension; i++) {
            strs.push('[]');
        }
        return strs.join('');
    }
}

export class TupleType extends Type {
    private types: Type[];

    constructor(types: Type[]) {
        super();
        this.types = types;
    }

    public getTypes(): Type[] {
        return this.types;
    }

    public getTypeString(): string {
        return '[' + this.types.join(', ') + ']';
    }
}

export class AliasType extends Type implements ArkExport {
    private originalType: Type;
    private name: string;
    private signature: LocalSignature;
    protected modifiers?: number;

    constructor(name: string, originalType: Type, signature: LocalSignature) {
        super();
        this.name = name;
        this.originalType = originalType;
        this.signature = signature;
    }

    public getName(): string {
        return this.name;
    }

    public setOriginalType(type: Type): void {
        this.originalType = type;
    }

    public getOriginalType(): Type {
        return this.originalType;
    }

    public getTypeString(): string {
        return this.getSignature().toString();
    }

    public getExportType(): ExportType {
        return ExportType.TYPE;
    }

    public getModifiers(): number {
        if (!this.modifiers) {
            return 0;
        }
        return this.modifiers;
    }

    public containsModifier(modifierType: ModifierType): boolean {
        if (!this.modifiers) {
            return false;
        }

        return (this.modifiers & modifierType) === modifierType;
    }

    public setModifiers(modifiers: number): void {
        if (modifiers !== 0) {
            this.modifiers = modifiers;
        }
    }

    public addModifier(modifier: ModifierType | number): void {
        this.modifiers = this.getModifiers() | modifier;
    }

    public removeModifier(modifier: ModifierType): void {
        if (!this.modifiers) {
            return;
        }
        this.modifiers &= MODIFIER_TYPE_MASK ^ modifier;
    }

    public getSignature(): LocalSignature {
        return this.signature;
    }
}

export class GenericType extends Type {
    private name: string;
    private defaultType?: Type;
    private constraint?: Type;
    private index?: number;

    constructor(name: string, defaultType?: Type, constraint?: Type) {
        super();
        this.name = name;
        this.defaultType = defaultType;
        this.constraint = constraint;
    }

    public getName(): string {
        return this.name;
    }

    public getDefaultType(): Type | undefined {
        return this.defaultType;
    }

    public setDefaultType(type: Type): void {
        this.defaultType = type;
    }

    public getConstraint(): Type | undefined {
        return this.constraint;
    }

    public setConstraint(type: Type): void {
        this.constraint = type;
    }

    public setIndex(index: number) {
        this.index = index;
    }

    public getIndex(): number {
        return this.index ?? 0;
    }

    public getTypeString(): string {
        let str = this.name;
        if (this.constraint) {
            str += ' extends ' + this.constraint.toString();
        }
        if (this.defaultType) {
            str += ' = ' + this.defaultType.toString();
        }
        return str;
    }
}

export abstract class AnnotationType extends Type {
    private originType: string;

    protected constructor(originType: string) {
        super();
        this.originType = originType;
    }

    public getOriginType(): string {
        return this.originType;
    }

    public getTypeString(): string {
        return this.originType;
    }
}

export class AnnotationNamespaceType extends AnnotationType {
    private namespaceSignature: NamespaceSignature = NamespaceSignature.DEFAULT;

    public static getInstance(signature: NamespaceSignature): AnnotationNamespaceType {
        const type = new AnnotationNamespaceType(signature.getNamespaceName());
        type.setNamespaceSignature(signature);
        return type;
    }

    public getNamespaceSignature(): NamespaceSignature {
        return this.namespaceSignature;
    }

    public setNamespaceSignature(signature: NamespaceSignature): void {
        this.namespaceSignature = signature;
    }

    constructor(originType: string) {
        super(originType);
    }

    public getOriginType(): string {
        return super.getOriginType();
    }
}

export class AnnotationTypeQueryType extends AnnotationType {
    constructor(originType: string) {
        super(originType);
    }
}

export class LexicalEnvType extends Type {
    private nestedMethodSignature: MethodSignature;
    private closures: Local[] = [];

    constructor(nestedMethod: MethodSignature, closures?: Local[]) {
        super();
        this.nestedMethodSignature = nestedMethod;
        this.closures = closures ?? this.closures;
    }

    public getNestedMethod(): MethodSignature {
        return this.nestedMethodSignature;
    }

    public getClosures(): Local[] {
        return this.closures;
    }

    public addClosure(closure: Local): void {
        this.closures.push(closure);
    }

    public getTypeString(): string {
        return `[${this.getClosures().join(', ')}]`;
    }
}
