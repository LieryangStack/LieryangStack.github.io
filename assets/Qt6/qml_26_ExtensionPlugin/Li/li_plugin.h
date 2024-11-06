#ifndef LI_PLUGIN_H
#define LI_PLUGIN_H

#include <QQmlExtensionPlugin>

class LiPlugin : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID QQmlExtensionInterface_iid)

public:
    void registerTypes(const char *uri) override;
};

#endif // LI_PLUGIN_H
