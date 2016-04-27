
#ifndef THEMESRESOURCE_H
#define THEMESRESOURCE_H

#include <ripc/buffer.h>
#include <ripc/abstractcachingresource.h>

namespace org_restfulipc
{

    class ThemesResource : public AbstractCachingResource 
    {
    public:
        ThemesResource();
        virtual ~ThemesResource();

    protected:
        Buffer buildContent(HttpMessage& request, map<string, string>& headers) override;
 
    private:

    };
}
#endif /* THEMESRESOURCE_H */

