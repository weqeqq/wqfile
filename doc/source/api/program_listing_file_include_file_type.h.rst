
.. _program_listing_file_include_file_type.h:

Program Listing for File type.h
===============================

|exhale_lsh| :ref:`Return to documentation for file <file_include_file_type.h>` (``include/file/type.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   
   #pragma once
   
   #include <memory>
   #include <vector>
   #include <cstdint>
   
   namespace File {
   
   using Byte               = std::uint8_t;
   using ByteSequence       = std::vector<std::uint8_t>;
   using ByteSequenceShared = std::shared_ptr<ByteSequence>;
   
   };
