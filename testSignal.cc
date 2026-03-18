#include "Signal.h"

#include <gtest/gtest.h>

/* Tests for Signal class */

TEST(SignalTest, idGeneration) {
  sig::Signal<int(int)> sig;
  const size_t id = sig.connectSlot([](const int x) { return x; });
  const size_t id2 = sig.connectSlot([](const int x) { return x; });

  EXPECT_TRUE(id != id2);
}
TEST(SignalTest, slotCalled) {
  sig::Signal<int(int)> sig;
  int acc = 0;
  sig.connectSlot([&acc](const int x) { acc += x; return x; });

  sig.emitSignal(3);

  EXPECT_EQ(3, acc);
}
TEST(SignalTest, multipleSlotsCalled) {
  sig::Signal<int(int)> sig;
  int acc = 0;
  sig.connectSlot([&acc](const int x) { acc += x; return x; });
  sig.connectSlot([&acc](const int x) { acc += x; return x; });
  sig.connectSlot([&acc](const int x) { acc += x; return x; });

  sig.emitSignal(3);

  EXPECT_EQ(9, acc);
}
TEST(SignalTest, slotDeletion) {
  sig::Signal<int(int)> sig;
  int count = 0;
  const size_t id = sig.connectSlot([&count](const int x) { count++; return x; });
  sig.disconnectSlot(id);

  sig.emitSignal(1);

  EXPECT_EQ(0, count);
}
TEST(SignalTest, noSlots) {
  sig::Signal<int(int)> sig;
  EXPECT_NO_THROW(sig.emitSignal(1));
}
TEST(SignalTest, connectDisconnectEmit) {
  sig::Signal<int(int)> sig;

  const size_t id = sig.connectSlot([](const int x) { return x; });
  sig.disconnectSlot(id);

  EXPECT_NO_THROW(sig.emitSignal(1));
}
TEST(SignalTest, nonExistentID) {
  sig::Signal<int(int)> sig;

  EXPECT_NO_THROW(sig.disconnectSlot(1));
}
TEST(SignalTest, doubleDisconnect) {
  sig::Signal<int(int)> sig;
  const size_t id = sig.connectSlot([](int x) { return x; });
  sig.disconnectSlot(id);
  EXPECT_NO_THROW(sig.disconnectSlot(id));
}

/* Tests for DiscardCombiner */

TEST(DiscardCombinerTest, returnVoid) {
  sig::Signal<int(int)> sig;

  using ResultType = decltype(sig.emitSignal(1));

  EXPECT_TRUE((std::is_same_v<ResultType, void>));
}

/* Tests for LastCombiner */

TEST(LastCombinerTest, noSlots) {
  sig::Signal<int(int)> sig;
  EXPECT_NO_THROW(sig.emitSignal(1));
}
TEST(LastCombinerTest, singleSlot) {
  sig::Signal<int(int), sig::LastCombiner<int>> sig;
  sig.connectSlot([](const int x) { return x; });

  const int res = sig.emitSignal(1);

  EXPECT_EQ(res, 1);
}
TEST(LastCombinerTest, multipleSlots) {
  sig::Signal<int(int), sig::LastCombiner<int>> sig;
  sig.connectSlot([](const int x) { return x; });
  sig.connectSlot([](const int x) { return 2*x; });

  const int res = sig.emitSignal(2);

  EXPECT_EQ(res, 4);
}
TEST(LastCombinerTest, voidReturn) {
  sig::Signal<void(int), sig::LastCombiner<int>> sig;
  sig.connectSlot([](const int x) { printf("%d\n", x); });

  const int res = sig.emitSignal(2);
  EXPECT_EQ(res, 0);
}

/* Tests for VectorCombiner */

TEST(VectorCombinerTest, noSlots) {
  sig::Signal<int(int), sig::VectorCombiner<int>> sig;

  const auto res = sig.emitSignal(2);
  EXPECT_TRUE(res.empty());
}
TEST(VectorCombinerTest, multipleSlots) {
  sig::Signal<int(int), sig::VectorCombiner<int>> sig;
  sig.connectSlot([](const int x) { return x; });
  sig.connectSlot([](const int x) { return x; });
  sig.connectSlot([](const int x) { return x; });

  const auto res = sig.emitSignal(1);
  EXPECT_FALSE(res.empty());
  for (auto i : res) {
    EXPECT_EQ(i, 1);
  }
}
TEST(VectorCombinerTest, multipleSlotsOrder) {
  sig::Signal<int(int), sig::VectorCombiner<int>> sig;
  sig.connectSlot([](const int x) { return x; });
  sig.connectSlot([](const int x) { return x + 1; });
  sig.connectSlot([](const int x) { return x + 2; });

  const auto res = sig.emitSignal(1);
  EXPECT_FALSE(res.empty());
  int count = 1;
  for (auto i : res) {
    EXPECT_EQ(i, count);
    count++;
  }
}
TEST(VectorCombinerTest, multipleSlotsSize) {
  sig::Signal<int(int), sig::VectorCombiner<int>> sig;
  sig.connectSlot([](const int x) { return x; });
  sig.connectSlot([](const int x) { return x + 1; });
  sig.connectSlot([](const int x) { return x + 2; });

  const auto res = sig.emitSignal(1);
  EXPECT_FALSE(res.empty());
  EXPECT_EQ(res.size(), 3);
}


int main(int argc, char* argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
