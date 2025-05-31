
.. _program_listing_file_include_file_read.h:

Program Listing for File read.h
===============================

|exhale_lsh| :ref:`Return to documentation for file <file_include_file_read.h>` (``include/file/read.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   
   #pragma once
   
   #include "file/type.h"
   #include <fstream>
   #include <cstdint>
   #include <cstring>
   #include <cerrno>
   #include <file/error.h>
   
   namespace File {
   
   class ReadError : public Error {
   public:
     struct OpenFailure;
     struct ReadFailure;
   
   protected:
     explicit ReadError(
       const std::string &message
   
     ) : Error(Namespace("ReadError", message)) {}
   };
   
   struct ReadError::OpenFailure : ReadError {
     explicit OpenFailure(
       const std::string &message
   
     ) : ReadError(Message("OpenFailure", message)) {}
   };
   
   struct ReadError::ReadFailure : ReadError {
     explicit ReadFailure(
       const std::string &message
   
     ) : ReadError(Message("ReadFailure", message)) {}
   };
   namespace Detail {
   
   class ReadFunction {
   public:
     ByteSequence operator()(const std::string &filename) const {
       std::ifstream stream = OpenStream(filename);
       std::uint64_t length = stream.tellg();
   
       stream.seekg(0, std::ios::beg); return Read(stream, length);
     }
   private:
     std::ifstream OpenStream(const std::string &filename) const {
       std::ifstream stream(
         filename,
         std::ios::binary | std::ios::ate
       );
       if (!stream) {
         throw ReadError::OpenFailure(std::strerror(errno));
       }
       return stream;
     }
     ByteSequence Read(
       std::ifstream &stream, std::uint64_t length
     ) const {
       ByteSequence data(length);
       stream.read(
         reinterpret_cast<char *>(data.data()),
         length
       );
       if (!stream) {
         throw ReadError::ReadFailure(std::strerror(errno));
       }
       return data;
     }
   };
   };
   inline constexpr auto Read = Detail::ReadFunction();
   };
