#include "pch.h"
#include "CppUnitTest.h"
#include "../../Kierki/FontFactory.h"
#include <rcommon/ROwnExc.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace KierkiTests
{
    TEST_CLASS(CFontFactoryTests)
    {
    public:

        TEST_METHOD(ThrowsIfNotInitialized)
        {
            // Reset internal state (if needed)
            CFontFactory::SetFontNames(_T(""), _T(""));

            try {
                CFontFactory& factory = CFontFactory::Instance();
                Assert::Fail(L"Expected exception not thrown");
            }
            catch (const ROwnExc&) {
                // Expected
            }
        }

        TEST_METHOD(InitializesSuccessfully)
        {
            CFontFactory::SetFontNames(_T("Segoe UI"), _T("Comic Sans MS"));
            CFontFactory& factory = CFontFactory::Instance();
            Assert::IsTrue(factory.Initialized());
        }

        TEST_METHOD(FontStyleSwitchAffectsFaceName)
        {
            CFontFactory::SetFontNames(_T("Segoe UI"), _T("Comic Sans MS"));
            CFontFactory& factory = CFontFactory::Instance();

            factory.SetFontStyle(FontStyle::Normal);
            Assert::AreEqual(_T("Segoe UI"), factory.GetFaceNameForTest().c_str());

            factory.SetFontStyle(FontStyle::Fancy);
            Assert::AreEqual(_T("Comic Sans MS"), factory.GetFaceNameForTest().c_str());
        }

        TEST_METHOD(MakeKeyProducesConsistentOutput)
        {
            CFontFactory::SetFontNames(_T("Arial"), _T("Times New Roman"));
            CFontFactory& factory = CFontFactory::Instance();
            factory.SetFontStyle(FontStyle::Fancy);

            tstring key = factory.MakeKeyForTest(96, -12, FW_BOLD);
            Assert::IsTrue(key.find(_T("Times New Roman")) != tstring::npos);
            Assert::IsTrue(key.find(_T("96dpi")) != tstring::npos);
            Assert::IsTrue(key.find(_T("12pt")) != tstring::npos);
        }

        TEST_METHOD(FontIsCachedAfterFirstCall)
        {
            CFontFactory::SetFontNames(_T("Arial"), _T("Times New Roman"));
            CFontFactory& factory = CFontFactory::Instance();

            HWND hwnd = GetDesktopWindow();
            HFONT font1 = factory.GetFont(hwnd, 10, FW_NORMAL);
            HFONT font2 = factory.GetFont(hwnd, 10, FW_NORMAL);

            Assert::IsTrue(font1 == font2); // Should be cached
        }

        TEST_METHOD(DpiCalculationIsConsistent)
        {
            CFontFactory::SetFontNames(_T("Arial"), _T("Times New Roman"));
            CFontFactory& factory = CFontFactory::Instance();

            HWND hwnd = GetDesktopWindow();
            int dpi = factory.GetDpiForWindowSafeForTest(hwnd);
            Assert::IsTrue(dpi >= 96); // Typical DPI range
        }

        TEST_METHOD(ClearCacheRemovesFonts)
        {
            CFontFactory::SetFontNames(_T("Arial"), _T("Times New Roman"));
            CFontFactory& factory = CFontFactory::Instance();

            HWND hwnd = GetDesktopWindow();
            HFONT fontBefore = factory.GetFont(hwnd, 10, FW_NORMAL);
            factory.ClearCache();
            HFONT fontAfter = factory.GetFont(hwnd, 10, FW_NORMAL);

            Assert::IsTrue(fontBefore != fontAfter); // New font created
        }
    };
}
