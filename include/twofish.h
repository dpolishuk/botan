/*************************************************
* Twofish Header File                            *
* (C) 1999-2007 Jack Lloyd                       *
*************************************************/

#ifndef BOTAN_TWOFISH_H__
#define BOTAN_TWOFISH_H__

#include <botan/base.h>

namespace Botan {

/*************************************************
* Twofish                                        *
*************************************************/
class BOTAN_DLL Twofish : public BlockCipher
   {
   public:
      void clear() throw();
      std::string name() const { return "Twofish"; }
      BlockCipher* clone() const { return new Twofish; }
      Twofish() : BlockCipher(16, 16, 32, 8) {}
   private:
      void enc(const byte[], byte[]) const;
      void dec(const byte[], byte[]) const;
      void key(const byte[], length_type);

      static void rs_mul(byte[4], byte, length_type);

      static const u32bit MDS0[256];
      static const u32bit MDS1[256];
      static const u32bit MDS2[256];
      static const u32bit MDS3[256];
      static const byte Q0[256];
      static const byte Q1[256];
      static const byte RS[32];
      static const byte EXP_TO_POLY[255];
      static const byte POLY_TO_EXP[255];

      SecureBuffer<u32bit, 256> SBox0, SBox1, SBox2, SBox3;
      SecureBuffer<u32bit, 40> round_key;
   };

}

#endif
