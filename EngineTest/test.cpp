#include "gtest/gtest.h"
#include "pch.h"
#include "SaveSystem.h"
#include "Vector.h"

TEST(VectorTests, ZeroContructor)
{
    Engine::Vector2D<float> vector;
    EXPECT_EQ(vector.x, 0);
    EXPECT_EQ(vector.y, 0);
}

TEST(VectorTests, Contructor)
{
    Engine::Vector2D<float> vector(5.f, -3.f);
    EXPECT_EQ(vector.x, 5.f);
    EXPECT_EQ(vector.y, -3.f);
}

TEST(VectorTests, VectorPlusVector)
{
    Engine::Vector2D<float> first(4.f, -3.f);
    Engine::Vector2D<float> second(-4.f, 9.f);
    Engine::Vector2D<float> third = first + second;

    EXPECT_EQ(third.x, 0);
    EXPECT_EQ(third.y, 6);
}

TEST(VectorTests, VectorMinusVector)
{
    Engine::Vector2D<float> first(4.f, -3.f);
    Engine::Vector2D<float> second(-4.f, 9.f);
    Engine::Vector2D<float> third = first - second;

    EXPECT_EQ(third.x, 8);
    EXPECT_EQ(third.y, -12);
}

TEST(VectorTests, NegativeVector)
{
    Engine::Vector2D<float> first(4.f, -3.f);
    Engine::Vector2D<float> second = -first;

    EXPECT_EQ(second.x, -4);
    EXPECT_EQ(second.y, 3);
}

TEST(VectorTests, VectorMultiplyVector)
{
    Engine::Vector2D<float> first(4.f, -3.f);
    Engine::Vector2D<float> second(-4.f, 9.f);
    Engine::Vector2D<float> third = first * second;

    EXPECT_EQ(third.x, -16);
    EXPECT_EQ(third.y, -27);
}

TEST(VectorTests, ScalarMultiplyVector)
{
    Engine::Vector2D<float> first(4.f, -3.f);
    Engine::Vector2D<float> second = -1.5f * first;

    EXPECT_EQ(second.x, -6);
    EXPECT_EQ(second.y, 4.5f);
}

TEST(VectorTests, VectorMultiplyScalar)
{
    Engine::Vector2D<float> first(4.f, -3.f);
    Engine::Vector2D<float> second = first * -1.5f;

    EXPECT_EQ(second.x, -6);
    EXPECT_EQ(second.y, 4.5f);
}

TEST(VectorTests, Equal)
{
    Engine::Vector2D<float> first(4.f, -3.f);
    Engine::Vector2D<float> second(4.f, -3.f);
    Engine::Vector2D<float> third(4.f, -6.2f);

    EXPECT_TRUE(first == second);
    EXPECT_TRUE((first == third) == false);
}

TEST(VectorTests, NotEqual)
{
    Engine::Vector2D<float> first(4.f, -3.f);
    Engine::Vector2D<float> second(4.f, -3.f);
    Engine::Vector2D<float> third(4.f, -6.2f);

    EXPECT_TRUE((first != second) == false);
    EXPECT_TRUE(first != third);
}

TEST(VectorTests, DotProduct)
{
    Engine::Vector2D<float> first(1.f, -3.f);
    Engine::Vector2D<float> second(6.f, -2.f);

    EXPECT_EQ(first.DotProduct(second), 12.f);
}

TEST(VectorTests, GetLength)
{
    Engine::Vector2D<float> first(4.f, -3.f);

    EXPECT_EQ(first.GetLength(), 5.f);
}

TEST(SaveSystemTests, StoresTypedValues)
{
    Engine::SaveSystem* saveSystem = Engine::SaveSystem::Instance();
    saveSystem->Clear();

    saveSystem->SetValue("player.health", 75.0f);

    std::optional<float> health = saveSystem->GetValue<float>("player.health");
    EXPECT_TRUE(health.has_value());
    EXPECT_EQ(health.value(), 75.0f);

    saveSystem->Clear();
}

TEST(SaveSystemTests, ReturnsEmptyOptionalForWrongType)
{
    Engine::SaveSystem* saveSystem = Engine::SaveSystem::Instance();
    saveSystem->Clear();

    saveSystem->SetValue("player.health", 75.0f);

    EXPECT_FALSE(saveSystem->GetValue<int>("player.health").has_value());

    saveSystem->Clear();
}

TEST(SaveSystemTests, RemovesValues)
{
    Engine::SaveSystem* saveSystem = Engine::SaveSystem::Instance();
    saveSystem->Clear();

    saveSystem->SetValue("run.inventory", 3);
    EXPECT_TRUE(saveSystem->HasValue("run.inventory"));

    saveSystem->RemoveValue("run.inventory");
    EXPECT_FALSE(saveSystem->HasValue("run.inventory"));
}
