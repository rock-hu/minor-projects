import { AbsolutePath, DescriptiveError, Dirent, FS } from '../core';
import { CliExecutor } from './CliExecutor';
import { IFs, memfs, NestedDirectoryJSON } from 'memfs';
import RawMemDirent from 'memfs/lib/Dirent';
import { Logger } from './Logger';
import chalk from 'chalk';

export class FakeCliExecutor extends CliExecutor {
  private commands: string[] = [];

  constructor(private onRun: (command: string) => string) {
    super();
  }

  run(
    command: string,
    options: {
      args?: Record<string, string | number | boolean>;
      cwd?: AbsolutePath;
    } = {}
  ): string {
    let commandWithArgs = command;
    if (options.args) {
      commandWithArgs += ' ' + this.stringifyCliArgs(options.args);
    }
    this.commands.push(commandWithArgs);
    return this.onRun(commandWithArgs);
  }

  getCommands() {
    return this.commands;
  }
}

class MemFSDirent extends Dirent {
  constructor(private rawDirent: RawMemDirent) {
    super();
  }

  isDirectory(): boolean {
    return this.rawDirent.isDirectory();
  }

  get name(): string {
    return this.rawDirent.name.toString();
  }
}
export class MemFS extends FS {
  private fs: IFs;

  constructor(initialFileStructure: NestedDirectoryJSON) {
    super();
    this.fs = memfs(initialFileStructure, './').fs;
  }

  existsSync(path: AbsolutePath): boolean {
    return this.fs.existsSync(path.toString());
  }

  readTextSync(path: AbsolutePath): string {
    return this.fs.readFileSync(path.toString()).toString();
  }

  async readText(path: AbsolutePath): Promise<string> {
    return new Promise((resolve, reject) => {
      this.fs.readFile(path.toString(), {}, (err, data) => {
        if (err) {
          reject(err);
          return;
        }
        resolve(data?.toString() ?? '');
      });
    });
  }

  writeTextSync(path: AbsolutePath, data: string): void {
    return this.fs.writeFileSync(path.toString(), data);
  }

  readDirentsSync(
    path: AbsolutePath,
    options?: { recursive?: boolean | undefined } | undefined
  ): Dirent[] {
    return this.fs
      .readdirSync(path.toString(), {
        withFileTypes: true,
        recursive: options?.recursive,
      })
      .flatMap((dataOutOrDirent) => {
        if (!(dataOutOrDirent instanceof RawMemDirent)) {
          return [];
        }
        return [new MemFSDirent(dataOutOrDirent)];
      });
  }
}

export class MockedLogger extends Logger {
  private logs: { type: 'debug' | 'info' | 'warn' | 'error'; msg: string }[] =
    [];

  debug(prepareMsg: (styles: typeof import('chalk')) => string): void {
    chalk.level = 0;
    this.logs.push({ type: 'debug', msg: prepareMsg(chalk) });
  }

  info(
    prepareMsg: (styles: typeof import('chalk')) => string,
    options?: { prefix?: boolean }
  ): void {
    chalk.level = 0;
    this.logs.push({ type: 'info', msg: prepareMsg(chalk) });
  }

  warn(prepareMsg: (styles: typeof chalk) => string): void {
    chalk.level = 0;
    this.logs.push({ type: 'warn', msg: prepareMsg(chalk) });
  }

  descriptiveError(error: DescriptiveError): void {
    chalk.level = 0;
    this.logs.push({ type: 'error', msg: error.getMessage() });
  }

  getLogs() {
    return this.logs;
  }
}