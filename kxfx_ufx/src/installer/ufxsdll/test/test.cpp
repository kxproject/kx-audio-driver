
#include "../ufxsdll.h"
#include <stdlib.h>

int main()
{
    int ret;

    ret = entry(UFXSETUP_FUNC_CHK_VERSION);
    printf("chkver: %i\n", ret);

    ret = entry(UFXSETUP_FUNC_PRE);
    printf("pre:    %i\n", ret);

    ret = entry(UFXSETUP_FUNC_DO);
    printf("do:     %i\n", ret);

    ret = entry(UFXSETUP_FUNC_POST);
    printf("post:   %i\n", ret);

    system("pause");
	return 0;
}
