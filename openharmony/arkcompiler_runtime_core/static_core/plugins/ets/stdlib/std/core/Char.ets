/*
 * Copyright (c) 2021-2025 Huawei Device Co., Ltd.
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

package std.core;

export type UTF_16_CodePoint = int;

/**
 * Represents boxed char value and related operations
 */
export final class Char extends Object implements Comparable<Char>, JSONable<Char> {
    private value: char;

    /**
     * constructor() creates a default Char object.
     */
    public constructor() {
        this.value = c'\u0000';
    }

    /**
     * constructor(char) creates a Char object from a specified primitive char.
     *
     * @param value a primitive char to create Char from.
     */
    public constructor(value: char) {
        this.value = value;
    }

    /**
     * constructor(Char) creates a Char object from another Char object.
     *
     * @param value a Char object to copy from.
     */
    public constructor(value: Char) {
        this.value = value.unboxed();
    }

    /**
     * unboxed() returns an underlying primitive char.
     *
     * @returns the underlying primitive char.
     */
    public unboxed(): char {
        return this.value;
    }

    /**
     * valueOf(char) creates a Char object from a primitive char.
     * This method is preferred over {@link <constructor(char)>} since this method can use a cached Char object,
     * i.e. this method might be more efficient.
     *
     * @param value a primitive char to create Char from.
     *
     * @returns an instance of Char created from the primitive char.
     */
    public static valueOf(value: char): Char {
        // TODO(ivan-tyulyandin): caching is possible
        return new Char(value);
    }

    /**
    * equals(Object) compares two Chars by their underlying primitive chars.
    *
    * @param other a reference to object to be compared with.
    *
    * @returns comparison result of underlying primitive chars.
    * Returns false if the argument is not an instance of Char.
    */
    public equals(other: NullishType): boolean {
        if (this === other) {
            return true
        }

        if (!(other instanceof Char)) {
            return false
        }

        return this.value == (other as Char).value
    }

    /**
     * toString() converts Char to String object that contains a single element with the underlying char.
     *
     * @returns the String representation of the Char.
     */
    public override toString(): String {
        // TODO(ivan-tyulyandin): if String(char) will be implemented, rewrite code below
        let buf: FixedArray<char> = new char[1];
        buf[0] = this.value;
        return new String(buf);
    }

    /**
     * compare(Char, Char) compares two Chars by their underlying chars.
     *
     * @param lhs the first Char to compare.
     *
     * @param rhs the second Char to compare.
     *
     * @returns comparison result of the underlying chars.
     */
    public static compare(lhs: Char, rhs: Char): boolean {
        return (lhs.unboxed() == rhs.unboxed());
    }

    /**
     * Compares this instance to other Char object
     * The result is less than 0 if this instance lesser than provided object
     * 0 if they are equal
     * and greater than 0 otherwise.
     *
     * @param other Char object to compare with
     *
     * @returns result of the comparison
     */
    public override compareTo(other: Char): int {
        return (this.value as int) - (other.unboxed() as int);
    }

    /**
     * $_hashCode(char) returns a hashcode of the char.
     *
     * @param value the char to be hash coded.
     *
     * @returns the hash code.
     */
    public static $_hashCode(value: char): int {
        return value as int;
    }

    /**
     * $_hashCode() returns a hashcode of the underlying char.
     *
     * @returns the hash code.
     */
    public override $_hashCode(): int {
        return Char.$_hashCode(this.value);
    }

    public static readonly CHAR_BIT_SIZE = 16;
    public static readonly MAX_CODE_POINT = 0x10FFFF;
    public static readonly HIGH_SURROGATE_MIN: char = c'\uD800';
    public static readonly HIGH_SURROGATE_MAX: char = c'\uDBFF';
    public static readonly LOW_SURROGATE_MIN: char = c'\uDC00';
    public static readonly LOW_SURROGATE_MAX: char = c'\uDFFF';
    private static readonly BMP_BORDER: UTF_16_CodePoint = 0x10000;

    /**
     * `MIN_VALUE` is a smallest value of type `char`
     */
    public static readonly MIN_VALUE: char = c'\u0000';

    /**
     * `MAX_VALUE` is a largest value of type `char`
     */
    public static readonly MAX_VALUE: char = c'\uFFFF';

    /**
     * Returns value of this instance
     *
     * @returns value as byte
     * @tag arkts
     */
    public toByte(): byte {
        return this.value.toByte();
    }

    /**
     * Returns value of this instance
     *
     * @returns value as short
     * @tag arkts
     */
    public toShort(): short {
        return this.value.toShort();
    }

    /**
     * Returns value of this instance
     *
     * @returns value as int
     * @tag arkts
     */
    public toInt(): int {
        return this.value.toInt();
    }

    /**
     * Returns value of this instance
     *
     * @returns value as long
     * @tag arkts
     */
    public toLong(): long {
        return this.value.toLong();
    }

    /**
     * Returns value of this instance
     *
     * @returns value as float
     * @tag arkts
     */
    public toFloat(): float {
        return this.value.toFloat();
    }

    /**
     * Returns value of this instance
     *
     * @returns value as float
     * @tag arkts
     */
    public toDouble(): double {
        return this.value.toDouble();
    }

    /**
     * Returns value of this instance
     *
     * @returns value as char
     * @tag arkts
     */
    public toChar(): char {
        return this.value;
    }

    /**
     * Returns the primitive as byte value
     *
     * @param value value to cast
     *
     * @returns casted value
     */
    public static native toByte(value: char): byte;

    /**
     * Returns the primitive as short value
     *
     * @param value value to cast
     *
     * @returns casted value
     */
    public static native toShort(value: char): short;

    /**
     * Returns the primitive as int value
     *
     * @param value value to cast
     *
     * @returns casted value
     */
    public static native toInt(value: char): int;

    /**
     * Returns the primitive as long value
     *
     * @param value value to cast
     *
     * @returns casted value
     */
    public static native toLong(value: char): long;

    /**
     * Returns the primitive as float value
     *
     * @param value value to cast
     *
     * @returns casted value
     */
    public static native toFloat(value: char): float;

    /**
     * Returns the primitive as float value
     *
     * @param value value to cast
     *
     * @returns casted value
     */
    public static native toDouble(value: char): double;

    /**
     * Returns the primitive as char value
     *
     * @param value value to cast
     *
     * @returns casted value
     */
    public static toChar(value: char): char {
        return value;
    };

    /**
     * isInBasicMultilingualPlane(char) checks if the char is in Basic Multilingual Plane.
     * See UTF-16 for more details.
     *
     * @param value the char to be checked.
     *
     * @returns if the char is in Basic Multilingual Plane.
     */
    public static isInBasicMultilingualPlane(value: char): boolean {
        if (value < Char.BMP_BORDER) {
            return (value < Char.HIGH_SURROGATE_MIN) || (value > Char.LOW_SURROGATE_MAX);
        }
        return false
    }

    /**
     * isInBasicMultilingualPlane(UTF_16_CodePoint) checks if the code point is in Basic Multilingual Plane.
     * See UTF-16 for more details.
     *
     * @param value the code point to be checked.
     *
     * @returns if the code point is in Basic Multilingual Plane.
     */
    public static isInBasicMultilingualPlane(value: UTF_16_CodePoint): boolean {
        if (value < Char.BMP_BORDER) {
            return Char.isInBasicMultilingualPlane(value as char);
        }
        return false;
    }

    /**
     * isInBasicMultilingualPlane() checks if the underlying char is in Basic Multilingual Plane.
     * See UTF-16 for more details.
     *
     * @returns if the char is in Basic Multilingual Plane.
     */
    public isInBasicMultilingualPlane(): boolean {
        return Char.isInBasicMultilingualPlane(this.value);
    }

    /**
     * isValidCodePoint() checks if the code point is correctly encoded.
     * See UTF-16 for more details.
     *
     * @returns true if the code point is not malformed.
     */
    public static isValidCodePoint(codePoint: UTF_16_CodePoint): boolean {
        return (codePoint >= 0 && codePoint <= Char.MAX_CODE_POINT &&
            !(codePoint >= (Char.HIGH_SURROGATE_MIN as int) && codePoint <= (Char.LOW_SURROGATE_MAX as int)));
    }

    /**
     * codeUnitsToEncode(UTF_16_CodePoint) counts a number of code units to encode the UTF-16 code point.
     * See UTF-16 for more details.
     *
     * @param value UTF-16 code point to be examinated.
     *
     * @returns the number of code units to encode the char.
     */
    public static codeUnitsToEncode(value: UTF_16_CodePoint): int {
        if (Char.isInBasicMultilingualPlane(value)) {
            return 1;
        }
        return 2;
    }

    /**
     * isHighSurrogate(char) checks if the char is a high surrogate.
     *
     * @param value the char to be checked.
     *
     * @returns true if the char is a high surrogate, false otherwise.
     */
    public static isHighSurrogate(value: char): boolean {
        return Char.HIGH_SURROGATE_MIN <= value
            && Char.HIGH_SURROGATE_MAX >= value;
    }

    /**
     * isLowSurrogate(char) checks if the char is a low surrogate.
     *
     * @param value the char to be checked.
     *
     * @returns true if the char is a low surrogate, false otherwise.
     */
    public static isLowSurrogate(value: char): boolean {
        return Char.LOW_SURROGATE_MIN <= value
            && Char.LOW_SURROGATE_MAX >= value;
    }

    /**
     * getHighSurrogate(UTF_16_CodePoint) splits code point as a two code units and return the first one.
     * The result can be malformed und thus has to be checked with {@link <isHighSurrogate(char)>}.
     *
     * @param value an encoded code point.
     *
     * @returns the high surrogate, possibly malformed.
     */
    public static getHighSurrogate(value: UTF_16_CodePoint): char {
        return (((value - 0x10000) >>> 10) + Char.HIGH_SURROGATE_MIN) as char
    }

    /**
     * getLowSurrogate(UTF_16_CodePoint) splits code point as a two code units and return the second one.
     * The result can be malformed und thus has to be checked with {@link <isLowSurrogate(char)>}.
     *
     * @param value an encoded code point.
     *
     * @returns the low surrogate, possibly malformed.
     */
    public static getLowSurrogate(value: UTF_16_CodePoint): char {
        return (((value - 0x10000) & 0x3FF) + Char.LOW_SURROGATE_MIN) as char
    }

    /**
     * isPartOfSurrogatePair(char) checks whether the char is low or high surrogate.
     *
     * @param value the char to be tested.
     *
     * @returns true if the argument is correctly encoded low or high surrogate.
     */
    public static isPartOfSurrogatePair(value: char): boolean {
        return Char.isHighSurrogate(value) || Char.isLowSurrogate(value);
    }

    /**
     * isPartOfSurrogatePair() checks whether the underlying char is low or high surrogate.
     *
     * @returns true if the argument is correctly encoded low or high surrogate.
     */
    public isPartOfSurrogatePair(): boolean {
        return Char.isPartOfSurrogatePair(this.value);
    }

    /**
     * charsToCodePoint(char, char) combines to chars to code point
     *
     * @returns codepoint decoded from UTF-16.
     */
    public static charsToCodePoint(highValue: char, lowValue: char): UTF_16_CodePoint {
        return ((highValue - Char.HIGH_SURROGATE_MIN) * 0x400) + (lowValue - Char.LOW_SURROGATE_MIN) + 0x10000;
    }

    /**
     * isBinDigit() checks whether the char represents a binary digit.
     *
     * @param value a char to check.
     *
     * @returns true if the char is a binary digit.
     */
    public static isBinDigit(value: char): boolean {
        let diff: int = value - c'0';
        return (0 == diff) || (diff == 1);
    }

    /**
     * isBinDigit() checks whether the underlying char represents a binary digit.
     *
     * @returns true if the char is a binary digit.
     */
    public isBinDigit(): boolean {
        return Char.isBinDigit(this.value);
    }

    /**
     * isDecDigit() checks whether the char represents a decimal digit.
     *
     * @param value a char to check.
     *
     * @returns true if the char is a decimal digit.
     */
    public static isDecDigit(value: char): boolean {
        let diff: int = value - c'0';
        return (0 <= diff) && (diff <= 9);
    }

    /**
     * isDecDigit() checks whether the underlying char represents a decimal digit.
     *
     * @returns true if the char is a decimal digit.
     */
    public isDecDigit(): boolean {
        return Char.isDecDigit(this.value);
    }

    /**
     * isBinDigit() checks whether the char represents a hexadecimal digit.
     *
     * @param value a char to check.
     *
     * @returns true if the char is a hexadecimal digit.
     */
    public static isHexDigit(value: char): boolean {
        let isDigit = (c'0' <= value) && (value <= c'9')
        let isChar = ((c'a' <= value) && (value <= c'f'))
                  || ((c'A' <= value) && (value <= c'F'))
        return isDigit || isChar
    }

    /**
     * isHexDigit() checks whether the underlying char represents a hexadecimal digit.
     *
     * @returns true if the char is a hexadecimal digit.
     */
    public isHexDigit(): boolean {
        return Char.isHexDigit(this.value);
    }

    /**
     * isLetter(char) checks whether the char is a letter.
     *
     * @param value a char to be tested.
     *
     * @returns true if the char is a letter.
     */
    public static isLetter(value: char): boolean {
        return (c'A' <= value) && (value <= c'Z')
            || (c'a' <= value) && (value <= c'z');
    }

    /**
     * isLetter() checks whether the underlying char is a letter.
     *
     * @returns true if the char is a letter.
     */
    public isLetter(): boolean {
        return Char.isLetter(this.value);
    }

    /**
     * isUpperCase(char) checks whether the char is an upper case letter.
     *
     * @param value a char to be tested.
     *
     * @returns true if the char is an upper case letter.
     */
    public static native isUpperCase(value: char): boolean;

    /**
     * isUpperCase() checks whether the underlying char is an upper case letter.
     *
     * @returns true if the char is an upper case letter.
     */
    public isUpperCase(): boolean {
        return Char.isUpperCase(this.value);
    }

    /**
     * isLowerCase(char) checks whether the char is a lower case letter.
     *
     * @param value a char to be tested.
     *
     * @returns true if the char is a lower case letter.
     */
    public static native isLowerCase(value: char): boolean

    /**
     * isLowerCase() checks whether the underlying char is a lower case letter.
     *
     * @returns true if the char is a lower case letter.
     */
    public isLowerCase(): boolean {
        return Char.isLowerCase(this.value);
    }

    /**
     * toUpperCase(char) converts the char to upper case if it is in lower case, otherwise the char returned itself
     *
     * @param value a char to transform to upper case.
     *
     * @returns if char is in lower case then an upper case of it is returned, otherwise the argument itself returned.
     */
    public static native toUpperCase(value: char): char

    /**
     * toUpperCase() converts the underlying char to upper case if it is in lower case, otherwise the char unchanged
     */
    public toUpperCase(): void {
        this.value = Char.toUpperCase(this.value);
    }

    /**
     * toLowerCase(char) converts the char to lower case if it is in upper case, otherwise the char returned itself
     *
     * @param value a char to transform to upper case.
     *
     * @returns if char is in lower case then an upper case of it is returned, otherwise the argument itself returned.
     */
    public static native toLowerCase(value: char): char

    /**
     * toLowerCase() converts the underlying char to a lower case if it is in upper case, otherwise the char unchanged.
     */
    public toLowerCase(): void {
        this.value = Char.toLowerCase(this.value);
    }

    /**
     * isWhiteSpace(char) checks whether the char is a whitespace char.
     *
     * @param value a char to be tested.
     *
     * @returns true if the char is a whitespace.
     */
    public static native isWhiteSpace(value: char): boolean;

    /**
     * isWhiteSpace() checks whether the underlying char is a whitespace char.
     *
     * @returns true if the char is a whitespace.
     */
     public isWhiteSpace(): boolean {
        return Char.isWhiteSpace(this.value);
    }

    /**
     * Creates a Char instance based on JSONValue
     *
     * @param json: JSONValue - a JSON representation
     *
     * @throws JSONTypeError if json does not encode a valid char
     *
     * @returns char - char value decoded from JSON
     */
    public createFromJSONValue(json: JSONValue): Char {
        if (json instanceof JSONString) {
            let str = (json as JSONString).value
            if (str.getLength() == 1) {
                return Char.valueOf(str.charAt(0))
            }
        }
        throw new JSONTypeError("Cannot create Char from JSON", new ErrorOptions(json as Object))
    }
}
