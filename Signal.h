#ifndef SIGNAL_H
#define SIGNAL_H

#include <functional>
#include <map>
#include <vector>
#include <optional>

namespace sig {

  class DiscardCombiner {
  public:
    using result_type = void;

    template<typename U>
    void combine([[maybe_unused]] U item) {}

    result_type result() {}

    void reset() {}
  };

  template<typename T>
  class LastCombiner {
  public:
    using result_type = T;

    template<typename U>
    void combine(U item) {
      if constexpr (!std::is_same_v<result_type, void>) {
        res = static_cast<result_type>(item);
      }
    }

    result_type result() {
      if constexpr (std::is_same_v<result_type, void>) {
        return;
      }
      else {
        return res;
      }
    }

    void reset() {}

  private:
    std::conditional_t<std::is_void_v<T>, int, result_type> res{};
  };

  template<typename T>
  class VectorCombiner {
  public:
    using result_type = std::conditional_t<std::is_void_v<T>, void, std::vector<T>>;

    template<typename U>
    void combine(U item) {
      if constexpr (!std::is_same_v<T, void>) {
        res.push_back(static_cast<T>(item));
      }
    }

    result_type result() {
      if constexpr (std::is_same_v<result_type, void>) {
        return;
      } else {
        return res;
      }
    }

    void reset() {
      if constexpr (std::is_same_v<result_type, std::vector<T>>) {
        res.clear();
      }
    }

  private:
    std::conditional_t<std::is_void_v<result_type>, int, result_type> res;
  };


  enum class PredicateType {
    Unary,
    Binary,
  };

  template<typename T, PredicateType PType = PredicateType::Binary>
  class PredicateCombiner {
  public:
    using predicate_type = std::conditional_t<
      PType == PredicateType::Unary,
      std::function<bool(const T&)>,
      std::function<bool(const T&, const T&)>
    >;

    using result_type = std::conditional_t<std::is_void_v<T>, void, std::optional<T>>;

    PredicateCombiner(predicate_type predicate) : predicate(predicate) {}

    template<typename U>
    void combine(U item) {
      // This decides where the predicate has one or two inputs
      if constexpr (PType == PredicateType::Unary) {
        if (predicate(item)) {
          lastItem = item;
        }
      } else {
        if (lastItem == std::nullopt || predicate(*lastItem, item)) {
          lastItem = item;
        }
      }
    }

    void reset() {
      lastItem = std::nullopt;
    }

    result_type result() {
      return lastItem;
    }

  private:
    std::conditional_t<std::is_void_v<result_type>, int, result_type> lastItem;
    predicate_type predicate;
  };

  template<typename Signature, typename Combiner = DiscardCombiner>
  class Signal;

  template<typename R, typename... Args, typename Combiner>
  class Signal<R(Args...), Combiner> {
  private:
    // map keeping track of functions
    std::map<size_t, std::function<R(Args...)>> slots;

    // next id to set in map
    size_t nextId;

    Combiner combiner;

  public:
    using combiner_type = Combiner;

    using result_type = typename combiner_type::result_type;

    // Default constructor
    Signal(Combiner combiner = Combiner()) : nextId(0), combiner(combiner) { }

    // Constructor that allows forwarding to the constructor of combiner
    template<typename... CombinerArgs>
    Signal(CombinerArgs ... args) : nextId(0), combiner(args...) { }

    std::size_t connectSlot(std::function<R(Args...)> callback) {
      size_t slotId = nextId++;
      slots[slotId] = callback;
      return slotId;
    }

    void disconnectSlot(std::size_t id) {
      slots.erase(id);
    }

    result_type emitSignal(Args... args) {
      // Reset the combine to not keep unwanted or deleted results
      combiner.reset();

      if constexpr (!std::is_same_v<R, void>) {
        for (auto &slot : slots) {
          auto value = slot.second(args...);
          combiner.combine(value);
        }
      }
      return combiner.result();
    }
  };

}

#endif // SIGNAL_H
