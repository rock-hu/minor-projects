import mustache from 'mustache';

const TEMPLATE = `
/**
{{#codegenNoticeLines}}
 * {{{line}}}
{{/codegenNoticeLines}}
 */

#pragma once

{{#libraries}}
#include <react/renderer/components/{{name}}/ComponentDescriptors.h>
{{/libraries}}
#include "RNOH/Package.h"
#include "RNOH/ArkTSTurboModule.h"
{{#turboModules}}
#include "RNOH/generated/turbo_modules/{{name}}.h"
{{/turboModules}}
{{#components}}
#include "RNOH/generated/components/{{name}}JSIBinder.h"
{{/components}}

namespace rnoh {

class {{name}}TurboModuleFactoryDelegate : public TurboModuleFactoryDelegate {
  public:
    SharedTurboModule createTurboModule(Context ctx, const std::string &name) const override {
        {{#turboModules}}
        if (name == "{{name}}") {
            return std::make_shared<{{name}}>(ctx, name);
        }
        {{/turboModules}}
        return nullptr;
    };
};

class {{name}}EventEmitRequestHandler : public EventEmitRequestHandler {
  public:
    void handleEvent(Context const &ctx) override {
        auto eventEmitter = ctx.shadowViewRegistry->getEventEmitter<facebook::react::EventEmitter>(ctx.tag);
        if (eventEmitter == nullptr) {
            return;
        }

        std::vector<std::string> supportedEventNames = {
            {{#arkTSEvents}}
            "{{name}}",
            {{/arkTSEvents}}
        };
        if (std::find(supportedEventNames.begin(), supportedEventNames.end(), ctx.eventName) != supportedEventNames.end()) {
            eventEmitter->dispatchEvent(ctx.eventName, ArkJS(ctx.env).getDynamic(ctx.payload));
        }    
    }
};


class {{name}} : public Package {
  public:
    {{name}}(Package::Context ctx) : Package(ctx){};

    std::unique_ptr<TurboModuleFactoryDelegate> createTurboModuleFactoryDelegate() override {
        return std::make_unique<{{name}}TurboModuleFactoryDelegate>();
    }

    std::vector<facebook::react::ComponentDescriptorProvider> createComponentDescriptorProviders() override {
        return {
            {{#components}}
            facebook::react::concreteComponentDescriptorProvider<facebook::react::{{name}}ComponentDescriptor>(),
            {{/components}}
        };
    }

    ComponentJSIBinderByString createComponentJSIBinderByName() override {
        return {
            {{#components}}
            {"{{name}}", std::make_shared<{{name}}JSIBinder>()},
            {{/components}}
        };
    };

    EventEmitRequestHandlers createEventEmitRequestHandlers() override {
        return {
            std::make_shared<{{name}}EventEmitRequestHandler>(),
        };
    }
};

} // namespace rnoh
`;

type GeneratedPackageTurboModule = {
  name: string;
};

type GeneratedPackageComponent = {
  name: string;
  libraryCppName: string;
};

type ArkTSEvent = {
  name: string;
};

export class BasePackageHTemplate {
  private turboModules: GeneratedPackageTurboModule[] = [];
  private components: GeneratedPackageComponent[] = [];
  private arkTSEvents: ArkTSEvent[] = [];
  private libraryCppNames = new Set<string>();

  constructor(private name: string, private codegenNoticeLines: string[]) {}

  addTurboModule(turboModule: GeneratedPackageTurboModule) {
    this.turboModules.push(turboModule);
  }

  addComponent(component: GeneratedPackageComponent) {
    this.components.push(component);
    this.libraryCppNames.add(component.libraryCppName);
  }

  addArkTSEvent(arkTSEvent: ArkTSEvent) {
    this.arkTSEvents.push(arkTSEvent);
  }

  build(): string {
    return mustache.render(TEMPLATE.trimStart(), {
      codegenNoticeLines: this.codegenNoticeLines.map((line) => ({
        line,
      })),
      name: this.name,
      turboModules: this.turboModules,
      components: this.components,
      arkTSEvents: this.arkTSEvents,
      libraries: Array.from(this.libraryCppNames).map((name) => ({
        name,
      })),
    });
  }
}
