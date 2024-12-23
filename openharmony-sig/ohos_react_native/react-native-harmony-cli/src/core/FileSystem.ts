import { AbsolutePath } from './AbsolutePath';
import pathUtils from 'node:path';

export abstract class Dirent {
  abstract isDirectory(): boolean;

  abstract get name(): string;
}

export abstract class FS {
  abstract writeTextSync(path: AbsolutePath, data: string): void;

  abstract readTextSync(path: AbsolutePath): string;

  abstract readText(path: AbsolutePath): Promise<string>;

  abstract existsSync(path: AbsolutePath): boolean;

  abstract readDirentsSync(
    path: AbsolutePath,
    options?:
      | {
          recursive?: boolean | undefined;
        }
      | undefined
  ): Dirent[];

  findFilePathsWithExtensions(
    dirPath: AbsolutePath,
    fileExtensions: readonly string[]
  ): AbsolutePath[] {
    let results: AbsolutePath[] = [];
    const dirents = this.readDirentsSync(dirPath);
    for (const dirent of dirents) {
      const path = dirPath.copyWithNewSegment(dirent.name.toString());
      if (dirent.isDirectory()) {
        results = results.concat(
          this.findFilePathsWithExtensions(path, fileExtensions)
        );
      } else if (
        fileExtensions.includes(
          pathUtils.extname(dirent.name.toString()).substring(1)
        )
      ) {
        results.push(path);
      }
    }
    return results;
  }
}