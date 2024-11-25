# Design documentation for compilation units, packages, and modules

## 1. Introduction

Programs are structured as sequences of elements ready for compilation, i.e., compilation units. Each compilation unit creates its own scope.
The compilation unit’s variables, functions, classes, interfaces, or other declarations are only accessible  within such scope if not explicitly exported.

A variable, function, class, interface, or other declarations exported from a different compilation unit must be imported first.

There are three kinds of compilation units:
   - Separate modules,
   - Declaration modules,
   - Packages

## 2. Objectives

The primary objectives of this design are:
- Give a general picture of how compilation unit handling works within the compiler.
- The current code structure needs to be refactored in order to follow the changes of the standard amendments, which allow other implementation approaches, and on the other hand, several hot fixes has been merged into the code base recently, which made the code quite kludgy and unclear.
- There are missing features that are not implemented such as the internal access modifier, single export directive or even the requirement that a package module can directly access all top-level entities declared in all modules that constitute the packages.

## 3. Module Handling

A separate module is a module without a package header. A separate module can optionally consist of the following
four parts:
1. Import directives that enable referring imported declarations in a module
2. Top-level declarations
3. Top-level statements
4. Re-export directive

Every module implicitly imports all exported entities from essential kernel packages of the standard library.
All entities from these packages are accessible as simple names, like the console variable.

```
// Hello, world! module
function main() {
  console.log("Hello, world!")
}
```

It is ensured currently via the *ETSParser::ParseDefaultSources* method, which parse an internally created ets file  named "<default_import>.sts".

### 3.1. Package level scope

Name declared on the package level should be accessible throughout the entire package. The name can be accessed in other packages or modules if exported.
Currently there is no package level scope, packages are handled almost exactly the same as separate modules, so the name declared on the module level is accessible
 throughout the entire module only, and only if it exported can be accessed in other modules/compilation units, including another package module belonging to the same package (compilation unit).

[#17665]

## 4. How variables are stored

All declared variables in source files are inserted into scopes - during parsing the source code. Each scope (local scope, param scope, global scope, ...) has a **bindings_** named field, that stores the identifier, and a pointer to the created Variable objects.

 * map<string, Variable*> bindings_

Each scope has a pointer to its parent scope. When a variable is referred in source code, the actual scope is checked. If the variable is not found in the actual scope, its parent scope is investigated, and so on. Search continues until the global scope is reached, which is the end of scope chain.

### 4.1. Storage of imported variables

Both import and export directives are only allowed in top-level. It means, that all the export marked variables are placed into the global scope of the actual program. Similarly, all the variables are stored in the global scope of the actual program that are imported.

According to this, global scope has an extra **foreignBindings_** member that helps to know which variables are defined locally and imported from external sources.

  * map<string, Variable*> bindings_
  * map<string, boolean> foreignBindings_

So, all the variables in global scope are inserted into **bindings_**, and also placed into the **foreignBindings_** as well.

### 4.2. How to import variables

When importing variables from an external source, only the global scope of the external source is checked for the variables. Only those variables can be imported, that are not foreign (name of the variable is in `foreignBindings_` with `false` value) and marked for export.
This **foreignBinding_** member helps to eliminate "export waterfall" that is demonstrated by the following example: 


```
// C.sts
export let a = 2
```

```b.sts
// B.sts
import { a } from c.sts  // 'a' marked as foreign
```

```
// A.sts
import { a } from b.sts
// 'a' is not visible here, since 'a' is foreign in B.sts
```

### 4.3. Improvement possibilities/suggestions

Foreign marker can be improved. Since **Variable** objects are stored by pointers, it is not a solution to remove 'export' flag when they are imported. Deep copy of **Variable** objects is a solution, to remove 'export' flag when variables are imported, but it could significantly increase memory consumption.

Currently, all variables - locally defined or imported ones - are stored in `foreignBindings_`. It could be enough if just imported ones are placed into that structure.


## 5. Import directives

Import directives make entities exported from other compilation units available for use in the current compilation unit by using different binding forms.

An import declaration has the following two parts:
* Import path that determines a compilation unit to import from;
* Import binding that defines what entities, and in what form—qualified or unqualified—can be used by the current compilation unit.

Import directive use the following form:
```'import' allBinding|selectiveBindings|defaultBinding|typeBinding 'from' importPath;```

## 5.1. Resolving importPath

The *importPath* is a string literal which can be points to a module (separate module | package module) or a folder (package folder, or a folder which contains an index.ts/index.sts file). It can be specified with a relative or an absolute path, with or without a file extension, and must be able to manage the paths entered in arktsconfig as well.  
Resolving these paths within the compiler is the responsibility of the *importPathManager*. In the process of parsing an import path, the string literal will be passed to the importPathManager which will resolve it as an absolute path and adds it to an own list, called *parseList_.*  
The latter list serves to let the compiler know what still needs to be parsed (handle and avoid duplications), and this list will be requested and traversed during the *ParseSources* call. The importPathManager also handles errors that can be caught before parsing, for example non-existent, incorrectly specified import paths, but not errors that can only be found after parsing (for example, the package folder should contains only package files that use the same package directive, etc.)  

The importPath with the resolved path and two additional information - which is the language information and whether the imported element has a declaration or not - , will be stored in an ImportSource instance. The latter two information can be set under the dynamicPaths tag in arktsconfig.json, otherwise they will be assigned a default value (the lang member will be specified from the extension, hasDecl member will be true). This instance will be passed as a parameter during the allocation of the *ETSImportDeclaration* AST node, as well as the specifiers list resolved from the binding forms explained in the next section and the import kind (type or value).

## 5.2. Handle binding forms (allBinding|selectiveBindings|defaultBinding|typeBinding)

The import specifier list will be filled until an import directive can be found, which may contain the following binding forms:

### 5.2.1. allBinding: '*' importAlias

```import * as N from "./test"```

 It is mandatory to add importAlias, but there is a temporary exception due to stdlib sources, which will have to be handled later and eliminate from the current implementation.  
The name of a compilation unit will be introduced as a result of import * as N where N is an identifier. In this case, it will be parsed by the *ParseNameSpaceSpecifier* (outdated/deprecated name, left from an old version of the standard). The allocated ImportNamespaceSpecifier AST node will be created here with the imported token, and that will be added to the specifier list.

### 5.2.2. selectiveBindings: '{' importBinding (',' importBinding)* '}'

The same bound entities can use several import bindings. The same bound entities can use one import directive, or several import directives with the same import path.  
```import {sin as Sine, PI} from "..."```

The *ParseNamedSpecifiers* method will create the *ImportSpecifier* AST Node with local name, which in case of import alias it will be a different identifier than the imported token. This specifier will be added to the specifier list.

### 5.2.3 defaultBinding: Identifier | ( '{' 'default' 'as' Identifier '}' );

Default import binding allows importing a declaration exported from some module as default export. Knowing the actual name of the declaration is not required as the new name is given at importing. A compile-time error occurs if another form of import is used to import an entity initially exported as default. As for now the compiler only support the following default import syntax:  
```import ident from "...""```  
but not the  
```import { default as ident} from "..." "```  
The latest one recently added to the standard (#17739)

The *ParseImportDefaultSpecifier* will create an *ImportDefaultSpecifier* AST node with the imported identifier member, and it will be added to the specifiers list.

### 5.2.4. typeBinding: 'type' selectiveBindings;

```import type {A} from "..."```

The difference between import and import type is that the first form imports all top-level declarations which were exported, and the second imports only exported types. There are two possible import kind that can be set to the *ETSImportDeclaration* AST node:
* ir::ImportKinds::TYPE
* ir::ImportKinds::VALUE;

The *ParseImportDeclarations* method itself will set the importKind member if it will met a type keyword token during the parsing process of import directive.

## 5.3. Build ETSImportDeclaration nodes

The *ETSImportDeclaration* nodes will be added to the statements list.

The binder component will build/validate all the scope bindings. It's not a standalone analysis; each binder validation is triggered during the parsing process. Currently, the triggers can include:
* Create a new scope
* Add a declaration for the current scope. If the current scope cannot accept the binding due to the scoping rules, a SyntaxError `es2panda::Error` is raised.

So the binder's BuildImportDeclaration method will be called for every *ETSImportDeclaration* nodes which will import these foreign bindings specified in the specifiers and insert it to the global scope's variable map, called *bindings_*.

## 6. Exported declarations and export directives

Top-level declarations can use export modifiers that make the declarations accessible in other compilation units by using import.
The declarations not marked as exported can be used only inside the compilation unit they are declared in.

In addition, only one top-level declaration can be exported by using the default export scheme. It allows specifying no declared name when importing.
A compile-time error occurs if more than one top-level declaration is marked as default.

The export directive allows the following:
* Specifying a selective list of exported declarations with optional renaming; or
* Specifying a name of one declaration; or
* Re-exporting declarations from other compilation units; or
* Export type

One important difference that stands out looking the parser sections  is that unlike import declarations, export declarations are not included in the dumped AST as a separate node, except for reexport declarations. This is a shortcoming that would be useful to address, but requires a major overhaul, which could be part of the rearchitecting phase.

There are compile time checks for the following:
* Exporting one program element more than once (like, exported as a type and also as a default export)
* Clashing names, because two program elements cannot be exported on the same name (could occur when aliasing)
* Trying to type export something, which is not a type

In case of handling some variants of the export (like selective and default export) the compiler extensively use an object called Varbinder
* It is a persistent object during the entire process of parsing, the lowering phases and checking
* The binder is shared among all parsed program files, making it suitable for checks, such as those used in default exports.

### 6.1. Exported declarations

Top-level declarations that can use export modifiers

```
export class TestClass {}

export function foo(): void {}

export let msg = "hello"
```

In nutshell, when a top-level declaration is exported, the following happens:
* The definition is stored in the global scope of the given file
* It gets a specific *ModifierFlag* after the export is parsed, the flag depends on the type of the export
* After this, it can be imported by another file using the specific import syntax
* The file doing the import stores its own definitions and the ones it imports, separately

### 6.2. Default export

Default import binding allows importing a declaration exported from some module as default export

```
//export.sts
export default class TestClass {}

//import.sts
import ImportedClass from "./export"
```

The basic rules of a default export and import are the following:
* Only one default export can exist per module, because it has a dedicated syntax
* When importing a default exported program element, any name can be used, as long as it does not clash with other names
* The original name of the default export can also be used when importing, but it is not necessary:

A brief description of what happens when the compiler is running:
* The *default* modifier itself is parsed in the *ETSParser::ParseMemberModifiers* method
* The exported declaration itself is parsed in the *ETSParser::ParseTopLevelDeclStatement* method
* It gets the *ModifierFlags::DEFAULT_EXPORT* flag
* As mentioned, only one default export is allowed per module and since every parsed program uses the same binder it is stored there as a member
* This member of the binder is set in the *InitScopesPhaseETS::ParseGlobalClass* method, where an error is thrown if more than one default export exists

### 6.3. Single export

Single export directive allows to specify the declaration which will be exported from the current compilation unit using its name.
```
export v
let v = 1
```

### 6.4. Selective export

Each top-level declaration can be marked as exported by explicitly listing the names of exported declarations. An export list directive uses the same syntax as an import directive with selective bindings:

```
function foo(): void {}

export {foo}
```

Renaming is optional:

```
function foo(): void {}

export {foo as test_func}
```

The selective export directive is parsed in the *ETSParser::ParseExport* method, it will calls the *ETSParser::ParseNamedSpecifiers* method to parse the export specifiers, which are the selective bindings after the *export* keyword:
```
export {a, b, c}
```

At this point these are only identifiers stored in an *ir::Identifier* node for each name.

Most part of the selective exports are handled in the lowering phase of top level statements via the  *ImportExportDecls* AST visitor

A brief description of how it works currently:
* In the first step a map named *fieldMap_* is populated with every definion in the parsed file while via visitor methods, like *ImportExportDecls::VisitFunctionDeclaration* and *ImportExportDecls::VisitVariableDeclaration*
* Another visitor method called *ImportExportDecls::VisitExportNamedDeclaration* stores the selective exported names in another map called *exportNameMap_*
* The logic is handled in the *ImportExportDecls::HandleGlobalStmts*, which contains checks and throws errors, if necessary

    * This method loops through the *exportnameMap_* map and searches for the given names in the *fieldMap_*

        * If something is found, it gets the *ModifierFlags::EXPORT* flag (it only handles the original names and ignores aliases)
        * Currently, this solution does not support aliasing and only works with function and variable declarations

As it has been mentioned above, renaming inside selective export is still under development. Its process is currently as follows, and this is what is expected:
* The logic itself is still handled in the same AST visitor, called *ImportExportDecls*
* A multimap is introduced to handle aliasing, which is stored in the binder for every parsed file
* It is populated in the *ImportExportDecls::VisitExportNamedDeclaration* method next to the other map named *exportNameMap_*

* As for the functionalities:

    * It will support renaming, as expected
    * It will checks for clashing names, when the given alias is a name, which is also exported, like:
    
        ```
        export function foo(): void {}
        let tmp_var = "hello"
        export {tmp_var as foo}
        ```

    * It also checks for clashing names between the different types of exports, like export type, default export and export declaration.
    * A compile time error will be thrown, if something is being imported using its original name, but it was exported with an alias
    * Similar behaviour will occur if something is being referred by its original name after being imported with a namespace import:
        ```
        //export.sts
        function test_func(): void {}

        export {test_func as foo}

        //import.sts
        import * as all from "./export
        all.test_func() //A compile time error will be thrown at this point, since 'test_func' has an alias 'foo'
        ```
    * Support for exporting an imported program element will also be added at some point:
        ```
        //export.sts
        export function foo(): void {}
        //export_imported.sts
        import {foo} from "./export"

        export {foo}
        ```
     * This would also support aliasing:
        ```
        import {foo} from "./export"

        export {foo as bar}
        ```

### 6.5. Re-export

In addition to exporting what is declared in the module, it is possible to re-export declarations that are part of other module's export. Only limited re-export possibilities are currently supported. It is possible to re-export a particular declaration or all declarations from a module.

Syntax:
```
reExportDirective:
'export' ('*' | selectiveBindings) 'from' importPath;
```

    ```
    //export.sts
    export function foo(): void {}

    //re-export.sts
    export {foo} from "./export"
    ```

When re-exporting, new names can be given. This action is similar to importing but with the opposite direction.

```
export {foo as f} from "./export"
```


A brief description how it works:

 * The specifiers will be parsed in the *ETSParser::ParseExport* method, which calls the *ETSParser::ParseNamedSpecifiers* method just like in case of selective export directives or the *ETSParser::ParseNamespaceSpecifiers* method.
 * It will resolve the importPath in the same way as in the case of import declarations.
 * Instead of storing this specifiers in an *ExportNamedDeclaration* AST node, it will create an *ETSImportDeclaration* node passing the specifiers and the resolved path.
 * An *ETSReExportDeclaration* AST node will be created, which will contains this *ETSImportDeclaration* node and the current program path which contains the reexport directive.
 * So the binder's BuildImportDeclaration method will be called for every ETSImportDeclaration nodes which will import these foreign bindings specified in the specifiers and insert it to the appropriate global scope's variable map, retrieving the *ETSReExportDeclaration* nodes, making sure that the specifiers are not available in the place where the reexport directive is. 
 * In case of '*',  ```import * as A from '...'```, the compiler create for re-exports an **ETSObjectType** each. If there is a call to a member within *A*, and cannot find it, the search will be extended with *SearchReExportsType* function that finds the right 'ETSObjectType' if it exists.
 
**Improvement possibilities**

Currently, re-export is a bit tweaked. See the following example, that is a typical usage of re-export:

```
// C.sts
export let a = 3

// B.sts
export {a} from "./C.sts"

// A.sts
import {a} from "./B.sts"
```

On a very high level view, the engine copies the path of C.sts (that is defined in B.sts for re-export", and creates a direct import call to C.sts from A.sts. The following code symbolizes it:

```
// C.sts
export let a = 3

// B.sts
export {a} from "./C.sts"

// A.sts
import {a} from "./C.sts"  <---- here, a direct import call is executed to C.sts, that works, but not correct.
```

Instead, B.sts should import all variables from C.sts and all re-exported variables should be stored in a separated re-exported-variables variable map in B. These variables are not visible in B.sts (for usage) since they are treated for re-export only. After that, A.sts can import all exported variables from B.sts including re-exported-variables.


### 6.6. Type exports

In addition to export that is attached to some declaration, a programmer can use the export type directive in order to do
the following:

* Export as a type a particular class or interface already declared; or
* Export an already declared type under a different name.

   ```
   class TestClass {}

   export type {TestClass}
   ```


The export type directive supports renaming.

   ```
   export type {TestClass as tc}
   ```


Since the export type directive only support exporting types, a compile time error will be thrown if anything else is exported in such way:

    ```
    let msg = "hello"
    function foo(): void {}

    export type {msg} //a CTE will be thrown here, since msg is not a type but a variable
    export type {foo}
    ```

A brief description about how it works currently:

* The keyword *type* is parsed in the *ETSParser::ParseMemberModifiers* method
* The export type directive uses the selective export syntax, which means it is parsed in the *ETSParser::ParseExport* method, which calls the *ETSParser::ParseNamedSpecifiers* method later to parse the specifiers
* Just like the selective export, it uses the *ImportExportDecls* visitor to handle its logic:

    * The checking of the exported types is handled in the *ImportExportDecls::VerifyTypeExports* and *ImportExportDecls::HandleSimpleType* methods
    * Compile time errors are thrown if the program element to be exported is not a type
    * Similarly, an error is thrown if something is being exported twice, for example once as a type and once as an export declaration
