
.. _program_listing_file_include_file_error.h:

Program Listing for File error.h
================================

|exhale_lsh| :ref:`Return to documentation for file <file_include_file_error.h>` (``include/file/error.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   
   #pragma once
   
   #include <exception/error.h>
   
   namespace File {
   class Error : public Exception::Error {
   protected:
     explicit Error(
       const std::string &message
   
     ) : Exception::Error(Namespace("File", message)) {}
   };
   };
