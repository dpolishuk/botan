/*************************************************
* MGF1 Header File                               *
* (C) 1999-2007 Jack Lloyd                       *
*************************************************/

#ifndef BOTAN_MGF1_H__
#define BOTAN_MGF1_H__

#include <botan/pk_util.h>

namespace Botan {

/*************************************************
* MGF1                                           *
*************************************************/
class BOTAN_DLL MGF1 : public MGF
   {
   public:
      void mask(const byte[], length_type, byte[], length_type) const;
      MGF1(const std::string&);
   private:
      const std::string hash_name;
   };

}

#endif
