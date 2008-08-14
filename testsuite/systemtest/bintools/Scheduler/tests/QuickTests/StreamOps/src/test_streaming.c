#include "tceops.h"

int main()
{
    char byte;
    int status;

    while (1)
    {
        _TCE_STREAM_IN_STATUS(0, status);
    
        if (status == 0)
            break;

        _TCE_STREAM_IN(0, byte);
        _TCE_STREAM_OUT(byte);
    }

    return 0;
}
