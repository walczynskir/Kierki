#include "pch.h"
#include "CUserCardsTests.h"
#include "CppUnitTest.h"
#include "../../Kierki/UserCards.h" // your actual header
#include "../../Kierki/TypeDefs.h" // your actual header
#include <random>
#include <algorithm>
#include <numeric>
#include <array>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace KierkiTests
{
    TEST_CLASS(CardSortTests)
    {
    public:

        TEST_METHOD(SortManualMatchesSTL_FullRange)
        {
            RunSortTest(0, 12);
        }

        TEST_METHOD(SortManualMatchesSTL_PartialRange)
        {
            RunSortTest(3, 9);
        }

        TEST_METHOD(SortManualMatchesSTL_SingleElement)
        {
            RunSortTest(5, 5);
        }

        TEST_METHOD(SortManualMatchesSTL_TailSlice)
        {
            RunSortTest(10, 12);
        }

        TEST_METHOD(SortSTLProducesSortedRange)
        {
            std::array<short, 13> values = GenerateUniqueCardValues();

            CUserCards deck;
            for (int i = 0; i < 13; ++i) {
                deck.SetCard(i, values[i], E_DL_1);
            }

            short start = 2;
            short end = 10;
            deck.SortSTL(start, end);

            for (short i = start + 1; i <= end; ++i)
            {
                short prev = deck.GetCard(i - 1).GetNr();
                short curr = deck.GetCard(i).GetNr();

                Assert::IsTrue(
                    prev < curr,
                    (L"SortSTL failed: index " + std::to_wstring(i - 1) +
                        L" has " + std::to_wstring(prev) +
                        L", but index " + std::to_wstring(i) +
                        L" has " + std::to_wstring(curr)).c_str()
                );
            }
        }

    private:

        void RunSortTest(short start, short end)
        {
            std::array<short, 13> values = GenerateUniqueCardValues();

            CUserCards manualDeck, stlDeck;
            for (int i = 0; i < 13; ++i) {
                manualDeck.SetCard(i, values[i], E_DL_1);
                stlDeck.SetCard(i, values[i], E_DL_1);
            }

            manualDeck.Sort(start, end);
            stlDeck.SortSTL(start, end);

            for (short i = start; i <= end; ++i)
            {
                Assert::AreEqual(
                    manualDeck.GetCard(i).GetNr(),
                    stlDeck.GetCard(i).GetNr(),
                    L"Mismatch at index"
                );
            }
        }

        std::array<short, 13> GenerateUniqueCardValues()
        {
            std::array<short, 52> deck;
            std::iota(deck.begin(), deck.end(), 1); // Fill with 1..52

            std::mt19937 rng{ 42 }; // Fixed seed for reproducibility
            std::shuffle(deck.begin(), deck.end(), rng);

            std::array<short, 13> hand;
            std::copy_n(deck.begin(), 13, hand.begin());
            return hand;
        }


    };
}
