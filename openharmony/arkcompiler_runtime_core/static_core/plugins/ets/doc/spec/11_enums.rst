..
    Copyright (c) 2021-2025 Huawei Device Co., Ltd.
    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at
    http://www.apache.org/licenses/LICENSE-2.0
    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.

.. _Enumerations:

Enumerations
############

.. meta:
    frontend_status: Done

Enumeration type ``enum`` specifies a distinct user-defined type with an
associated set of named constants that define its possible values.

The syntax of *enumeration declaration* is presented below:

.. code-block:: abnf

    enumDeclaration:
        'const'? 'enum' identifier (':' type)? '{' enumConstantList? '}'
        ;

    enumConstantList:
        enumConstant (',' enumConstant)* ','?
        ;

    enumConstant:
        identifier ('=' constantExpression)?
        ;

.. index::
   enumeration
   type enum
   user-defined type
   constant
   named constant

Type ``const enum`` is supported for source-level compatibility with |TS|.
Type ``const`` is skipped as it has no impact on ``enum`` semantics in
|LANG|.

Enumerations with explicitly specifed type values are described here 
:ref:`Enumeration with explicit type`.

Qualification by type is mandatory to access the enumeration constant, except
enumeration constant initialization expressions:

.. code-block:: typescript
   :linenos:

    enum Color { Red, Green, Blue }
    let c: Color = Color.Red

    enum Flags { Read, Write, ReadWrite = Read | Write }
    // No need to use Flags.Read | Flags.Write in initialization


If enumeration type is exported, then all enumeration constants are
exported along with the mandatory qualification.

For example, if *Color* is exported, then all constants like ``Color.Red``
are exported along with the mandatory qualification ``Color``.

.. index::
   source-level compatibility
   const enum
   semantics
   qualification
   access
   accessibility
   enumeration constant
   enumeration type
   enum constant
   const enum

The value of an enum constant can be set as follows:

-  Explicitly to a numeric constant expression (expression of type ``int`` or
   ``long`` ) or to a constant expression of type ``string``; or
-  Implicitly by omitting the constant expression.

If constant expression is omitted, then the value of the enum constant is set
implicitly to an integer value (see :ref:`Enumeration Integer Values`).

A :index:`compile-time error` occurs if integer or ``string`` type enumeration
constants are combined in one enumeration.

.. index::
   enum constant
   numeric constant expression
   type int
   constant expression
   numeric value
   enumeration constant
   integer type
   string type
   enumeration

A type all enumeration constant values belongs is called *enumeration base type*.
It is either ``int``, or ``long``, or ``string``.

.. index::
   enumeration base type

Any enumeration constant is of type ``enumeration``. Implicit conversion (see
:ref:`Enumeration to Constants Type Conversions`) of an enumeration constant
to integer types or type ``string`` depends on the type of
constants.

In addition, all enumeration constant names must be unique. Otherwise,
a :index:`compile-time error` occurs.

.. code-block:: typescript
   :linenos:

    enum E1 { A, B = "hello" }     // compile-time error
    enum E2 { A = 5, B = "hello" } // compile-time error
    enum E3 { A = 5, A = 77 }      // compile-time error
    enum E4 { A = 5, B = 5 }       // OK! values can be the same

Empty ``enum`` is supported as a corner case for compatibility with |TS|.

.. code-block:: typescript
   :linenos:

    enum Empty {} // OK


.. index::
   enumeration constant
   type enumeration
   conversion
   type string
   constant
   expression
   enum
   compatibility

|

.. _Enumeration Integer Values:

Enumeration Integer Values
**************************

.. meta:
    frontend_status: Done

The integer value of an ``enum`` constant is set implicitly if an enumeration
constant specifies no value.

A constant expression of type ``int`` or ``long`` can be used to set the value
explicitly:

.. index::
   enumeration integer value
   integer value
   enum constant
   enumeration constant
   integer type
   value
   expression
   constant expression
   type int
   type long

.. code-block:: typescript
   :linenos:

    enum Background { White = 0xFF, Grey = 0x7F, Black = 0x00 }
    enum LongEnum { A = 0x7FFF_FFFF_1, B, C }

Choosing which type to use---``int`` or ``long``---is based on the same
principle as for integer literals (see :ref:`Integer Literals`).

If all constants have no value, then the first constant is assigned
the value zero. The other constant is assigned the value of the
immediately preceding constant plus one.

If some but not all constants have their values set explicitly, then
the values of the constants are set by the following rules:

-  The value of the first constant without an explicit value is assigned to
   zero.
-  A constant with an explicit value has that explicit value.
-  A constant that is not the first and has no explicit value takes the value
   of the immediately preceding constant plus one.

In the example below, the value of ``Red`` is 0, of ``Blue``, 5, and of
``Green``, 6:

.. code-block:: typescript
   :linenos:

    enum Color { Red, Blue = 5, Green }

.. index::
   type int
   type long
   integer literal
   assignment
   constant
   value
   assignment

|

.. _Enumeration String Values:

Enumeration String Values
*************************

.. meta:
    frontend_status: Done

A string value for enumeration constants must be set explicitly:

.. code-block:: typescript
   :linenos:

    enum Commands { Open = "fopen", Close = "fclose" }

.. index::
   string value
   value
   enumeration
   enumeration constant

|

.. _Enumeration Operations:

Enumeration Operations
**********************

.. meta:
    frontend_status: Done

The value of an enumeration constant can be converted to type ``string`` by
using the method ``toString``:

.. index::
   enumeration constant
   type string
   method

.. code-block:: typescript
   :linenos:

    enum Color { Red, Green = 10, Blue }
    let c: Color = Color.Green
    console.log(c.toString()) // prints: 10

The name of enumeration type can be indexed by the value of this enumeration
type to get the name of the constant:

.. code-block:: typescript
   :linenos:

    enum Color { Red, Green = 10, Blue }
    let c: Color = Color.Green
    console.log(Color[c]) // prints: Green

If several enumeration constants have the same value, then the textually last
constant has the priority:

.. code-block:: typescript
   :linenos:

    enum E { One = 1, one = 1, oNe = 1 }
    console.log(E[1 as E]) // prints: oNe


Additional methods available for enumeration types and constants are discussed
in :ref:`Enumeration Methods` in the chapter Experimental Features.

.. index::
   method
   enumeration type
   constant

.. raw:: pdf

   PageBreak
