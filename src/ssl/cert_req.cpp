/**
* Certificate Request Message 
* (C) 2004-2006 Jack Lloyd
*
* Released under the terms of the Botan license
*/

#include <botan/tls_messages.h>
#include <botan/der_enc.h>
#include <botan/ber_dec.h>
#include <botan/loadstor.h>
#include <botan/secqueue.h>

namespace Botan {

/**
* Create a new Certificate Request message
*/
Certificate_Req::Certificate_Req(Record_Writer& writer,
                                 HandshakeHash& hash,
                                 const std::vector<X509_Certificate>& certs)
   {
   for(u32bit j = 0; j != certs.size(); j++)
      names.push_back(certs[j].subject_dn());

   // FIXME: should be able to choose what to ask for
   types.push_back(RSA_CERT);
   types.push_back(DSS_CERT);

   send(writer, hash);
   }

/**
* Serialize a Certificate Request message
*/
SecureVector<byte> Certificate_Req::serialize() const
   {
   SecureVector<byte> buf;

   buf.append(types.size());
   for(u32bit j = 0; j != types.size(); j++)
      buf.append(types[j]);

   DER_Encoder encoder;
   for(u32bit j = 0; j != names.size(); j++)
      encoder.encode(names[j]);

   SecureVector<byte> der_names = encoder.get_contents();
   u16bit names_size = der_names.size();

   buf.append(get_byte(0, names_size));
   buf.append(get_byte(1, names_size));
   buf.append(der_names);

   return buf;
   }

/**
* Deserialize a Certificate Request message
*/
void Certificate_Req::deserialize(const MemoryRegion<byte>& buf)
   {
   if(buf.size() < 4)
      throw Decoding_Error("Certificate_Req: Bad certificate request");

   u32bit types_size = buf[0];

   if(buf.size() < types_size + 3)
      throw Decoding_Error("Certificate_Req: Bad certificate request");

   for(u32bit j = 0; j != types_size; j++)
      types.push_back(static_cast<Certificate_Type>(buf[j+1]));

   u32bit names_size = make_u16bit(buf[types_size+2], buf[types_size+3]);

   if(buf.size() != names_size + types_size + 3)
      throw Decoding_Error("Certificate_Req: Bad certificate request");

   BER_Decoder decoder(buf.begin() + types_size + 3, names_size);

   while(decoder.more_items())
      {
      X509_DN name;
      decoder.decode(name);
      names.push_back(name);
      }
   }

/**
* Create a new Certificate message
*/
Certificate::Certificate(Record_Writer& writer,
                         const std::vector<X509_Certificate>& cert_list,
                         HandshakeHash& hash)
   {
   certs = cert_list;
   send(writer, hash);
   }

/**
* Serialize a Certificate message
*/
SecureVector<byte> Certificate::serialize() const
   {
   SecureVector<byte> buf(3);

   for(u32bit j = 0; j != certs.size(); j++)
      {
      SecureVector<byte> raw_cert = certs[j].BER_encode();
      u32bit cert_size = raw_cert.size();
      for(u32bit j = 0; j != 3; j++)
         buf.append(get_byte(j+1, cert_size));
      buf.append(raw_cert);
      }

   u32bit buf_size = buf.size() - 3;
   for(u32bit j = 0; j != 3; j++)
      buf[j] = get_byte(j+1, buf_size);

   return buf;
   }

/**
* Deserialize a Certificate message
*/
void Certificate::deserialize(const MemoryRegion<byte>& buf)
   {
   if(buf.size() < 3)
      throw Decoding_Error("Certificate: Message malformed");

   u32bit total_size = make_u32bit(0, buf[0], buf[1], buf[2]);

   SecureQueue queue;
   queue.write(buf + 3, buf.size() - 3);

   if(queue.size() != total_size)
      throw Decoding_Error("Certificate: Message malformed");

   while(queue.size())
      {
      if(queue.size() < 3)
         throw Decoding_Error("Certificate: Message malformed");

      byte len[3];
      queue.read(len, 3);
      u32bit cert_size = make_u32bit(0, len[0], len[1], len[2]);

      u32bit original_size = queue.size();
      X509_Certificate cert(queue);
      if(queue.size() + cert_size != original_size)
         throw Decoding_Error("Certificate: Message malformed");
      certs.push_back(cert);
      }
   }

}