// [Start custom_plugin]
import fs from 'fs';

interface OhPackage {
  name: string;
  version: number;
  description: string;
  author: string;
}

export function renameHarTask(str?: string) {
  return {
    pluginId: 'RenameHarTaskID',
    apply(pluginContext) {
      pluginContext.registerTask({
        // Write custom tasks
        name: 'renameHarTask',
        run: (taskContext) => {
          // Read oh-package.json5 and parse the version
          const packageFile = taskContext.modulePath + '\\oh-package.json5';
          let fileContent = fs.readFileSync(packageFile, 'utf8');
          const content: OhPackage = JSON.parse(fileContent);
          const version = content.version;
          const author = content.author;
          const sourceFile =
            taskContext.modulePath + '\\build\\default\\outputs\\default\\' + taskContext.moduleName + '.har';
          const targetPath = taskContext.modulePath + '\\build\\default\\outputs\\target\\';
          const targetFile = targetPath
            + taskContext.moduleName + '-' + version + '-' + author + '.har';

          // Create Directory
          fs.mkdir(targetPath, { recursive: true }, (err) => {
            if (err) {
              throw err;
            }
            // Move and modify product file names
            fs.rename(sourceFile, targetFile, (err) => {
            });
          });
        },
        // Confirm custom task insertion position
        dependencies: ['default@PackageHar'],
        postDependencies: ['assembleHar']
      })
    }
  }
}
// [End custom_plugin]