#include "pch.h"
#include "CppUnitTest.h"
#include "../../Kierki/typedefs.h"
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#pragma once
#include <string>
#include <CppUnitTest.h>

namespace Microsoft::VisualStudio::CppUnitTestFramework
{
    // Player
    template<> inline std::wstring ToString<Player>(const Player& value)
    {
        switch (value)
        {
        case E_DL_1: return L"E_DL_1";
        case E_DL_2: return L"E_DL_2";
        case E_DL_3: return L"E_DL_3";
        case E_DL_4: return L"E_DL_4";
        case E_DL_NULL: return L"E_DL_NULL";
        default: return L"Unknown Player";
        }
    }

    // Serie
    template<> inline std::wstring ToString<Serie>(const Serie& value)
    {
        switch (value)
        {
        case E_SR_1: return L"E_SR_1";
        case E_SR_2: return L"E_SR_2";
        case E_SR_3: return L"E_SR_3";
        case E_SR_4: return L"E_SR_4";
        case E_SR_NULL: return L"E_SR_NULL";
        default: return L"Unknown Serie";
        }
    }

    // GameType
    template<> inline std::wstring ToString<GameType>(const GameType& value)
    {
        switch (value)
        {
        case E_GM_NOTRICKS: return L"E_GM_NOTRICKS";
        case E_GM_NOHEARTS: return L"E_GM_NOHEARTS";
        case E_GM_NOBOYS: return L"E_GM_NOBOYS";
        case E_GM_NOLADIES: return L"E_GM_NOLADIES";
        case E_GM_NOKINGOFHEART: return L"E_GM_NOKINGOFHEART";
        case E_GM_NOSIXANDLAST: return L"E_GM_NOSIXANDLAST";
        case E_GM_ROBBER: return L"E_GM_ROBBER";
        case E_GM_RECOVER1: return L"E_GM_RECOVER1";
        case E_GM_RECOVER2: return L"E_GM_RECOVER2";
        case E_GM_RECOVER3: return L"E_GM_RECOVER3";
        case E_GM_RECOVER4: return L"E_GM_RECOVER4";
        case E_GM_PUZZLE: return L"E_GM_PUZZLE";
        case E_GM_NULL: return L"E_GM_NULL";
        default: return L"Unknown GameType";
        }
    }
}



namespace CircularEnumTests
{
    TEST_CLASS(PlayerCycleTests)
    {
    public:
        TEST_METHOD(NextPlayerCycle)
        {
            Assert::AreEqual(E_DL_2, PlayerCycle::Next(E_DL_1));
            Assert::AreEqual(E_DL_3, PlayerCycle::Next(E_DL_2));
            Assert::AreEqual(E_DL_4, PlayerCycle::Next(E_DL_3));
            Assert::AreEqual(E_DL_1, PlayerCycle::Next(E_DL_4)); // wraparound
            Assert::AreEqual(E_DL_1, PlayerCycle::Next(E_DL_NULL)); // sentinel
        }

        TEST_METHOD(PrevPlayerCycle)
        {
            Assert::AreEqual(E_DL_4, PlayerCycle::Prev(E_DL_1)); // wraparound
            Assert::AreEqual(E_DL_1, PlayerCycle::Prev(E_DL_2));
            Assert::AreEqual(E_DL_2, PlayerCycle::Prev(E_DL_3));
            Assert::AreEqual(E_DL_3, PlayerCycle::Prev(E_DL_4));
            Assert::AreEqual(E_DL_4, PlayerCycle::Prev(E_DL_NULL)); // sentinel
        }
    };

    TEST_CLASS(SerieCycleTests)
    {
    public:
        TEST_METHOD(NextSerieCycle)
        {
            Assert::AreEqual(E_SR_2, SerieCycle::Next(E_SR_1));
            Assert::AreEqual(E_SR_3, SerieCycle::Next(E_SR_2));
            Assert::AreEqual(E_SR_4, SerieCycle::Next(E_SR_3));
            Assert::AreEqual(E_SR_1, SerieCycle::Next(E_SR_4)); // wraparound
            Assert::AreEqual(E_SR_1, SerieCycle::Next(E_SR_NULL)); // sentinel
        }

        TEST_METHOD(PrevSerieCycle)
        {
            Assert::AreEqual(E_SR_4, SerieCycle::Prev(E_SR_1)); // wraparound
            Assert::AreEqual(E_SR_1, SerieCycle::Prev(E_SR_2));
            Assert::AreEqual(E_SR_2, SerieCycle::Prev(E_SR_3));
            Assert::AreEqual(E_SR_3, SerieCycle::Prev(E_SR_4));
            Assert::AreEqual(E_SR_4, SerieCycle::Prev(E_SR_NULL)); // sentinel
        }
    };

    TEST_CLASS(GameCycleTests)
    {
    public:
        TEST_METHOD(NextGameCycle)
        {
            Assert::AreEqual(E_GM_NOHEARTS, GameCycle::Next(E_GM_NOTRICKS));
            Assert::AreEqual(E_GM_PUZZLE, GameCycle::Next(static_cast<GameType>(E_GM_PUZZLE - 1)));
            Assert::AreEqual(E_GM_NOTRICKS, GameCycle::Next(E_GM_PUZZLE)); // wraparound
            Assert::AreEqual(E_GM_NOTRICKS, GameCycle::Next(E_GM_NULL)); // sentinel
        }

        TEST_METHOD(PrevGameCycle)
        {
            Assert::AreEqual(E_GM_PUZZLE, GameCycle::Prev(E_GM_NOTRICKS)); // wraparound
            Assert::AreEqual(static_cast<GameType>(E_GM_PUZZLE - 1), GameCycle::Prev(E_GM_PUZZLE));
            Assert::AreEqual(E_GM_PUZZLE, GameCycle::Prev(E_GM_NULL)); // sentinel
        }
    };
}
