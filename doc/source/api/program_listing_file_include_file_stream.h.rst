
.. _program_listing_file_include_file_stream.h:

Program Listing for File stream.h
=================================

|exhale_lsh| :ref:`Return to documentation for file <file_include_file_stream.h>` (``include/file/stream.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   
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
   #include <unicode/type.h>
   #include <memory>
   
   #include <bit>
   #include <file/export.h>
   #include <file/type.h>
   #include <file/error.h>
   
   namespace File {
   using ByteSpan = std::span<const Byte>;
   enum class Endian {
     Big    = std::to_underlying(std::endian::big),
     Little = std::to_underlying(std::endian::little),
     Native = std::to_underlying(std::endian::native)
   };
   template <Endian E>
   class Stream;
   template <typename T, Endian E>
   struct Serialiser {
   public:
     using Target = T; static constexpr Endian UsedEndian = E;
   
     explicit Serialiser(
       const Target &target, Stream<UsedEndian> &stream
     ) {
       static_assert(false, "Serialiser not implemented");
     }
   };
   template <typename T, Endian E>
   struct Deserialiser {
   public:
     using Target = T; static constexpr Endian UsedEndian = E;
   
     explicit Deserialiser(
       Target &target, Stream<UsedEndian> &stream
     ) {
       static_assert(false, "Deserialiser not implemented");
     }
   };
   class StreamError : public Error {
   protected:
     explicit StreamError(
       const std::string &message
   
     ) : Error(Namespace("StreamError", message)) {} 
   };
   struct StreamReadFailure : StreamError {
     explicit StreamReadFailure(
       const std::string &message 
   
     ) : StreamError(Message("ReadFailure", message)) {}
   };
   namespace Detail {
   namespace Concept {
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
   template <typename T>
   concept Vector = Vector_Struct<T>::value;
   
   template <typename T>
   struct String_Struct 
     : std::false_type {};
   
   template <typename T>
   struct String_Struct<std::basic_string<T>> 
     : std::true_type {};
   template <typename T>
   concept String = String_Struct<T>::value;
   
   template <typename T>
   struct Array_Struct 
     : std::false_type {};
   
   template <typename T, std::uint64_t S>
   struct Array_Struct<std::array<T, S>> 
     : std::true_type {};
   template <typename T>
   concept Array = Array_Struct<T>::value;
   
   template <typename T>
   struct Span_Struct 
     : std::false_type {};
   
   template <typename T>
   struct Span_Struct<std::span<T>>
     : std::true_type {};
   template <typename T>
   concept Span = Span_Struct<T>::value;
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
   };
   };
   template <Endian E = Endian::Native> 
   class Stream {
   public:
     static constexpr Endian UsedEndian = E;
     explicit Stream() = default;
     explicit Stream(ByteSequence data) : data_(std::make_shared<ByteSequence>(std::move(data))) {}
     explicit Stream(
       const std::string &filename
     ) : data_(new ByteSequence(std::move(File::Read(filename)))) {}
     template <typename T, typename... A>
     requires Detail::Concept::HasInternalDeserialiserImpl<T, UsedEndian, A...> || 
              Detail::Concept::HasExternalDeserialiserImpl<T, UsedEndian, A...>
     inline T Read(A&&... args) {
       return Read(T(), std::forward<A>(args)...);
     }
     template <Detail::Concept::Simple Simple> 
     inline Simple Read() {
       if (std::ranges::distance(current_, data_->end()) < sizeof(Simple)) {
         throw StreamReadFailure("Insufficient data");
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
     template <Detail::Concept::String String>
     inline String Read(std::uint64_t length) {
       return Read(String(length, 0));
     }
     template <Detail::Concept::Vector Vector>
     inline Vector Read(std::uint64_t length) {
       return Read(Vector(length));
     }
     template <Detail::Concept::Array Array> 
     inline Array Read() {
       return Read(Array());
     }
     template <Detail::Concept::Span Span> 
     requires std::same_as<Span, ByteSpan>
     inline Span Read(std::uint64_t length) {
       return Span(current_, length);
     }
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
     template <typename Input, typename... Args>
     requires (Detail::Concept::HasInternalSerialiserImpl<Input, UsedEndian, Args...> || 
               Detail::Concept::HasExternalSerialiserImpl<Input, UsedEndian, Args...>)
     inline void Write(const Input &input, Args&&... args) {
       Write(input);
     }
     std::uint64_t Tell() const {
       return std::ranges::distance(
         data_->begin(), current_
       ); 
     }
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
     std::uint64_t Length() const {
       return data_->size();
     }
   private:
     std::shared_ptr<ByteSequence> data_ = std::make_shared<ByteSequence>();
     ByteSequence::iterator current_ = data_->begin();
   
     template <typename T, typename... A>
     requires Detail::Concept::HasInternalDeserialiserImpl<T, UsedEndian, A...> 
     inline T Read(T &&output, A&&... args) {
       T::template Deserialiser<UsedEndian>(
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
     inline void Write(const Input &input, Args&&... args) {
       Input::template Serialiser<UsedEndian>(
         input, *this
       )(std::forward<Args>(args)...);
     }
     template <typename Input, typename... Args> 
     requires Detail::Concept::HasExternalSerialiserImpl<Input, UsedEndian, Args...>
     inline void Write(const Input &input, Args&&... args) {
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
   
