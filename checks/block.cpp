
/*
  We don't use the standard issue ECB filter, because we also want to check
  that the encryption and decryption operations are inverses (ie, it works).

  This class only works with NoPadding mode, unlike the regular ECB filters
*/

#include <iostream>
#include <string>
#include <cstdlib>
#include <botan/filter.h>
#include <botan/lookup.h>
using namespace Botan;

class ECB_Encryption_ErrorCheck : public Filter
   {
   public:
      void write(const byte[], length_type);
      void end_msg();
      ECB_Encryption_ErrorCheck(const std::string& cipher_name,
                                const std::string&,
                                const SymmetricKey& key) :
         BLOCKSIZE(block_size_of(cipher_name))
         {
         const std::string HASH = "SHA-1";

         cipher = get_block_cipher(cipher_name);
         input_hash = get_hash(HASH);
         decrypt_hash = get_hash(HASH);
         buffer.create(BLOCKSIZE);
         cipher->set_key(key);
         position = 0;
         }
      ~ECB_Encryption_ErrorCheck()
         { delete cipher; delete input_hash; delete decrypt_hash; }
   private:
      const length_type BLOCKSIZE;
      BlockCipher* cipher;
      SecureVector<byte> buffer;
      length_type position;
      HashFunction* input_hash, *decrypt_hash;
   };

void ECB_Encryption_ErrorCheck::write(const byte input[], length_type length)
   {
   input_hash->update(input, length);
   buffer.copy(position, input, length);
   if(position + length >= BLOCKSIZE)
      {
      cipher->encrypt(buffer);
      send(buffer, BLOCKSIZE);
      cipher->decrypt(buffer);
      decrypt_hash->update(buffer, BLOCKSIZE);
      input += (BLOCKSIZE - position);
      length -= (BLOCKSIZE - position);
      while(length >= BLOCKSIZE)
         {
         cipher->encrypt(input, buffer);
         send(buffer, BLOCKSIZE);
         cipher->decrypt(buffer);
         decrypt_hash->update(buffer, BLOCKSIZE);
         input += BLOCKSIZE;
         length -= BLOCKSIZE;
         }
      buffer.copy(input, length);
      position = 0;
      }
   position += length;
   }

void ECB_Encryption_ErrorCheck::end_msg()
   {
   SecureVector<byte> hash1 = input_hash->final();
   SecureVector<byte> hash2 = decrypt_hash->final();

   if(hash1 != hash2)
      {
      std::cout << "In " << cipher->name()
                << " decryption check failed." << std::endl;
      }

   if(position)
      throw Exception("ECB: input was not in full blocks");
   }

Filter* lookup_block(const std::string& algname, const std::string& key)
   {
   Filter* cipher = 0;
   try {
      cipher = new ECB_Encryption_ErrorCheck(algname, "NoPadding", key);
      }
   catch(Algorithm_Not_Found) {}

   return cipher;
   }
