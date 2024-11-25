import mustache from 'mustache';

const TEMPLATE = `
/**
{{#codegenNoticeLines}}
 * {{{line}}}
{{/codegenNoticeLines}}
 */

{{#reexports}}
export * {{as}}from "{{{from}}}"
{{/reexports}}
`;

const TEMPLATE_EMPTY = `
/**
{{#codegenNoticeLines}}
 * {{{line}}}
{{/codegenNoticeLines}}
 */

export {}
`;

type Reexport = {
  from: string;
  as?: string;
};

export class IndexTSTemplate {
  private reexports: Reexport[] = [];

  constructor(private codegenNoticeLines: string[]) {}

  addReexport(reexport: Reexport) {
    this.reexports.push(reexport);
    return this;
  }

  build(): string {
    if (this.reexports.length === 0) {
      return mustache.render(TEMPLATE_EMPTY, {});
    }

    return mustache.render(TEMPLATE.trimStart(), {
      codegenNoticeLines: this.codegenNoticeLines.map((line) => ({
        line,
      })),
      reexports: this.reexports.map((reexport) => ({
        from: reexport.from,
        as: reexport.as ? `as ${reexport.as} ` : '',
      })),
    });
  }
}
