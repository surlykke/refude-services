#include <stdio.h>
#include <X11/Xlib.h>
#include <vector>
#include <iostream>

namespace refude
{
	template<typename T>
    std::vector<T> getProp(Display *display,
                  		Window w,
                        const char* propName,
                        bool show = false)
    {
        // Naming of these variables (and parameters above) should match whats used 
        // in XGetWindowPropterty man page  (except for '_delete' -> 'delete'). 
        // So have a look at that.
        Atom property = XInternAtom(display, propName, False);
        long long_offset = 0;
        long long_length = 32;
        Bool _delete = False;
        Atom req_type = AnyPropertyType;
        Atom actual_type_return;
        int actual_format_return;
        unsigned long nitems_return;
        unsigned long bytes_after_return;
        unsigned char* prop_return;

		std::vector<T> result;

        do {
            if (XGetWindowProperty(display, w, property, long_offset, long_length, _delete,
                                   req_type, &actual_type_return, &actual_format_return,
                                   &nitems_return, &bytes_after_return, &prop_return) != Success) {
                printf("error in getProp...\n");
				result.clear();
                break;
            }
            else {
                int bytesPrItem;
                if (actual_format_return == 32) {
                    long* tmp = reinterpret_cast<long*>(prop_return);
                    for (int i = 0; i < nitems_return; i++) {
                        result.push_back(static_cast<T>(tmp[i]));
                    }
                    long_offset += nitems_return;
                }
                else if (actual_format_return == 16) {
                    short int* tmp = reinterpret_cast<short int*>(prop_return);
                    for (int i = 0; i < nitems_return; i++) {
                        result.push_back(static_cast<T>(tmp[i]));
                    }
                    long_offset += nitems_return / 2;
                }
                else if (actual_format_return == 8) {
                    for (int i = 0; i < nitems_return; i++) {
                        result.push_back(static_cast<T>(prop_return[i]));
                    }
                    long_offset += nitems_return / 4;
                }
                XFree(prop_return);
            }
            long_length = bytes_after_return/4;
        }
        while (long_length > 0);

        return result;
    }

}
