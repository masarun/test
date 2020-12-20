#pragma once

#include <initguid.h>
#include <combaseapi.h>

// {04AC1AAF-F6B9-4F15-A5C5-0BE72221B779}
DEFINE_GUID(CLSID_FortuneTeller,
    0x4ac1aaf, 0xf6b9, 0x4f15, 0xa5, 0xc5, 0xb, 0xe7, 0x22, 0x21, 0xb7, 0x79);

// {7100933F-F990-443B-86E3-87B93C02ACFD}
DEFINE_GUID(IID_IFortuneTeller,
    0x7100933f, 0xf990, 0x443b, 0x86, 0xe3, 0x87, 0xb9, 0x3c, 0x2, 0xac, 0xfd);


// {82335C9D-5EE2-4D4A-8ADE-5511255287CD}
DEFINE_GUID(CLSID_DeviceMFTTest,
    0x82335c9d, 0x5ee2, 0x4d4a, 0x8a, 0xde, 0x55, 0x11, 0x25, 0x52, 0x87, 0xcd);



#define GUID_Hoge L"{04AC1AAF-F6B9-4F15-A5C5-0BE72221B779}"

#define GUID_DeviceMFTTest L"{82335C9D-5EE2-4D4A-8ADE-5511255287CD}"
