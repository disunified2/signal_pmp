#ifndef SIGNAL_H
#define SIGNAL_H

#include <map>
#include <optional>

namespace sig {

  class DiscardCombiner {
  public:
    using result_type = /* implementation defined */;

    template<typename U>
    void combine(/* implementation defined */ item) {
      // implementation defined
    }

    result_type result() {
      // implementation defined
    }
  };

  template<typename T>
  class LastCombiner {
  public:
    using result_type = /* implementation defined */;

    template<typename U>
    void combine(/* implementation defined */ item) {
      // implementation defined
    }

    result_type result() {
      // implementation defined
    }
  };

  template<typename T>
  class VectorCombiner {
  public:
    using result_type = /* implementation defined */;

    template<typename U>
    void combine(/* implementation defined */ item) {
      // implementation defined
    }

    result_type result() {
      // implementation defined
    }
  };

  enum class PredicateType {
    Unary,
    Binary,
  };

  template<typename T, PredicateType PType = PredicateType::Binary>
  class PredicateCombiner {
  public:
    using result_type = std::optional</* implementation defined */>;

    PredicateCombiner(/* implementation defined */ predicate) {
      // implementation defined
    }

    template<typename U>
    void combine(/* implementation defined */ item) {
      // implementation defined
    }

    result_type result() {
      // implementation defined
    }
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
      for (auto &slot : slots) {
        auto value = slot.second(args...);
        combiner.combine(value);
      }
      return combiner.result();
    }
  };

}

#endif // SIGNAL_H
