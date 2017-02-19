#include "comm.h"

#include "windowresource.h"

namespace  refude
{
    WindowResource::WindowResource(Json&& action, Window window) :
        JsonResource(std::move(action)),
        window(window)
    {
    }

    void WindowResource::doPOST(Descriptor& socket, HttpMessage& request, const char* remainingPath)
    {
        std::lock_guard<std::shared_mutex> lock(mutex);
        WindowInfo(window).raiseAndFocus();
        return sendStatus(socket, HttpCode::Http204);
    }
}
