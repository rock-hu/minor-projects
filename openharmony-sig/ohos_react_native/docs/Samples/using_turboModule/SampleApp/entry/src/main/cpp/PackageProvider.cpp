#include "RNOH/PackageProvider.h"
#include "SampleTurboModulePackage.h"

using namespace rnoh;

std::vector<std::shared_ptr<Package>> PackageProvider::getPackages(Package::Context ctx) {
    return {
        std::make_shared<SampleTurboModulePackage>(ctx),
    };
}