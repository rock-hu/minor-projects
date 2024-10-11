# DECLGEN TOOL FOR `.d.sts` FILES GENERATION

This tool generates `.d.sts` files with declarations in `sts` for given TypeScript source code files.

It

- is based on rules from the "TS to STS cookbook,
- autofixes some subset of TS code,
- outputs errors in .json format for code which is not autofixable.

## Installation

```
npm i
```

## Build

```
npm run build
```

## Run

```
npm run declgen -- <declgen options>
```

### Example

```
npm run build
npm run declgen -- -f my_file.ts
```

This generates a file `my_file.d.sts` with `sts` declarations for `my_file.ts`.

## Options

To see all available options, run the following command:

```
npm run declgen -- --help
```

```
Usage: declgen [options]

Declarations generator for sts.

Options:
  -o, --out <outDir>            Directory where to put generated declarations.
  -p, --project <tsconfigPath>  path to TS project config file
  -d, --dir <projectDir>        Directory with TS files to generate declrartions from. (default: [])
  -f, --file <fileName>         TS file to generate declarations from. (default: [])
  -h, --help                    display help for command
```

## Test

```
npm run test
```
