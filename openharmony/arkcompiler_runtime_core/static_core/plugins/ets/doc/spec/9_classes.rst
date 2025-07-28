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

.. _Classes:

Classes
#######

.. meta:
    frontend_status: Done

Class declarations introduce new reference types and describe the manner
of their implementation.

A class body contains declarations and initializer blocks.

Declarations can introduce class members (see :ref:`Class Members`) or class
constructors (see :ref:`Constructor Declaration`).

The body of the declaration of a member comprises the scope of a
declaration (see :ref:`Scopes`).

Class members include:

-  Fields,
-  Methods, and
-  Accessors.

.. index::
   class declaration
   class constructor
   reference type
   implementation
   class body
   field
   method
   accessor
   constructor
   class member
   initializer block
   scope

Class members can be *declared* or *inherited*.

Every member is associated with the class declaration it is declared in.

Field, method, accessor and constructor declarations can have the following
access modifiers (see :ref:`Access Modifiers`):

-  ``Public``,
-  ``Protected``,
-  ``Internal``, or
-  ``Private``.

A newly declared method can shadow, overload, implement, or override a method
declared in a superclass or superinterface.

Every class defines two class-level scopes (see :ref:`Scopes`): one for
instance members, and the other for static members. It means that two members
of a class can have the same name if one is static while the other is not.

.. index::
   class declaration
   declared class member
   inherited class member
   access modifier
   accessor
   method
   field
   shadowing
   overloading
   implementing
   overriding
   superclass
   superinterface
   class-level scope
   instance member
   static member

|

.. _Class Declarations:

Class Declarations
******************

.. meta:
    frontend_status: Done

Every class declaration defines a *class type*, i.e., a new named
reference type.

The class name is specified by an *identifier* inside a class declaration.

If ``typeParameters`` are defined in a class declaration, then that class
is a *generic class* (see :ref:`Generics`).

The syntax of *class declaration* is presented below:

.. code-block:: abnf

    classDeclaration:
        classModifier? 'class' identifier typeParameters?
          classExtendsClause? implementsClause? classBody
        ;

    classModifier:
        'abstract' | 'final'
        ;

The scope of a class declaration is specified in :ref:`Scopes`.

An example of a class is presented below:

.. code-block:: typescript
   :linenos:

    class Point {
      public x: number
      public y: number
      public constructor(x : number, y : number) {
        this.x = x
        this.y = y
      }
      public distanceBetween(other: Point): number {
        return Math.sqrt(
          (this.x - other.x) * (this.x - other.x) +
          (this.y - other.y) * (this.y - other.y)
        )
      }
      static origin = new Point(0, 0)
    }

.. index::
   class declaration
   class type
   reference type
   class name
   identifier
   generic class
   scope

|

.. _Abstract Classes:

Abstract Classes
================

.. meta:
    frontend_status: Done

A class with the modifier ``abstract`` is known as abstract class.
Abstract classes can be used to represent notions that are common
to some set of more concrete notions.

A :index:`compile-time error` occurs if an attempt is made to create
an instance of an abstract class:

.. code-block:: typescript
   :linenos:

   abstract class X {
      field: number
      constructor (p: number) { this.field = p }
   }
   let x = new X(42)
     // Compile-time error: Cannot create an instance of an abstract class.

Subclasses of an abstract class can be abstract or non-abstract.
A non-abstract subclass of an abstract superclass can be instantiated. As a
result, a constructor for the abstract class, and field initializers
for non-static fields of that class are executed:

.. index::
   abstract class
   modifier abstract
   abstract class
   subclass
   non-abstract class
   field initializer
   constructor
   non-static field

.. code-block:: typescript
   :linenos:

   abstract class Base {
      field: number
      constructor (p: number) { this.field = p }
   }

   class Derived extends Base {
      constructor (p: number) { super(p) }
   }

A method with the modifier ``abstract`` is considered an *abstract method*
(see :ref:`Abstract Methods`).
Abstract methods have  no bodies, i.e., they can be declared but not
implemented.

Only abstract classes can have abstract methods.
A :index:`compile-time error` occurs if a non-abstract class has
an abstract method:

.. code-block:: typescript
   :linenos:

   class Y {
     abstract method (p: string)
     /* Compile-time error: Abstract methods can only
        be within an abstract class. */
   }

A :index:`compile-time error` occurs if an abstract method declaration
contains the modifiers ``final`` or ``override``.

.. index::
   modifier abstract
   abstract method
   method body
   non-abstract class
   class
   method declaration

|

.. _Final Classes:

Final Classes
=============

.. meta:
    frontend_status: Done

Final classes are discussed in the chapter Experimental Features (see
:ref:`Final Classes Experimental`).

.. index::
   final class

|

.. _Class Extension Clause:

Class Extension Clause
======================

.. meta:
    frontend_status: Done

All classes except class ``Object`` can contain the ``extends`` clause that
specifies the *base class*, or the *direct superclass* of the current class.
In this situation, the current class is a *derived class*, or a
*direct subclass*. Any class, except class ``Object`` that has no ``extends``
clause, is assumed to have the ``extends Object`` clause.

.. index::
   class
   Object
   extends clause
   base class
   derived class
   direct subclass
   clause
   direct superclass
   superclass

The syntax of *class extension clause* is presented below:

.. code-block:: abnf

    classExtendsClause:
        'extends' typeReference
        ;

A :index:`compile-time error` occurs if:

-  An ``extends`` clause appears in the definition of the class ``Object``,
   which is the top of type hierarchy, and has no superclass.

-  Class type named by ``typeReference`` is not accessible (see
   :ref:`Accessible`).

-  The ``extends`` graph has a cycle.

-  ``typeReference`` refers directly to, or is an alias of interface, or of any
   non-class type, e.g., of primitive, array, string, enumeration, union,
   function, or utility type.

*Class extension* implies that a class inherits all members of the direct
superclass.

**Note**. Private members are inherited from superclasses, but are not
accessible (see :ref:`Accessible`) within subclasses:

.. index::
   class
   extends clause
   Object
   superclass
   type
   enum type
   primitive type
   class type
   class extension
   extends clause
   extends graph
   type argument
   inheritance
   access
   private member

.. code-block:: typescript
   :linenos:

    class Base {
      // All methods are mutually accessible in the class where
          they were declared
      public publicMethod () {
        this.protectedMethod()
        this.privateMethod()
      }
      protected protectedMethod () {
        this.publicMethod()
        this.privateMethod()
      }
      private privateMethod () {
        this.publicMethod();
        this.protectedMethod()
      }
    }
    class Derived extends Base {
      foo () {
        this.publicMethod()    // OK
        this.protectedMethod() // OK
        this.privateMethod()   // compile-time error:
                               // the private method is inaccessible
      }
    }

The transitive closure of a *direct subclass* relationship is the *subclass*
relationship. Class ``A`` can be a subclass of class ``C`` if:

-  Class ``A`` is the direct subclass of ``C``; or

-  Class ``A`` is a subclass of some class ``B``,  which is in turn a subclass
   of ``C`` (i.e., the definition applies recursively).

Class ``C`` is a *superclass* of class ``A`` if ``A`` is its subclass.

.. index::
   transitive closure
   direct subclass
   subclass relationship
   subclass
   class

|

.. _Class Implementation Clause:

Class Implementation Clause
===========================

.. meta:
    frontend_status: Done

A class can implement one or more interfaces. Interfaces to be implemented by
a class are listed in the ``implements`` clause. Interfaces listed in this
clause are *direct superinterfaces* of the class.

The syntax of *class implementation clause* is presented below:

.. code-block:: abnf

    implementsClause:
        'implements' interfaceTypeList
        ;

    interfaceTypeList:
        typeReference (',' typeReference)*
        ;

A :index:`compile-time error` occurs if ``typeReference`` fails to name an
accessible interface type (see :ref:`Accessible`).

.. code-block:: typescript
   :linenos:

    // File1
    interface I { } // Not exported

    // File2
    import {I} from "File1"
    class C implements I {}
       // Compile-time error I is not accessible

If some interface is repeated as a direct superinterface in a single
``implements`` clause (even if that interface is named differently), then all
repetitions are ignored.

.. index::
   class declaration
   class implementation clause
   implements clause
   accessible interface type
   type argument
   interface
   direct superinterface

For the class declaration ``C`` <``F``:sub:`1` ``,..., F``:sub:`n`> (:math:`n\geq{}0`,
:math:`C\neq{}Object`):

- *Direct superinterfaces* of class type ``C`` <``F``:sub:`1` ``,..., F``:sub:`n`>
  are the types specified in the ``implements`` clause of the declaration of
  ``C`` (if there is an ``implements`` clause).

For the generic class declaration ``C`` <``F``:sub:`1` ``,..., F``:sub:`n`> (*n* > *0*):

-  *Direct superinterfaces* of the parameterized class type ``C``
   < ``T``:sub:`1` ``,..., T``:sub:`n`> are all types ``I``
   < ``U``:sub:`1`:math:`\theta{}` ``,..., U``:sub:`k`:math:`\theta{}`> if:

    - ``T``:sub:`i` (:math:`1\leq{}i\leq{}n`) is a type;
    - ``I`` <``U``:sub:`1` ``,..., U``:sub:`k`> is the direct superinterface of
      ``C`` <``F``:sub:`1` ``,..., F``:sub:`n`>; and
    - :math:`\theta{}` is the substitution [``F``:sub:`1` ``:= T``:sub:`1` ``,..., F``:sub:`n` ``:= T``:sub:`n`].

.. index::
   class declaration
   parameterized class type
   direct superinterface
   implements clause
   substitution
   generic class declaration
   parameterized class type

Interface type ``I`` is a superinterface of class type ``C`` if ``I`` is one of
the following:

-  Direct superinterface of ``C``;
-  Superinterface of ``J`` which is in turn a direct superinterface of ``C``
   (see :ref:`Superinterfaces and Subinterfaces` that defines superinterface
   of an interface); or
-  Superinterface of the direct superclass of ``C``.

A class *implements* all its superinterfaces.

A :index:`compile-time error` occurs if a class implements
two interface types that represent different instantiations of the same
generic interface (see :ref:`Generics`).

.. index::
   class type
   direct superinterface
   superinterface
   interface
   superclass
   class
   interface type
   instantiation
   generic interface

If a class is not declared *abstract*, then:

-  Any abstract method of each direct superinterface is implemented (see
   :ref:`Inheritance`) by a declaration in that class.
-  The declaration of an existing method is inherited from a direct superclass,
   or a direct superinterface.

If superinterfaces have more then one default implementations (see
:ref:`Default Interface Method Declarations`) for some method ``m``, then:

- The class that implements these interfaces has method that overrides ``m`` 
  (see :ref:`Override-Compatible Signatures`); or

- There is a single interface method with default implementation
  that overrides all other methods; or

- All interface methods refer to the same implementation, and this default
  implementation is the current class method.

Otherwise, a :index:`compile-time error` occurs.

.. index::
   abstract class
   abstract method
   direct superinterface
   superinterface
   inheritance
   direct superclass
   implementation
   class
   override-compatible signature

.. code-block:: typescript
   :linenos:

    interface I1 { foo () {} }
    interface I2 { foo () {} }
    class C1 implements I1, I2 {
       foo () {} // foo() from C1 overrides both foo() from I1 and foo() from I2
    }

    class C2 implements I1, I2 {
       // Compile-time error as foo() from I1 and foo() from I2 have different implementations
    }

    interface I3 extends I1 {}
    interface I4 extends I1 {}
    class C3 implements I3, I4 {
       // OK, as foo() from I3 and foo() from I4 refer to the same implementation
    }

    interface I5 extends I1 { foo() {} } // override method from I1
    class C4 implements I1, I5 {
       // OK, as I5.foo overrides I1.foo, and there is only one implementation accessible
    }

A single method declaration in a class is allowed to implement methods of one
or more superinterfaces.

A :index:`compile-time error` occurs if a class field has the same name as
a method from one of superinterfaces implemented by the class, except when one
is static and the other is not.

.. index::
   method declaration
   method
   superinterface
   class field
   superclass
   implementation

|

.. _Implementing Interface Properties:

Implementing Interface Properties
=================================

.. meta:
    frontend_status: Done

A class must implement all properties from all superinterfaces (see
:ref:`Interface Properties`) that are always defined as a getter, a
setter, or both. Providing implementation for the property in the form of
a field is not necessary:

.. code-block-meta:

.. code-block:: typescript
   :linenos:

    interface Style {
      get color(): string
      set color(s: string)
    }

    class StyleClassOne implements Style {
      color: string = ""
    }

    class StyleClassTwo implements Style {
      private color_: string = ""

      get color(): string {
        return this.color_
      }

      set color(s: string) {
        this.color_ = s
      }
    }

.. index::
   interface property
   class
   superinterface
   getter
   setter
   field

If a property is defined in a form that requires a setter, then the
implementation of the property in the form of a ``readonly`` field causes a
:index:`compile-time error`:

.. code-block-meta:
   expect-cte

.. code-block:: typescript
   :linenos:

    interface Style {
      set color(s: string)
      writable: number
    }

    class StyleClassTwo implements Style {
      readonly color: string = "" // compile-time error
      readonly writable: number = 0  // compile-time error
    }

    function write_into_read_only (s: Style) {
      s.color = "Black"
      s.writable = 42
    }

    write_into_read_only (new StyleClassTwo)

.. index::
   property
   implementation
   setter
   readonly field

If a property is defined in the ``readonly`` form, then the implementation of
the property can either keep the ``readonly`` form or extend it to a writable
form as follows:

.. code-block:: typescript
   :linenos:

    interface Style {
      get color(): string
      readonly readable: number
    }

    class StyleClassThree implements Style {
      get color(): string { return "Black" }
      set color(s: string) {} // OK!
      readable: number = 0  // OK!
    }

    function how_to_write (s: Style) {
      s.color = "Black" // compile-time error
      s.readable = 42 // compile-time error
      if (s instanceof StyleClassThree) {
        let s1 = s as StyleClassThree
        s1.color = "Black" // OK!
        s1.readable = 42 // OK!
      }
    }

    how_to_write (new StyleClassThree)

.. index::
   property
   readonly
   implementation
   class
   getter
   setter
   field

|

.. _Class Body:

Class Body
**********

.. meta:
    frontend_status: Done

A *class body* can contain declarations of the following members:

-  Fields,
-  Methods,
-  Accessors,
-  Constructors, and
-  Initializer blocks.

The syntax of *class body* is presented below:

.. code-block:: abnf

    classBody:
        '{'
           classBodyDeclaration* classInitializer? classBodyDeclaration*
        '}'
        ;

    classBodyDeclaration:
        annotationUsage?
        accessModifier?
        ( constructorDeclaration
        | classFieldDeclaration
        | classMethodDeclaration
        | classAccessorDeclaration
        )
        ;

Declarations can be inherited or immediately declared in a class. Any
declaration within a class has a class scope. The class scope is fully
defined in :ref:`Scopes`.

The usage of annotations is discussed in :ref:`Using Annotations`.

.. index::
   class body
   declaration
   member
   field
   method
   accessor
   type
   class
   interface
   constructor
   initializer block
   inheritance
   class scope
   scope

|

.. _Class Members:

Class Members
*************

.. meta:
    frontend_status: Done

Class members are as follows:

-  Members inherited from their direct superclass (see :ref:`Inheritance`),
   except class ``Object`` that cannot have a direct superclass.
-  Members declared in a direct superinterface (see
   :ref:`Superinterfaces and Subinterfaces`).
-  Members declared in the class body (see :ref:`Class Body`).

Class members declared ``private`` are not accessible (see :ref:`Accessible`)
to all subclasses of the current class.

.. index::
   inheritance
   class member
   inherited member
   direct superclass
   superinstance
   subinterface
   Object
   direct superinstance
   class body
   private
   subclass
   access

Class members declared ``protected`` or ``public`` are inherited by all
subclasses of the class and accessible (see :ref:`Accessible`) for all
subclasses.

Class members declared ``internal`` are accessible within the package the
current class resides in. They are inherited by all subclasses of the current
class.

Constructors and initializer blocks are not members, and are not inherited.

Members can be as follows:

.. index::
   class
   class member
   protected
   public
   subclass
   access
   internal
   constructor
   initializer block
   inheritance

-  Class fields (see :ref:`Field Declarations`),
-  Methods (see :ref:`Method Declarations`), and
-  Accessors (see :ref:`Accessor Declarations`).

A *method* is defined by the following:

#. *Type parameter*, i.e., the declaration of any type parameter of the
   method member.
#. *Argument type*, i.e., the list of types of arguments applicable to the
   method member.
#. *Return type*, i.e., the return type of the method member.

Members can be as follows:

-  Static members that are not part of class instances, and can be accessed
   by using a qualified name notation (see :ref:`Names`) anywhere the class
   name is accessible (see :ref:`Accessible`); and
-  Non-static, or instance members that belong to any instance of the class.

All names in static and, separately, non-static class declaration scopes (see
:ref:`Scopes`) must be unique, i.e., fields and methods cannot have the
same name.

.. index::
   class field
   method
   accessor
   accessor declaration
   type parameter
   argument type
   return type
   static member
   class instance
   qualified name
   notation
   class declaration scope
   field
   non-static class

|

.. _Access Modifiers:

Access Modifiers
****************

.. meta:
    frontend_status: Done

Access modifiers define how a class member or a constructor can be accessed.
Accessibility in |LANG| can be of the following kinds:

-  ``Private``,
-  ``Internal``,
-  ``Protected``, or
-  ``Public``.

The desired accessibility of class members and constructors can be explicitly
specified by the corresponding *access modifiers*.

The syntax of *class members or constructors modifiers* is presented below:

.. code-block:: abnf

    accessModifier:
        'private'
        | 'internal'
        | 'protected'
        | 'public'
        ;

If no explicit modifier is provided, then a class member or a constructor
is implicitly considered ``public`` by default.

.. index::
   access modifier
   member
   constructor
   private
   public
   accessibility

|

.. _Private Access Modifier:

Private Access Modifier
=======================

.. meta:
    frontend_status: Done
    todo: only parsing is implemented, but checking isn't implemented yet, need libpandafile support too

The modifier ``private`` indicates that a class member or a constructor is
accessible (see :ref:`Accessible`) within its declaring class, i.e., a private
member or constructor *m* declared in some class ``C`` can be accessed only
within the class body of ``C``:

.. code-block:: typescript
   :linenos:

    class C {
      private count: number
      getCount(): number {
        return this.count // ok
      }
    }

    function increment(c: C) {
      c.count++ // compile-time error - 'count' is private
    }

.. index::
   access modifier
   private
   private member
   class member
   constructor
   access
   accessibility
   declaring class
   class body

|

.. _Internal Access Modifier:

Internal Access Modifier
========================

.. meta:
    frontend_status: Partly
    todo: Implement in libpandafile, implement semantic, now it is parsed and ignored - #16088

The modifier ``internal`` is discussed in the chapter Experimental Features
(see :ref:`Internal Access Modifier Experimental`).

.. index::
   access modifier
   internal

|

.. _Protected Access Modifier:

Protected Access Modifier
=========================

.. meta:
    frontend_status: Done

The modifier ``protected`` indicates that a class member or a constructor is
accessible (see :ref:`Accessible`) only within its declaring class and the
classes derived from that declaring class. A protected member ``M`` declared in
some class ``C`` can be accessed only within the class body of ``C`` or of a
class derived from ``C``:

.. code-block:: typescript
   :linenos:

    class C {
      protected count: number
       getCount(): number {
         return this.count // ok
       }
    }

    class D extends C {
      increment() {
        this.count++ // ok, D is derived from C
      }
    }

    function increment(c: C) {
      c.count++ // compile-time error - 'count' is not accessible
    }

.. index::
   modifier protected
   access modifier
   accessible constructor
   method
   protected
   constructor
   accessibility
   class body
   derived class

|

.. _Public Access Modifier:

Public Access Modifier
======================

.. meta:
    frontend_status: Done
    todo: spec needs to be clarified - "The only exception and panic here is that the type the member or constructor belongs to must also be accessible"

The modifier ``public`` indicates that a class member or a constructor can be
accessed everywhere, provided that the member or the constructor belongs to
a type that is also accessible (see :ref:`Accessible`).

.. index::
   modifier public
   public
   access modifier
   protected
   access
   constructor
   accessibility
   accessible type

|

.. _Field Declarations:

Field Declarations
******************

.. meta:
    frontend_status: Partly
    todo: syntax for definite assignment

*Field declarations* represent data members in class instances or static data
members (see :ref:`Static Fields`).
Syntactically, a field declaration is similar to a variable declaration.

.. code-block:: abnf

    classFieldDeclaration:
        fieldModifier* 
        identifier
        ( '?'? ':' type initializer?
        | '?'? initializer
        | '!' ':' type
        )
        ;

    fieldModifier:
        'static' | 'readonly' | 'override'
        ;

A field with an identifier marked with '``?``' is called *optional field*
(see :ref:`Optional Fields`).
A field with an identifier marked with '``!``' is called
*field with late initialization*
(see :ref:`Fields with Late Initialization`).

A :index:`compile-time error` occurs if:

-  One and the same field modifier is used more than once in a field declaration.
-  Name of a field declared in the body of a class declaration is also
   used for a method of this class with the same static or
   non-static status.
-  Name of a field declared in the body of a class declaration is also
   used for another field in the same declaration with the same static or
   non-static status.

.. index::
   field declaration
   class instance field
   class instance variable
   field modifier
   field declaration
   method
   class
   class declaration
   static field
   non-static field

Any static field can be accessed only with the qualification of a superclass
name (see :ref:`Field Access Expression`).

A class can inherit more than one field or property with the same name from
its superinterfaces, or from both its superclass and superinterfaces. However,
an attempt to refer to such a field or property by its simple name within the
class body causes a :index:`compile-time error`.

The same field or property declaration can be inherited from an interface in
more than one way. In that case, the field or property is considered
to be inherited only once.

.. index::
   static field
   qualified name
   access
   superinterface
   field
   field declaration
   inheritance
   property declaration

|

.. _Static Fields:

Static Fields
=============

.. meta:
    frontend_status: Done

There are two categories of class fields as follows:

- Static fields

  Static fields are declared with the modifier ``static``. A static field
  is not part of a class instance. There is one copy of a static field
  irrespective of how many instances of the class (even if zero) are
  eventually created.

  Static fields are always accessed by using a qualified name notation
  wherever the class name is accessible (see :ref:`Accessible`).

- Instance, or non-static fields

  Instance fields belong to each instance of the class. An instance field
  is created for, and associated with a newly-created instance of a class,
  or of its superclass. An instance field is accessible (see :ref:`Accessible`)
  via the instance name.

.. index::
   class fields
   modifier static
   static
   static field
   instantiation
   instance
   initialization
   class
   class instance
   superclass
   non-static field
   accessibility
   access
   instance field
   qualified name
   notation
   instance name
   instance

|

.. _Readonly Constant Fields:

Readonly (Constant) Fields
==========================

.. meta:
    frontend_status: Done

A field with the modifier ``readonly`` is a *readonly field*. Changing
the value of a readonly field after initialization is not allowed. Both static
and non-static fields can be declared *readonly fields*.

.. index::
   readonly field
   modifier readonly
   readonly
   constant field
   initialization
   modifier
   static field
   non-static field

|

.. _Optional Fields:

Optional Fields
===============

.. meta:
    frontend_status: Partly

*Optional field* ``f?: T = expr`` effectively means that the type of ``f``is
``T | undefined``. If an *initializer* is absent in a *field declaration*,
then the default value ``undefined`` (see :ref:`Default Values for Types`) is
used as the initial value of the field.

.. index::
   undefined
   default value
   optional field

For example, the following two fields are actually defined the same way:

.. code-block:: typescript
   :linenos:

    class C {
        f?: string
        g: string | undefined = undefined
    }
    
|

.. _Field Initialization:

Field Initialization
====================

.. meta:
    frontend_status: Done

All fields except :ref:`Fields with Late Initialization` are initialized by
using the default value (see :ref:`Default Values for Types`), or field initializer
(see below) otherwise, the field can be initalized in

- An initializer block of for static field (see :ref:`Static Initialization`), or
- A class constructor of a non-static field (see :ref:`Constructor Declaration`).

.. index::
   field initialization
   evaluation
   field initializer
   field access
   expression
   field access expression
   field initializer
   initializer block
   static field
   class constructor
   non-static field

*Field initializer* is an expression that is evaluated at compile time or
runtime. The result of successful evaluation is assigned into the field. The
semantics of field initializers is therefore similar to that of assignments
(see :ref:`Assignment`). The initializer expression evaluation and subseqent
assignment is performed only once.

``Readonly`` fields initialization never uses default values (see
:ref:`Default Values for Types`).

.. index::
   field initializer
   evaluation
   expression
   compile time
   runtime
   access
   field
   semantics
   assignment
   keyword this
   keyword super
   method
   this
   super

In a non-static field declaration, an initializer is evaluated at runtime.
The assignment is performed each time an instance of the class is created.

The instance field initializer expression cannot do the following:

- Call methods of ``this`` or ``super``;
- Use ``this`` directly (as an argument of function calls or in assignments);

If the initializer expression contains one of the above patterns, then a
:index:`compile-time error` occurs.

If allowed in the code, the above restrictions can break the consistency of
class instances as shown in the following examples:

.. index::
   non-static field declaration
   initializer
   initializer expression
   uninitialized field
   evaluation
   runtime
   assignment
   instance
   class
   instance field initializer
   call method
   this
   super

.. code-block:: typescript
   :linenos:

    class C {
        a = this        // Compile-time error

        f1 = this.foo() // Compile-time error as 'this' method is invoked
        f2 = "a string field"

        foo (): string {
           // Type safety requires fields to be initialized before access
           console.log (this.f1, this.f2)
           return this.f2
        }

    }

.. index::
   compiler
   field initializer
   non-static field
   initialization
   circular dependency
   initializer
   initializer expression

|

.. _Fields with Late Initialization:

Fields with Late Initialization
===============================

.. meta:
    frontend_status: None

*Field with late initialization* must be an *instance field*. If it is defined
as ``static``, then a :index:`compile-time error` occurs.

A type of a *field with late initialization* can not be *nullish type*
(see :ref:`Nullish Types`), otherwise a :index:`compile-time error` occurs. 

As all other fields, a *field with late initialization* must be initialized
before it is used for the first time. However, this field can be initialized
*later* and not within a class declaration. 
Initialization of this field can be performed in a constructor
(see :ref:`Constructor Declaration`), although it is not mandatory.

A *field with late initialization* cannot have *field initializers* 
and cannot be an *optional field* (see :ref:`Optional Fields`).
It must be initialized explicitly, even though its type has
a *default value*.

The fact of initialization of *field with late initialization* is checked when
the field value is read. The check normally is performed at runtime. If the
compiler identifies an error situation, then the error is reported at compile time.

.. code-block:: typescript
   :linenos:

    class C {
        f!: string
    }
    
    let x = new C()
    x.f = "aa"
    console.log(x.f) // ok
    
    let y = new C()
    console.log(y.f) // run-time or compile-time error

**Note.** Access to a *field with late initialization* in most cases is less
performant then access to other fields.

|TS| uses the term *definite assignment assertion* for the notion similar to
*late initialization*. However, |LANG| uses stricter rules.

|

.. _Override Fields:

Overriding Fields
=================

.. meta:
    frontend_status: None

While extending a class or implementing interfaces, a field declared in a
superclass or a superinterface can be overridden by field with the same name,
the same type and the same ``static`` or non ``static`` modifier status.
Using the keyword ``override`` is not required. The new declaration acts as
redeclaration. The type of the overriding field is to be the same as the type of
the overridden field. Otherwise, a :index:`compile-time error` occurs. Initializers of
overridden fields are preserved for execution, initialization is performed in context of
*super* class constructors as normal. A :index:`compile-time error` occurs if a field
is not declared as ``readonly`` in a superclass but an overriding field is marked as ``readonly``.

.. code-block:: typescript
   :linenos:

    class Base1 {
        field: number = this.init_in_base_1()
        private init_in_base_1() {
           console.log ("Base1 field initialization")
           return 123
        }
    }
    interface Base2 {
        field: number
    }

    class Base3 extends Base1 {
        override field: number = 123 // Explicitly mark the field as overridden
    }
    console.log (Base1.field, Base3.field)

    class Derived extends Base3 implements Base2 {
        field = this.init_in_derived() // overriding 'field' and providing new initial value
        private init_in_derived() {
           console.log ("Derived field initialization")
           return 42
        }
    }
    new Derived()
    /* Output:
        Base1 field initialization
        Derived field initialization
    */


.. index::
   overriding
   field overriding
   overridden field
   initialization
   instance field
   superclass
   superinterface
   interface
   implementation
   keyword override
   readonly
   field

|

.. _Method Declarations:

Method Declarations
*******************

.. meta:
    frontend_status: Done

*Methods* declare executable code that can be called.

The syntax of *class method declarations* is presented below:

.. code-block:: abnf

    classMethodDeclaration:
        methodModifier* identifier typeParameters? signature block?
        ;

    methodModifier:
        'abstract'
        | 'static'
        | 'final'
        | 'override'
        | 'native'
        | 'async'
        ;

The identifier of ``classMethodDeclaration`` is the method name that can be
used to refer to a method (see :ref:`Method Call Expression`).

A :index:`compile-time error` occurs if:

-  The method modifier appears more than once in a method declaration.
-  The body of a class declaration declares a method but the name of that
   method is already used for a field in the same declaration.
-  The body of a class declaration declares two same-name methods with
   overload-equivalent signatures (see :ref:`Overload-Equivalent Signatures`)
   as members of that body of a class declaration.

.. index::
   method declaration
   executable code
   overloading signature
   identifier
   method call
   method call expression
   method modifier
   method declaration
   class declaration
   overload-equivalent signature
   class declaration body

|

.. _Static Methods:

Static Methods
==============

.. meta:
    frontend_status: Done

A method declared in a class with the modifier ``static`` is a *static method*.

A :index:`compile-time error` occurs if:

-  The method declaration contains another modifier (``abstract``, ``final``,
   or ``override``) along with the modifier ``static``.
-  The header or body of a class method includes the name of a type parameter
   of the surrounding declaration.

Static methods are always called without reference to a particular object. As
a result, a :index:`compile-time error` occurs if the keywords ``this`` or
``super`` are used inside a static method.

.. index::
   static method
   class
   modifier
   modifier abstract
   abstract
   modifier final
   final
   modifier override
   override
   modifier static
   static
   keyword this
   keyword super

|

.. _Instance Methods:

Instance Methods
================

.. meta:
    frontend_status: Done

A method that is not declared static is called *non-static method*, or
*instance method*.

An instance method is always called with respect to an object that becomes
the current object which the keyword ``this`` refers to during the execution
of the method body.

.. index::
   static method
   instance method
   non-static method
   keyword this
   method body

|

.. _Abstract Methods:

Abstract Methods
================

.. meta:
    frontend_status: Done

An *abstract* method declaration introduces the method as a member along
with its signature but without implementation. An abstract method is
declared with the modifier ``abstract`` in the declaration.

Non-abstract methods can be referred to as *concrete methods*.

A :index:`compile-time error` occurs if:

-  An abstract method is declared private.
-  The method declaration contains another modifier (``static``, ``final``,
   ``native``, or ``async``) along with the modifier ``abstract``.
-  The declaration of an abstract method *m* does not appear directly within
   abstract class ``A``.
-  Any non-abstract subclass of ``A`` (see :ref:`Abstract Classes`) does not
   provide implementation for *m*.

An abstract method declaration provided by an abstract subclass can override
another abstract method. A :index:`compile-time error` occurs if an abstract
method overrides a non-abstract instance method.

.. index::
   abstract method declaration
   abstract method
   non-abstract instance method
   non-abstract method
   method signature
   abstract
   modifier abstract
   modifier static
   static
   modifier final
   final
   modifier native
   native
   modifier async
   async
   private
   abstract class
   overriding

|

.. _Final Methods:

Final Methods
=============

.. meta:
    frontend_status: Done

Final methods are discussed in :ref:`Final Methods Experimental`.

.. index::
   final method

|

.. _Async Methods:

Async Methods
=============

.. meta:
    frontend_status: Done

Async methods are discussed in :ref:`Concurrency Async Methods`.

.. index::
   async method

|

.. _Overriding Methods:

Overriding Methods
==================

.. meta:
    frontend_status: Done

The ``override`` modifier indicates that an instance method in a superclass is
overridden by the corresponding instance method from a subclass (see
:ref:`Overloading and Overriding`).

The usage of the modifier ``override`` is optional but strongly recommended as
it makes the overriding explicit.

A :index:`compile-time error` occurs if:

-  A method marked with the modifier ``override`` does not override a method
   from a superclass.
-  A method declaration contains modifier ``static`` along with the modifier
   ``override``.

If the signature of an overridden method contains parameters with default
values (see :ref:`Optional Parameters`), then the overriding method always
uses the default parameter values of the overridden method.

A :index:`compile-time error` occurs if a parameter in the overriding method
has a default value.

More details on overriding are provided
in :ref:`Overloading and Overriding in Classes` and
:ref:`Overloading and Overriding in Interfaces`.


.. index::
   modifier override
   modifier abstract
   modifier static
   override
   abstract
   static
   final method
   signature
   overriding
   method
   superclass
   instance
   interface
   subclass
   default value
   overridden method
   overriding method

|

.. _Native Methods:

Native Methods
==============

.. meta:
    frontend_status: Done

Native methods are discussed in :ref:`Native Methods Experimental`.

.. index::
   native method

|

.. _Method Body:

Method Body
===========

.. meta:
    frontend_status: Done

*Method body* is a block of code that implements a method. A semicolon or
an empty body (i.e., no body at all) indicate the absence of implementation.

An abstract or native method must have an empty body.

In particular, a :index:`compile-time error` occurs if:

-  The body of an abstract or native method declaration is a block.
-  The method declaration is neither abstract nor native, but its body
   is either empty or a semicolon.

The rules that apply to return statements in a method body are discussed in
:ref:`Return Statements`.

A :index:`compile-time error` occurs if a method is declared to have a return
type, but its body can complete normally (see :ref:`Normal and Abrupt Statement Execution`).

.. index::
   method body
   semicolon
   empty body
   block
   implementation
   implementation method
   abstract method
   native method
   method declaration
   return statement
   return type
   normal completion

|

.. _Methods Returning this:

Methods Returning ``this``
==========================

.. meta:
    frontend_status: Done

A return type of an instance method can be ``this``.
It means that the return type is the class type to which the method belongs.
It is the only place where the keyword ``this`` can be used as type annotation
(see :ref:`Signatures` and :ref:`Return Type`).

The only result that is allowed to be returned from an instance method is
``this``. There are two variants how ``this`` can be returned:

-  Literally ``return this``; or
-  Return the result of any method that returns ``this``.


A call to another method can return ``this`` or ``this`` statement:

.. code-block:: typescript
   :linenos:

    class C {
        foo(): this {
            return this
        }
        bar(): this {
            return this.foo()
        }
    }

.. index::
    return type
    instance method
    class
    method signature
    signature
    this
    this statement
    subclass

The return type of an overridden method in a subclass must also be ``this``:

.. code-block:: typescript
   :linenos:

    class D extends C {
        foo(): this {
            return this
        }
    }

    let x = new C().foo() // type of 'x' is 'C'
    let y = new D().foo() // type of 'y' is 'D'

Otherwise, a :index:`compile-time error` occurs.

|

.. _Accessor Declarations:

Accessor Declarations
*********************

.. meta:
    frontend_status: Done

Accessors are often used instead of fields to add additional control for
operations of getting or setting a field value. An accessor can be either
a getter or a setter.

The syntax of *accessor declarations* is presented below:

.. code-block:: abnf

    classAccessorDeclaration:
        accessorModifier*
        ( 'get' identifier '(' ')' returnType block?
        | 'set' identifier '(' parameter ')' block?
        )
        ;

    accessorModifier:
        'abstract'
        | 'static'
        | 'final'
        | 'override'
        | 'native'
        ;

Accessor modifiers are a subset of method modifiers. The allowed accessor
modifiers have exactly the same meaning as the corresponding method modifiers
(see :ref:`Abstract Methods` for the modifier ``abstract``,
:ref:`Static Methods` for the modifier ``static``, :ref:`Final Methods` for the
modifier ``final``, :ref:`Overriding Methods` for the modifier ``override``, and
:ref:`Native Methods` for the modifier ``native``).

.. index::
   access declaration
   field
   field value
   accessor
   control
   getting
   setting
   getter
   setter
   expression
   accessor modifier
   access modifier
   method modifier
   modifier abstract
   abstract
   modifier native
   native
   modifier abstract
   abstract
   static method
   final method
   overriding method

.. code-block:: typescript
   :linenos:

    class Person {
      private _age: number = 0
      get age(): number { return this._age }
      set age(a: number) {
        if (a < 0) { throw new Error("wrong age") }
        this._age = a
      }
    }

A *get-accessor* (*getter*) must have an explicit return type but no parameters.
A *set-accessor* (*setter*) must have a single parameter and no return type. The
use of getters and setters looks the same as the use of fields.
A :index:`compile-time error` occurs if:

-  Getters or setters are used as methods;
-  *Set-accessor* (*setter*) has a single parameter that is optional (see
   :ref:`Optional Parameters`):

.. code-block:: typescript
   :linenos:

    class Person {
      private _age: number = 0
      get age(): number { return this._age }
      set age(a: number) {
        if (a < 0) { throw new Error("wrong age") }
        this._age = a
      }
    }

    let p = new Person()
    p.age = 25        // setter is called
    if (p.age > 30) { // getter is called
      // do something
    }
    p.age(17) // Compile-time error: setter is used as a method
    let x = p.age() // Compile-time error: getter is used as a method

    class X {
        set x (p?: Object) {} // Compile-time error: setter has optional parameter
    }

.. index::
   get-accessor
   getter
   parameter
   return type
   set-accessor
   setter
   field

A class can define a getter, a setter, or both with the same name.
If both a getter and a setter with a particular name are defined,
then both must have the same accessor modifiers. Otherwise, a
:index:`compile-time error` occurs.

Accessors can be implemented by using a private field or fields to store the
data (as in the example above).

.. index::
   accessor
   getter
   setter
   accessor
   private field
   accessor modifier

.. code-block:: typescript
   :linenos:

    class Person {
      name: string = ""
      surname: string = ""
      get fullName(): string {
        return this.surname + " " + this.name
      }
    }
    console.log (new Person().fullName)

A name of an accessor cannot be the same as that of a non-static field, or of a
method of class or interface. Otherwise, a :index:`compile-time error`
occurs. Moreover, a name of an accessor cannot be the same as that of another
accessor for overloading is not allowed:

.. index::
   accessor
   non-static field
   class
   method
   interface
   class method
   interface method
   overloading

.. code-block:: typescript
   :linenos:

    class Person1 {
      name: string = ""
      get name(): string { // Compile-time error: getter name clashes with the field name
          return this.name
      }
      set name(a_name: string) { // Compile-time error: setter name clashes with the field name
          this.name = a_name
      }
    }

    class Person2 {
      set name(name: string) {}
      set name(name: number) {} // Compile-time error: setters overloading is not permitted
      get name(): string {  return "A name" }
      get name(): number {  return 100 }  // Compile-time error: getters overloading is not permitted
    }

In the process of inheriting and overriding (see :ref:`Overloading and Overriding`),
accessors behave as methods. The getter parameter type follows the covariance
pattern, and the setter parameter type follows the contravariance pattern (see
:ref:`Override-Compatible Signatures`):

.. code-block:: typescript
   :linenos:

    class Base {
      get field(): Base { return new Base }
      set field(a_field: Derived) {}
    }
    class Derived extends Base {
      override get field(): Derived { return new Derived }
      override set field(a_field: Base) {}
    }
    function foo (base: Base) {
       base.field = new Derived // setter is called
       let b: Base = base.field // getter is called
    }
    foo (new Derived)

.. index::
   overriding
   inheritance
   accessor
   method
   covariance pattern
   contravariance pattern

|

.. _Constructor Declaration:

Constructor Declaration
***********************

.. meta:
    frontend_status: Partly
    todo: native constructors
    todo: Explicit Constructor Call - "Qualified superclass constructor calls" - not implemented, need more investigation (inner class)

*Constructors* are used to initialize objects that are instances of class. A
*constructor declaration* starts with the keyword ``constructor``, and has no
name. In any other syntactical aspect, a constructor declaration is similar to
a method declaration with no return type:

.. code-block:: abnf

    constructorDeclaration:
        'native'? 'constructor' parameters constructorBody?
        ;

Constructors are called by the following:

.. index::
   constructor
   initialization
   instance
   constructor declaration
   keyword constructor
   return type

-  Class instance creation expressions (see :ref:`New Expressions`);
-  Conversions and concatenations caused by the string concatenation operator
   '``+``' (see :ref:`String Concatenation`); and
-  Explicit constructor calls from other constructors (see :ref:`Constructor Body`).

Access to constructors is governed by access modifiers (see
:ref:`Access Modifiers` and :ref:`Scopes`). Declaring a constructor
inaccessible prevents class instantiation from using this constructor.
If the only constructor is declared inaccessible, then no class instance
can be created.

A :index:`compile-time error` occurs if two constructors in a class are
declared, and have identical signatures (see
:ref:`Distinguishable Declarations`).

A ``native`` constructor (an experimental feature described in
:ref:`Native Constructors`) must have no *constructorBody*. Otherwise, a
:index:`compile-time error` occurs.

A non-``native`` constructor must have *constructorBody*. Otherwise, a
:index:`compile-time error` occurs.


.. index::
   class instance
   class instantiation
   instance creation expression
   keyword constructor
   constructor declaration
   constructor call
   access modifier
   concatenation
   conversion
   access
   native constructor
   non-native constructor

|

.. _Formal Parameters:

Formal Parameters
=================

.. meta:
    frontend_status: Done

The syntax and semantics of a constructor’s formal parameters are identical
to those of a method.

.. index::
   constructor parameter

.. _Constructor Body:

Constructor Body
================

.. meta:
    frontend_status: Done

*Constructor body* is a block of code that implements a constructor.

The syntax of *constructor body* is presented below:

.. code-block:: abnf

    constructorBody:
        '{' statement* '}'
        ;

.. index::
   constructor body
   block of code
   constructor
   implementation

The constructor body must provide correct initialization of new class instances.
Constructors have two variations:

- *Primary constructor* that initializes its instance own fields [1]_ directly;

- *Secondary constructor* that uses another same-class constructor to initialize
  its instance fields.

.. index::
   constructor body
   initialization
   class instance
   primary constructor
   instance own field
   secondary constructor

The high-level sequence of a *primary constructor* body includes the following:

1. Optional arbitrary code that does not use ``this`` or ``super``.

2. Mandatory call to ``super(`` *arguments* ``)`` (see :ref:`Explicit Constructor Call`)
   if a class has an extension clause (see :ref:`Class Extension Clause`) on all
   execution paths of the constructor body.

3. Implicitly executed field initializers in the order they appear in class body

4. Optional arbitrary code.

.. index::
   primary constructor
   this
   super
   mandatory call
   constructor call
   execution path
   constructor body
   compiler-generated code
   instance own field
   circular reference
   extension clause
   compiler
   default value
   arbitrary code
   instance
   instance field
   initialization
   instance method
   field

The example below represents *primary constructors*:

.. code-block:: typescript
   :linenos:

    class Point {
      x: number
      y: number
      constructor(x: number, y: number) {
        this.x = x
        this.y = y
      }
    }

    class ColoredPoint extends Point {
      static readonly WHITE = 0
      static readonly BLACK = 1
      color: number
      constructor(x: number, y: number, color: number) {
        super(x, y) // calls base class constructor
        this.color = color
      }
    }

    class Base {
       field: string
       constructor(field: string) {
          this.field = field
       }
    }
    class Derived extends Base {
      constructor(condition: boolean) {
        console.log ("Code which does not use this")
        // zone where super() is called
        if (condition) { super ("abc") }
        else { super ("cba") }
        console.log ("Any code as this was initialized")
      }
    }


The high-level sequence of a *secondary constructor* body includes the following:

1. Optional arbitrary code that does not use ``this`` or ``super``.

2. Call to another same-class constructor ``this(`` *arguments_if_any* ``)`` on all
   execution paths of the constructor body.

3. Optional arbitrary code.

The example below represents *primary* and *secondary* constructors:

.. code-block-meta:

.. code-block:: typescript
   :linenos:

    class ColoredPoint extends Point {
      static readonly WHITE = 0
      static readonly BLACK = 1
      color: number

      // primary constructor:
      constructor(x: number, y: number, color: number) {
        super(x, y) // calls base class constructor as class has 'extends'
        this.color = color
      }
      // secondary constructor:
      constructor(color: number) {
        this(0, 0, color)
      }
    }

    class ClasWithTwoConstructors {
       field: string
       constructor(field: string) {
          this.field = field
       }
      constructor(condition: boolean) {
        console.log ("Code which does not use this or super")
        // zone where this() is called
        if (condition) { this ("abc") }
        else { this ("cba") }
        console.log ("Any code as this was initialized")
      }
    }

.. index::
   constructor body
   constructor
   secondary constructor
   arbitrary code
   this
   super
   execution path
   primary constructor
   constructor call

A :index:`compile-time error` occurs if a constructor calls itself, directly or
indirectly through a series of one or more explicit constructor calls
using ``this``.

A constructor body looks like a method body (see :ref:`Method Body`), except
for the semantics as described above. Explicit return of a value (see
:ref:`Return Statements`) is prohibited. On the opposite, a constructor body
can use a return statement without an expression.

A constructor body can have no more than one call to the current class or
direct superclass constructor. Otherwise, a :index:`compile-time error` occurs.

.. index::
   constructor
   constructor call
   constructor body
   method body
   this
   object field
   return statement
   superclass
   method body
   semantics
   compiler
   expression
   superclass constructor

|

.. _Explicit Constructor Call:

Explicit Constructor Call
=========================

.. meta:
    frontend_status: Done

There are two kinds of *explicit constructor call* statements:

-  *Alternate constructor calls* that begin with the keyword ``this``, and
   can be prefixed with explicit type arguments (see :ref:`Type Arguments`)
   (used to call an alternate same-class constructor).
-  *Superclass constructor calls* (used to call a constructor from
   the direct superclass) called *unqualified superclass constructor calls*
   that begin with the keyword ``super``, and can be prefixed with explicit
   type arguments.

A :index:`compile-time error` occurs if the constructor body of an explicit
constructor call statement:

-  Refers to any non-static field or instance method; or
-  Uses the keywords ``this`` or ``super`` in any expression.

.. index::
   explicit constructor call statement
   alternate constructor call
   constructor call
   keyword this
   superclass
   superclass constructor call
   direct superclass
   unqualified superclass constructor call
   keyword super
   prefix
   type argument
   constructor body
   constructor call
   non-static field
   instance method
   expression

An ordinary method call evaluates an alternate constructor call statement
left-to-right. The evaluation starts from arguments, proceeds to constructor,
and then the constructor is called.

The process of evaluation of a superclass constructor call statement is
performed as follows:

.. index::
   expression
   method call
   alternate constructor call
   alternate constructor call statement
   constructor
   superclass
   superclass constructor call
   superclass constructor call statement

1. If instance *i* is created, then the following procedure is used to
   determine *i*'s immediately enclosing instance with respect to ``S``
   (if available):

   -  If the declaration of ``S`` occurs in a static context, then *i* has no
      immediately enclosing instance with respect to ``S``.

      If *n* is an integer (:math:`n\geq{}1`), and *O* is the *n*’th
      lexically enclosing type declaration of ``C``, then *i*'s immediately
      enclosing instance with respect to ``S`` is the *n*’th lexically
      enclosing instance of ``this``.

.. index::
   instance
   creation
   declaration
   instance
   static context
   lexically enclosing type declaration
   immediately enclosing instance
   lexically enclosing instance
   superclass constructor call
   superclass constructor call statement
   lexically enclosing type declaration
   this

2. After *i*'s immediately enclosing instance with respect to ``S`` (if
   available) is determined, the evaluation of the superclass constructor call
   statement continues left-to-right. The arguments to the constructor are
   evaluated, and then the constructor is called.

3. If the superclass constructor call statement completes normally after all,
   then all non-static field initializers of ``C`` are executed. ``I`` is
   executed before ``J`` if a non-static field initializer *I* textually
   precedes another non-static field initializer *J*.

   An alternate constructor call does not perform the implicit execution.

.. index::
   immediately enclosing instance
   instance
   evaluation
   superclass constructor call
   superclass constructor call statement
   constructor
   non-static field initializer
   normal completion
   execution
   alternate constructor call
   alternate constructor call statement
   constructor call statement

|

.. _Default Constructor:

Default Constructor
===================

.. meta:
    frontend_status: Done

If a class contains no constructor declaration, then a default constructor
is implicitly declared. This guarantees that every class effectively has at
least one constructor. The form of a default constructor is as follows:

-  Default constructor has modifier ``public`` (see :ref:`Access Modifiers`).

-  The default constructor body contains a call to a superclass constructor
   with no arguments except the primordial class ``Object``. The default
   constructor body for the primordial class ``Object`` is empty.

A :index:`compile-time error` occurs if a default constructor is implicit, but
the superclass has no accessible constructor without parameters
(see :ref:`Accessible`).

.. index::
   class
   constructor declaration
   constructor
   modifier public
   public
   access modifier
   constructor body
   superclass constructor
   primordial class
   Object
   accessible constructor
   accessibility
   parameter

.. code-block:: typescript
   :linenos:

   // Class declarations without constructors
   class Object {}
   class Base {}
   class Derived extends Base {}

   // Class declarations with default constructors declared implicitly
   class Object {
     constructor () {} // Empty body - as there is no superclass
   }
   // Default constructors added
   class Base { constructor () { super () } }
   class Derived extends Base { constructor () { super () } }

   // Example of an error case
   class A {
       private constructor () {}
   }
   class B extends A {} // No constructor in B
   // During compilation of B
   class B extends A { constructor () { super () } } // Default constructor added
   // that leads to compile-time error as default constructor calls super()
   // which is private and inaccessible

|

.. _Inheritance:

Inheritance
***********

.. meta:
    frontend_status: Done

Class ``C`` inherits all accessible members from its direct superclass and
direct superinterfaces (see :ref:`Accessible`), and optionally overrides or
hides some of the inherited members.

An accessible member is a public, protected, or internal member in the
same package as ``C``.

If ``C`` is not abstract, then it must implement all inherited abstract methods.
The method of each inherited abstract method must be defined with
*override-compatible* signatures (see :ref:`Override-Compatible Signatures`).

Semantic checks for inherited method and accessors are described in
:ref:`Overloading and Overriding in Classes`.

Constructors from the direct superclass of ``C``  are not subject of overloading
and overriding because such constructors are not accessible (see
:ref:`Accessible`) in ``C`` directly, and can only be called from a constructor
of ``C`` (see :ref:`Constructor Body`).

If ``C`` defines a static or instance field ``F`` with the same name as that of
a field accessible from its direct superclass (see :ref:`Accessible`), then ``F``
hides the inherited field:

.. index::
   class
   inheritance
   inherited member
   accessibility
   accessible member
   direct superclass
   direct superinterface
   overriding
   overloading
   semantic check
   public member
   protected member
   internal member
   abstract method
   override-compatible signature
   constructor
   constructor body
   inherited field
   static field
   instance field

.. code-block:: typescript
   :linenos:

   interface Interface {
      foo()
   }
   class Base {
      foo() { /* Base class method body */ }
      // foo() is declared in class Base

      static foo () { /* Base class static method body */ }
   }
   class Derived extends Base implements Interface {
      override foo() { /* Derived class method body */ }
      // foo() is both
      //   - overridden in class Derived, and
      //   - implements foo() from the Interface
      static foo () { /* Derived class static method body */ }
   }

   let target: Interface = new Derived
   target.foo()  // this is a call to an instance method foo() overridden in class Derived

   Base.foo()    // this is a call to a static method foo() declared in Base
   Derived.foo() // this is a call to a static method foo() declared in Derived

.. index::
   inheritance
   direct superclass
   static method
   instance method
   method body
   interface
   overriding
   call
   static method

-------------

.. [1]
   Instance own fields here means fields declared within an instance.

.. raw:: pdf

   PageBreak
