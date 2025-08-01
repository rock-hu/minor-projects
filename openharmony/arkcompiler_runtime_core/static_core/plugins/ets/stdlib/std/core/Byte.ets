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

/**
 * Represents boxed byte value and related operations
 */
export final class Byte extends Integral implements Comparable<Byte>, JSONable<Byte> {
    private value: byte;

    /**
     * Constructs a new Byte instance with initial value zero
     */
    public constructor() {
        this.value = 0;
    }

    /**
     * Constructs a new Byte instance with provided initial value
     *
     * @param value the initial value
     */
    public constructor(value: byte) {
        this.value = value;
    }

    /**
     * Returns value of this instance as a primitive
     *
     * @returns value of this instance
     */
    public unboxed(): byte {
        return this.value;
    }

    /**
     * Returns boxed representation of the primitive
     *
     * @param value value to box
     *
     * @returns boxed value
     */
    public static valueOf(value: byte): Byte {
        // TODO(ivan-tyulyandin): caching is possible
        return new Byte(value);
    }

    /**
     * Minimal value that this type can have as an integral
     */
    public static MIN_VALUE: byte = -128;

    /**
     * Maximal value that this type can have as an integral
     */
    public static MAX_VALUE: byte = 127;

    /**
     * Size of this type in bits
     */
    public static BIT_SIZE: byte = 8;

    /**
     * Size of this type in bytes
     */
    public static BYTE_SIZE: byte = 1;

    /**
     * Returns value of this instance
     *
     * @returns value as byte
     */
    public override toByte(): byte {
        return this.value;
    }

    /**
     * Returns value of this instance
     *
     * @returns value as short
     */
    public override toShort(): short {
        return this.value.toShort();
    }

    /**
     * Returns value of this instance
     *
     * @returns value as int
     */
    public override toInt(): int {
        return this.value.toInt();
    }

    /**
     * Returns value of this instance
     *
     * @returns value as long
     */
    public override toLong(): long {
        return this.value.toLong();
    }

    /**
     * Returns value of this instance
     *
     * @returns value as float
     */
    public override toFloat(): float {
        return this.value.toFloat();
    }

    /**
     * Returns value of this instance
     *
     * @returns value as double
     */
    public override toDouble(): double {
        return this.value.toDouble();
    }

    /**
     * Returns value of this instance
     *
     * @returns value as char
     */
    public toChar(): char {
        return this.value.toChar();
    }

    /**
     * Returns the primitive as short value
     *
     * @param value value to cast
     *
     * @returns casted value
     */
    public static native toShort(value: byte): short;

    /**
     * Returns the primitive as int value
     *
     * @param value value to cast
     *
     * @returns casted value
     */
    public static native toInt(value: byte): int;

    /**
     * Returns the primitive as long value
     *
     * @param value value to cast
     *
     * @returns casted value
     */
    public static native toLong(value: byte): long;

    /**
     * Returns the primitive as float value
     *
     * @param value value to cast
     *
     * @returns casted value
     */
    public static native toFloat(value: byte): float;

    /**
     * Returns the primitive as double value
     *
     * @param value value to cast
     *
     * @returns casted value
     */
    public static native toDouble(value: byte): double;

    /**
     * Returns the primitive as char value
     *
     * @param value value to cast
     *
     * @returns casted value
     */
    public static native toChar(value: byte): char;

    /**
     * Returns the primitive as byte value
     *
     * @param value value to cast
     *
     * @returns casted value
     */
    public static toByte(value: byte): byte {
        return value;
    }

    /**
     * Compares this instance to other Byte object
     * The result is less than 0 if this instance lesser than provided object
     * 0 if they are equal
     * and greater than 0 otherwise.
     *
     * @param other Byte object to compare with
     *
     * @returns result of the comparison
     */
    public override compareTo(other: Byte): int {
        return (this.value - other.unboxed()) as int;
    }

    /**
     * Converts this object to a string
     *
     * @returns result of the conversion
     */
    public override toString(): String {
        // Check for zero, log10 would go inf otherwise
        if (this.value == 0) {
            return "0";
        }
        let val = this.value;
        let negative: boolean = (val < 0);
        let digitsNum = Double.toInt(log10(abs(this.value)) + 1);
        if (negative) {
            ++digitsNum;
        }
        let data : FixedArray<char> = new char[digitsNum];
        let curPlace = digitsNum - 1;
        while (val != 0) {
            let remainder = val % 10;
            if (negative) {
                remainder = -remainder;
            }
            data[curPlace] = (remainder + c'0').toChar();
            val /= 10;
            curPlace -= 1;
        }
        if (negative) {
            data[0] = c'-';
        }
        return new String(data);
    }

    /**
     * Converts this object to a string
     *
     * @returns result of the conversion
     */
    public toString(radix: number): string {
        return (new Long(this.value)).toString(radix);
    }

    /**
     * Returns a hash code (integer representation) for this instance
     *
     * @returns representation of this instance
     */
    public override $_hashCode(): int {
        return this.toInt();
    }

    /**
    * Checks for equality this instance with provided object, treated as a Byte
    *
    * @param other object to be checked against
    *
    * @returns true if provided object and this instance have same value, false otherwise
    * Returns false if type of provided object is not the same as this type
    */
    public equals(other: NullishType): boolean {
        if (this === other) {
            return true
        }

        if (!(other instanceof Byte)) {
            return false
        }

        return this.value == (other as Byte).value
    }

    /**
     * Performs integral addition of this instance with provided one, returns the result as new instance
     *
     * @param other Right hand side of the addition
     *
     * @returns Result of the addition
     */
    public add(other: Byte): Byte {
        return (this.value + other.toByte()).toByte()
    }

    /**
     * Performs integral subtraction of this instance with provided one, returns the result as new instance
     *
     * @param other Right hand side of the subtraction
     *
     * @returns Result of the subtraction
     */
    public sub(other: Byte): Byte {
        return (this.value - other.toByte()).toByte()
    }

    /**
     * Performs integral multiplication of this instance with provided one, returns the result as new instance
     *
     * @param other Right hand side of the multiplication
     *
     * @returns Result of the multiplication
     */
    public mul(other: Byte): Byte {
        return (this.value * other.toByte()).toByte()
    }

    /**
     * Performs integral division of this instance with provided one, returns the result as new instance
     *
     * @param other Right hand side of the division
     *
     * @returns Result of the division
     */
    public div(other: Byte): Byte {
        return (this.value / other.toByte()).toByte()
    }

    /**
     * Checks if this instance value is less than value of provided instance
     *
     * @param other Right hand side of the comparison
     *
     * @returns true if this value is less than provided, false otherwise
     */
    public isLessThan(other: Byte): boolean {
        return this.value < other.toByte();
    }

    /**
     * Checks if this instance value is less than or equal to value of provided instance
     *
     * @param other Right hand side of the comparison
     *
     * @returns true if this value is less than or equal to provided, false otherwise
     */
    public isLessEqualThan(other: Byte): boolean {
        return this.value <= other.toByte();
    }

    /**
     * Checks if this instance value is greater than value of provided instance
     *
     * @param other Right hand side of the comparison
     *
     * @returns true if this value is greater than provided, false otherwise
     */
    public isGreaterThan(other: Byte): boolean {
        return this.value > other.toByte();
    }

    /**
     * Checks if this instance value is greater than or equal to value of provided instance
     *
     * @param other Right hand side of the comparison
     *
     * @returns true if this value is greater than or equal to provided, false otherwise
     */
    public isGreaterEqualThan(other: Byte): boolean {
        return this.value >= other.toByte();
    }


    /**
     * Creates a Byte instance based on JSONValue
     *
     * @param json: JSONValue - a JSON representation
     *
     * @throws JSONTypeError if json does not encode a valid byte
     *
     * @returns Byte - byte value decoded from JSON
     */
    public createFromJSONValue(json: JSONValue): Byte {
        if (json instanceof JSONNumber) {
            let num = (json as JSONNumber).value
            if (Double.isInteger(num) && Byte.MIN_VALUE <= num && num <= Byte.MAX_VALUE) {
                return num.toByte()
            }
        }
        throw new JSONTypeError("Cannot create Byte from JSON", new ErrorOptions(json as Object))
    }

 }
