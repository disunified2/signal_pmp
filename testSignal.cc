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

/* Tests for DiscardCombiner */

TEST(DiscardCombinerTest, returnVoid) {
  sig::Signal<int(int), sig::DiscardCombiner> sig;

  using ResultType = decltype(sig.emitSignal(1));

  EXPECT_TRUE((std::is_same_v<ResultType, void>));
}

int main(int argc, char* argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
