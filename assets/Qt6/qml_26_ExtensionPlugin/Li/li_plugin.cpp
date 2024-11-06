#include "li_plugin.h"

#include "myitem.h"

#include <qqml.h>

void LiPlugin::registerTypes(const char *uri)
{
    // @uri Li
    qmlRegisterType<MyItem>(uri, 1, 0, "MyItem");
}

