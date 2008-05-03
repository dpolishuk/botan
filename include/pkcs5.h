/*************************************************
* PKCS #5 Header File                            *
* (C) 1999-2007 Jack Lloyd                       *
*************************************************/

#ifndef BOTAN_PKCS5_H__
#define BOTAN_PKCS5_H__

#include <botan/s2k.h>

namespace Botan {

/*************************************************
* PKCS #5 PBKDF1                                 *
*************************************************/
class BOTAN_DLL PKCS5_PBKDF1 : public S2K
   {
   public:
      std::string name() const;
      S2K* clone() const { return new PKCS5_PBKDF1(hash_name); }
      PKCS5_PBKDF1(const std::string&);
   private:
      OctetString derive(length_type, const std::string&,
                          const byte[], length_type, length_type) const;
      const std::string hash_name;
   };

/*************************************************
* PKCS #5 PBKDF2                                 *
*************************************************/
class BOTAN_DLL PKCS5_PBKDF2 : public S2K
   {
   public:
      std::string name() const;
      S2K* clone() const { return new PKCS5_PBKDF2(hash_name); }
      PKCS5_PBKDF2(const std::string&);
   private:
      OctetString derive(length_type, const std::string&,
                          const byte[], length_type, length_type) const;
      const std::string hash_name;
   };

}

#endif
