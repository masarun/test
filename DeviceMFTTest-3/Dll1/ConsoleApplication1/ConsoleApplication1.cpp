// ConsoleApplication1.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <mftransform.h>
#include "IFortune.h"

#define DEFINE_GUID(name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
        EXTERN_C const GUID DECLSPEC_SELECTANY name \
                = { l, w1, w2, { b1, b2,  b3,  b4,  b5,  b6,  b7,  b8 } }

DEFINE_GUID(CLSID_MultiPinMFT,
    0xe313280, 0x3169, 0x4f41, 0xa3, 0x29, 0x9e, 0x85, 0x41, 0x69, 0x63, 0x4f);

using namespace std;

int main()
{
    cout << "Hello World!\n";

    HRESULT hr;
    IClassFactory *pIClassFactory;
    //IFortuneTeller *pIFortuneTeller;
    IMFDeviceTransform* pIFortuneTeller;

    //BSTR bstrFortune;

    hr = CoInitialize(NULL);
    if (FAILED(hr))
    {
        cout << "CoInitialize failed\n";
        return 1;
    }

    hr = CoGetClassObject(CLSID_FortuneTeller, CLSCTX_INPROC_SERVER, NULL, IID_IClassFactory, (VOID**)&pIClassFactory);
    //hr = CoGetClassObject(CLSID_MultiPinMFT, CLSCTX_INPROC_SERVER, NULL, IID_IClassFactory, (VOID**)&pIClassFactory);
    if (FAILED(hr))
    {
        cout << "CoGetClassObject failed\n";

        CoUninitialize();
        return 1;
    }

    hr = pIClassFactory->CreateInstance(NULL, IID_IMFDeviceTransform, (VOID**)&pIFortuneTeller);
    if (FAILED(hr))
    {
        cout << "CreateInstance failed\n";

        CoUninitialize();
        return 1;
    }

    pIClassFactory->Release();
    pIClassFactory = NULL;

    /*
    hr = pIFortuneTeller->GetFortune(&bstrFortune);
    if (SUCCEEDED(hr))
    {
        cout << "GetFortune scceeded!\n";
        cout << "Fortune: ";
        wcout << bstrFortune;

        SysFreeString(bstrFortune);
    }
    */
    pIFortuneTeller->InitializeTransform(NULL);

    pIFortuneTeller->Release();
    pIFortuneTeller = NULL;

    CoUninitialize();

    return 0;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
