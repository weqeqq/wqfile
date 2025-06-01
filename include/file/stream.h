
#pragma once 

#include <concepts>
#include <array>
#include <span>
#include <cstdint>
#include <algorithm>
#include <ranges>
#include <iterator>
#include <file/read.h>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>
#include <memory>

#include <bit>
#include <file/type.h>
#include <file/error.h>

namespace File {
/**
 * @enum Endian
 * @brief Specifies the endianness for data streaming.
 *
 * @var Endian::Big
 * @brief Big-endian format.
 *
 * @var Endian::Little
 * @brief Little-endian format.
 *
 * @var Endian::Native
 * @brief Native endianness of the system.
 */
enum class Endian {
  Big    = std::to_underlying(std::endian::big),
  Little = std::to_underlying(std::endian::little),
  Native = std::to_underlying(std::endian::native)
};
/**
 * @class Stream
 * @brief A stream class for reading and writing data with specified endianness.
 *
 * @tparam E The endianness to use for the stream. Defaults to Endian::Native.
 */
template <Endian E>
class Stream;
/**
 * @class Serialiser
 * @brief A template for serializing objects of type T with endianness E.
 *
 * @tparam T The type to serialize.
 * @tparam E The endianness to use.
 *
 * Specialize this template to provide custom serialization for your types.
 *
 * @note The specialization should provide a constructor taking (const T&, Stream<E>&) and a function call operator.
 */
template <typename T, Endian E>
struct Serialiser {
public:
  using Target = T; static constexpr Endian UsedEndian = E;

  explicit Serialiser(
    const Target &, Stream<UsedEndian> &
  ) {
    static_assert(false, "Serialiser not implemented");
  }
};
/**
 * @class Deserialiser
 * @brief A template for deserializing objects of type T with endianness E.
 *
 * @tparam T The type to deserialize.
 * @tparam E The endianness to use.
 *
 * Specialize this template to provide custom deserialization for your types.
 *
 * @note The specialization should provide a constructor taking (T&, Stream<E>&) and a function call operator.
 */
template <typename T, Endian E>
struct Deserialiser {
public:
  using Target = T; static constexpr Endian UsedEndian = E;

  explicit Deserialiser(
    Target &, Stream<UsedEndian> &
  ) {
    static_assert(false, "Deserialiser not implemented");
  }
};
class StreamError : public Error {
protected:
  explicit StreamError(
    std::string_view message

  ) : Error(Namespace("StreamError", message)) {} 
};
namespace StreamErrorMessage {
class ReadFailure : public StreamError {
public:
  explicit ReadFailure(
    std::string_view message 

  ) : StreamError(Message("ReadFailure", message)) {}
};
};
namespace Detail {
namespace Concept {
/**
 * @concept Simple
 * @brief Concept for simple types: integral or floating-point.
 */
template <typename T>
concept Simple = 
  std::is_integral_v       <T> ||
  std::is_floating_point_v <T>;

template <typename T>
struct Vector_Struct 
  : std::false_type {};

template <typename T>
struct Vector_Struct<std::vector<T>> 
  : std::true_type {};
/**
 * @concept Vector
 * @brief Concept for vector types.
 */
template <typename T>
concept Vector = Vector_Struct<T>::value;

template <typename T>
struct String_Struct 
  : std::false_type {};

template <typename T>
struct String_Struct<std::basic_string<T>> 
  : std::true_type {};
/**
 * @concept String
 * @brief Concept for string types.
 */
template <typename T>
concept String = String_Struct<T>::value;

template <typename T>
struct Array_Struct 
  : std::false_type {};

template <typename T, std::uint64_t S>
struct Array_Struct<std::array<T, S>> 
  : std::true_type {};
/**
 * @concept Array
 * @brief Concept for array types.
 */
template <typename T>
concept Array = Array_Struct<T>::value;

template <typename T>
struct Span_Struct 
  : std::false_type {};

template <typename T>
struct Span_Struct<std::span<T>>
  : std::true_type {};
/**
 * @concept Span
 * @brief Concept for span types.
 */
template <typename T>
concept Span = Span_Struct<T>::value;
/**
 * @concept Iterable
 * @brief Concept for iterable types: vectors, strings, arrays, or spans.
 */
template <typename T>
concept Iterable = 
  Vector <T> || 
  String <T> || 
  Array  <T> || 
  Span   <T>;

template <typename T>
concept HasTarget = requires {
  typename T::Target;
};
template <typename T>
concept HasUsedEndian = requires {
  T::UsedEndian;
};
template <typename T>
concept SerialiserHasConstructor = 
  HasTarget     <T> && 
  HasUsedEndian <T> &&
  requires(const T::Target &target, Stream<T::UsedEndian> &stream) {
    T(target, stream);
  };
template <typename T, typename... A>
concept HasFunctionOperator = requires(T input, A... args) {
  { input(args...) } -> std::same_as<void>;
};
template <typename T, typename... A>
concept SerialiserImpl = 
  SerialiserHasConstructor <T> &&
  HasFunctionOperator      <T, A...>;

template <typename T, Endian E, typename... A>
concept HasInternalSerialiserImpl = requires { 
  requires SerialiserImpl<typename T::template Serialiser<E>, A...>;
};
template <typename T, Endian E, typename... A>
concept HasExternalSerialiserImpl = requires {
  requires SerialiserImpl<Serialiser<T, E>, A...>;
};
template <typename T, Endian E, typename... A>
concept HasSerialiserImpl = requires {
  requires (HasInternalSerialiserImpl<T, E, A...> ||
            HasExternalSerialiserImpl<T, E, A...>); 
};
template <typename T>
concept DeserialiserHasConstructor = 
  HasTarget     <T> && 
  HasUsedEndian <T> && 
  requires(T::Target &target, Stream<T::UsedEndian> &stream) {
    T(target, stream); 
  };
template <typename T, typename... A>
concept DeserialiserImpl = 
  DeserialiserHasConstructor <T> && 
  HasFunctionOperator        <T, A...>;

template <typename T, Endian E, typename... A>
concept HasInternalDeserialiserImpl = requires { 
  requires DeserialiserImpl<typename T::template Deserialiser<E>, A...>;
};
template <typename T, Endian E, typename... A>
concept HasExternalDeserialiserImpl = requires {
  requires DeserialiserImpl<Deserialiser<T, E>, A...>;
};
template <typename T, Endian E, typename... A>
concept HasDeserialiserImpl = requires {
  requires (HasInternalDeserialiserImpl<T, E, A...> ||
            HasExternalDeserialiserImpl<T, E, A...>); 
};
};
};
template <Endian E = Endian::Native> 
class Stream {
public:
  /**
   * @brief The endianness used by this stream instance.
   */
  static constexpr Endian UsedEndian = E;
  /**
   * @brief Default constructor.
   *
   * Creates an empty stream.
   */
  explicit Stream() = default;
  /**
   * @brief Constructor from a byte sequence.
   *
   * @param data The byte sequence to use for the stream.
   */
  explicit Stream(ByteSequence data) : data_(std::make_shared<ByteSequence>(std::move(data))) {}
  /**
   * @brief Constructor from a file.
   *
   * Reads the file and uses its contents as the stream data.
   *
   * @param filename The name of the file to read.
   */
  explicit Stream(
    const std::string &filename
  ) : data_(new ByteSequence(File::Read(filename))) {}
  /**
   * @brief Deserialises an object from the stream using either internal or external deserialisation.
   *
   * This method provides a unified interface for reading objects that may implement deserialisation
   * through either:
   * 1. An internal `Deserialiser` type (HasInternalDeserialiserImpl), or
   * 2. An external `Deserialiser` specialization (HasExternalDeserialiserImpl)
   *
   * @tparam T The type of object to be deserialized (must be default constructible)
   * @tparam A Additional argument types needed for the deserialisation process
   *
   * @param args Additional arguments to forward to the deserialiser
   * @return The deserialised object of type T
   *
   * @note The method requires that either:
   *       - `T` provides an internal `Deserialiser<UsedEndian>` type constructible with
   *         `(T&&, Stream<UsedEndian>&)` and callable with `args...`, OR
   *       - There exists a `Deserialiser<T, UsedEndian>` specialization constructible with
   *         `(T&&, Stream<UsedEndian>&)` and callable with `args...`
   *
   * @par Example (Internal Deserialiser):
   * @code
   * struct CustomType {
   *
   *   template<Endian E>
   *   struct Deserialiser {
   *     explicit Deserialiser(T&, Stream<E>&);
   *
   *     void operator()(int);
   *   };
   *
   * };
   * auto value = stream.Read<CustomType>(42);
   * @endcode
   *
   * @par Example (External Deserialiser):
   * @code
   * template<Endian E>
   * struct Deserialiser<OtherType, E> {
   *
   *   explicit Deserialiser(OtherType&, Stream<E>&);
   *
   *   void operator()(double);
   * };
   * auto value = stream.Read<OtherType>(3.14);
   * @endcode
   *
   * @warning The type T must be default constructible as this method creates a temporary
   *          which is then passed to the deserializer
   */
  template <typename T, typename... A>
  requires Detail::Concept::HasDeserialiserImpl<T, UsedEndian, A...>
  inline T Read(A&&... args) {
    return Read(T(), std::forward<A>(args)...);
  }
  /**
   * @brief Reads a simple type from the stream.
   *
   * @tparam Simple The type to read, must be a simple type (integral or floating-point).
   * @return The read value.
   *
   * @throws StreamError::ReadFailure If there is insufficient data in the stream.
   */
  template <Detail::Concept::Simple Simple> 
  inline Simple Read() {
    if (std::ranges::distance(current_, data_->end()) < sizeof(Simple)) {
      throw StreamErrorMessage::ReadFailure("Insufficient data");
    }
    Simple simple;
    if constexpr (UsedEndian != Endian::Native) {
      std::ranges::reverse_copy(
        std::span(current_, sizeof(Simple)), reinterpret_cast<Byte *>(&simple)
      );
    } else {
      std::ranges::copy(
        std::span(current_, sizeof(simple)), reinterpret_cast<Byte *>(&simple)
      );
    }
    current_ = std::ranges::next(current_, sizeof(simple));
    return simple;
  }
  /**
   * @brief Reads data into an iterable container.
   *
   * @tparam Iterable The type of the iterable container.
   * @param iterable The container to read data into.
   * @return The iterable container.
   *
   * @note The elements of the iterable must be simple types.
   */
  template <Detail::Concept::Iterable Iterable>
  inline Iterable Read(Iterable &&iterable) {
    static_assert(
      Detail::Concept::Simple<std::ranges::range_value_t<Iterable>>,
      "Elements of Iterable must satisfy Detail::Concept::Simple"
    );
    for (auto &element : iterable) {
      element = Read<std::ranges::range_value_t<Iterable>>();
    }
    return std::forward<Iterable>(iterable);
  }
  /**
   * @brief Reads a string of specified length.
   *
   * @tparam String The string type.
   * @param length The length of the string to read.
   * @return The read string.
   */
  template <Detail::Concept::String String>
  inline String Read(std::uint64_t length) {
    return Read(String(length, 0));
  }
  /**
   * @brief Reads a vector of specified length.
   *
   * @tparam Vector The vector type.
   * @param length The number of elements to read.
   * @return The read vector.
   */
  template <Detail::Concept::Vector Vector>
  inline Vector Read(std::uint64_t length) {
    return Read(Vector(length));
  }
  /**
   * @brief Reads an array from the stream.
   *
   * @tparam Array The array type.
   * @return The read array.
   */
  template <Detail::Concept::Array Array> 
  inline Array Read() {
    return Read(Array());
  }
  /**
   * @brief Reads a span of bytes from the stream.
   *
   * @param length The number of bytes to read.
   * @return A span pointing to the read bytes in the stream.
   */
  template <Detail::Concept::Span Span> 
  requires std::same_as<Span, ByteSpan>
  inline Span Read(std::uint64_t length) {

    if (std::ranges::distance(current_, data_->end()) < length) {
      throw StreamErrorMessage::ReadFailure("Insufficient data");
    }
    auto span = Span(current_, length);

    current_ = std::ranges::next(
      current_, length
    );
    return span;
  }
  /**
   * @brief Writes a simple type to the stream.
   *
   * @tparam Simple The type to write, must be a simple type (integral or floating-point).
   * @param simple The value to write.
   */
  template <Detail::Concept::Simple Simple>
  inline void Write(const Simple &simple) {
    current_ = std::ranges::next(
      data_->begin(), UpdateData<Simple>()
    );
    if constexpr (UsedEndian != Endian::Native) {
      current_ = std::ranges::reverse_copy(
        std::span(reinterpret_cast<const Byte *>(&simple), sizeof(Simple)), current_
      ).out;
    } else {
      current_ = std::ranges::copy(
        std::span(reinterpret_cast<const Byte *>(&simple), sizeof(Simple)), current_
      ).out;
    }
  }
  /**
   * @brief Writes an iterable container to the stream.
   *
   * @tparam Iterable The type of the iterable container.
   * @param iterable The container to write.
   *
   * @note The elements of the iterable must be simple types.
   */
  template <Detail::Concept::Iterable Iterable>
  inline void Write(const Iterable &iterable) {
    static_assert(
      Detail::Concept::Simple<std::ranges::range_value_t<Iterable>>,
      "Elements of Iterable must satisfy Detail::Concept::Simple"
    );
    for (const auto &element : iterable) {
      Write(element);
    }
  }
  /**
   * @brief Writes an object to the stream using either its internal or external serialisation.
   *
   * This method provides a unified interface for writing objects that may implement serialisation
   * in one of two ways:
   * 1. Through an internal `Serialiser` type (HasInternalSerialiserImpl)
   * 2. Through an external `Serialiser` specialization (HasExternalSerialiserImpl)
   *
   * @tparam Input The type of object to be serialized.
   * @tparam Args  Additional arguments needed for the serialization process.
   *
   * @param input The object to be written to the stream.
   * @param args  Additional arguments to forward to the serialiser.
   *
   * @note The method requires that either:
   *       - `Input` provides an internal `Serialiser<UsedEndian>` type constructible with
   *         `(const Input&, Stream<UsedEndian>&)` and callable with `args...`, OR
   *       - There exists a `Serialiser<Input, UsedEndian>` specialization constructible with
   *         `(const Input&, Stream<UsedEndian>&)` and callable with `args...`
   *
   * @code
   * // For a type with internal serialisation:
   * struct CustomType {
   *   template<Endian E>
   *   struct Serialiser {
   *     
   *     explicit Serialiser(const CustomType&, Stream<E>&);
   *
   *     void operator()(int);
   *   };
   * };
   * stream.Write(CustomType(), 42);
   *
   * // For a type with external serialisation:
   * template<Endian E>
   * struct Serialiser<OtherType, E> {
   *
   *   explicit Serialiser(const OtherType&, Stream<E>&);
   *
   *   void operator()(double);
   * };
   * stream.Write(OtherType(), 3.14);
   * @endcode
   */
  template <typename Input, typename... Args>
  requires Detail::Concept::HasSerialiserImpl<Input, UsedEndian, Args...>
  inline void Write(const Input &input, Args&&... args) {

    if constexpr (requires {
      requires Detail::Concept::HasInternalSerialiserImpl<Input, UsedEndian, Args...>; 
    }) {
      return WriteInternal(input, std::forward<Args>(args)...);
    } else {
      return WriteExternal(input, std::forward<Args>(args)...);
    }
  }
  /**
   * @brief Gets the current position in the stream.
   *
   * @return The current position.
   */
  std::uint64_t Tell() const {
    return std::ranges::distance(
      data_->begin(), current_
    ); 
  }
  /**
   * @brief Reserves space in the underlying container to accommodate at least the specified number of elements.
   *
   * This function ensures that the container has enough capacity to hold at least `input` elements
   * without needing to reallocate memory. It preserves the position of the current iterator by
   * calculating its offset from the container's beginning, reserving the requested capacity, and
   * then updating the iterator to point to the same logical position in the potentially reallocated
   * container.
   *
   * @param input The minimum number of elements for which to reserve space in the container.
   */
  void Reserve(std::uint64_t input) {
    std::uint64_t offset = std::ranges::distance(
      data_->begin(), current_
    );
    data_->reserve(input);
    current_ = std::ranges::next(
      data_->begin(), offset
    );
  }
  void Seek(std::uint64_t offset) {
    current_ = std::ranges::next(
      current_, offset, data_->end()
    );
  }
  void Rewind() {
    current_ = data_->begin();
  }
  /**
   * @brief Gets the total length of the stream.
   *
   * @return The length of the stream.
   */
  std::uint64_t Length() const {
    return data_->size();
  }
private:
  std::shared_ptr<ByteSequence> data_ = std::make_shared<ByteSequence>();
  ByteSequence::iterator current_ = data_->begin();

  template <typename T, typename... A>
  requires Detail::Concept::HasInternalDeserialiserImpl<T, UsedEndian, A...> 
  inline T Read(T &&output, A&&... args) {
    typename T::template Deserialiser<UsedEndian>(
      output, *this
    )(std::forward<A>(args)...);
    return std::forward<T>(output);
  }
  template <typename T, typename... A>
  requires Detail::Concept::HasExternalDeserialiserImpl<T, UsedEndian, A...>
  inline T Read(T &&output, A&&... args) {
    Deserialiser<T, UsedEndian>(
      output, *this
    )(std::forward<A>(args)...);
    return std::forward<T>(output);
  }
  template <typename Input, typename... Args>
  requires Detail::Concept::HasInternalSerialiserImpl<Input, UsedEndian, Args...>
  inline void WriteInternal(const Input &input, Args&&... args) {
    typename Input::template Serialiser<UsedEndian>(
      input, *this
    )(std::forward<Args>(args)...);
  }
  template <typename Input, typename... Args> 
  requires Detail::Concept::HasExternalSerialiserImpl<Input, UsedEndian, Args...>
  inline void WriteExternal(const Input &input, Args&&... args) {
    Serialiser<Input, UsedEndian>(
      input, *this
    )(std::forward<Args>(args)...);
  }
  template <Detail::Concept::Simple Simple>
  std::uint64_t UpdateData() {
    auto offset = std::ranges::distance(
      data_->begin(), current_
    );
    if (std::ranges::distance(current_, data_->end()) < sizeof(Simple)) {
      data_->resize(Length() + sizeof(Simple));
    }
    return offset;
  }
};
};

