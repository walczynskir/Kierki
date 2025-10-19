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


        TEST_METHOD(SortProducesSortedRange)
        {
            std::array<short, 13> values = GenerateUniqueCardValues();

            CUserCards deck;
            for (int i = 0; i < 13; ++i) {
                deck.SetCard(i, values[i], E_DL_1);
            }

            short start = 2;
            short end = 10;
            deck.Sort(start, end);

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
