// [Start practical_case_two]
import { webview } from '@kit.ArkWeb';
import { hiTraceMeter } from '@kit.PerformanceAnalysisKit';

interface Config {
  url: string,
  localPath: string, // local resource path
  options: webview.CacheOptions
}

@Entry
@Component
struct Index {
  controller: webview.WebviewController = new webview.WebviewController();
  // Configure precompiled JavaScript file information
  configs: Array<Config> = [
    {
      url: 'https://www.example.com/example.js',
      localPath: 'example.js',
      options: {
        responseHeaders: [
          { headerKey: 'E-Tag', headerValue: 'xxx' },
          { headerKey: 'Last-Modified', headerValue: 'Web, 21 Mar 2024 10:38:41 GMT' }
        ]
      }
    }
  ]

  // [StartExclude practical_case_two]
  build() {
    Column() {
      // [Start web_attach_controller]
      Web({ src: 'https://www.example.com/a.html', controller: this.controller })
        .onControllerAttached(async () => {
          // Read the configuration and precompile.
          for (const config of this.configs) {
            let content = await this.getUIContext().getHostContext()?.resourceManager.getRawFileContentSync(config.localPath);

            try {
              this.controller.precompileJavaScript(config.url, content, config.options)
                .then((errCode: number) => {
                  console.log('precompile successfully!' );
                }).catch((errCode: number) => {
                console.error('precompile failed.' + errCode);
              })
            } catch (err) {
              console.error('precompile failed!.' + err.code + err.message);
            }
          }
        })
      // [End web_attach_controller]
    }
  }
  // [EndExclude practical_case_two]
}
// [End practical_case_two]
