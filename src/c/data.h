/**
 * Copyright (C) 2013-2015 Regents of the University of California.
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version, with the additional exemption that
 * compiling, linking, and/or using OpenSSL is allowed.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * A copy of the GNU Lesser General Public License is in the file COPYING.
 */

#ifndef NDN_DATA_H
#define NDN_DATA_H

#include <math.h>
#include <ndn-cpp/c/data-types.h>
#include "name.h"
#include "publisher-public-key-digest.h"
#include "key-locator.h"

#ifdef __cplusplus
extern "C" {
#endif

/** ndn_SignatureType defines constants for the Signature "type" field.
 * Note that the constants are the same as defined in the NDN-TLV spec, but we
 * keep a separate enum so that we aren't directly tied to the TLV code.
 */
typedef enum {
  ndn_SignatureType_DigestSha256Signature = 0,
  ndn_SignatureType_Sha256WithRsaSignature = 1,
  ndn_SignatureType_Sha256WithEcdsaSignature = 3
} ndn_SignatureType;

/**
 * An ndn_Signature struct holds the signature bits and other info representing 
 * the signature in a data packet or signed interest. We use one structure which
 * is a union of all the fields from the different known signature types. This
 * lets us avoid the infrastructure to simulate an abstract base class with
 * subclasses and virtual methods.
 */
struct ndn_Signature {
  ndn_SignatureType type;          /**< -1 for none */
  struct ndn_Blob digestAlgorithm; /**< A Blob whose value is a pointer to a pre-allocated buffer.  0 for none.
                                    *   If none, default is 2.16.840.1.101.3.4.2.1 (sha-256). */
  struct ndn_Blob witness;         /**< A Blob whose value is a pointer to pre-allocated buffer.  0 for none. */
  struct ndn_Blob signature;
  /** @deprecated.  The Signature publisherPublicKeyDigest is deprecated.  If you need a publisher public key digest,
   * set the keyLocator keyLocatorType to KEY_LOCATOR_DIGEST and set its key data to the digest. */
  struct ndn_PublisherPublicKeyDigest publisherPublicKeyDigest;
  struct ndn_KeyLocator keyLocator;
};

/**
 * Initialize the ndn_Signature struct with values for none and the default digestAlgorithm.
 * @param self A pointer to the ndn_Signature struct.
 * @param keyNameComponents The pre-allocated array of ndn_NameComponent for the keyLocator.
 * @param maxKeyNameComponents The number of elements in the allocated keyNameComponents array.
 */
static __inline void ndn_Signature_initialize(struct ndn_Signature *self, struct ndn_NameComponent *keyNameComponents, size_t maxKeyNameComponents) {
  self->type = (ndn_SignatureType)-1;
  ndn_Blob_initialize(&self->digestAlgorithm, 0, 0);
  ndn_Blob_initialize(&self->witness, 0, 0);
  ndn_Blob_initialize(&self->signature, 0, 0);
  ndn_PublisherPublicKeyDigest_initialize(&self->publisherPublicKeyDigest);
  ndn_KeyLocator_initialize(&self->keyLocator, keyNameComponents, maxKeyNameComponents);
}

/**
 * Set the fields of the ndn_Signature struct to the values from
 * ndn_Signature_initialize.
 * @param self A pointer to the ndn_Signature struct.
 */
static __inline void ndn_Signature_clear(struct ndn_Signature *self)
{
  ndn_Signature_initialize
    (self, self->keyLocator.keyName.components,
     self->keyLocator.keyName.maxComponents);
}

/**
 * An ndn_MetaInfo struct holds the meta info which is signed inside the data packet.
 */
struct ndn_MetaInfo {
  ndn_MillisecondsSince1970 timestampMilliseconds; /**< milliseconds since 1/1/1970. -1 for none */
  ndn_ContentType type;                  /**< default is ndn_ContentType_DATA. -1 for none */
  ndn_Milliseconds freshnessPeriod;      /**< -1 for none */
  struct ndn_NameComponent finalBlockId; /**< has a pointer to a pre-allocated buffer.  0 for none */
};

/**
 * Initialize the ndn_MetaInfo struct with values for none and the type to the default ndn_ContentType_BLOB.
 * @param self A pointer to the ndn_MetaInfo struct.
 */
static __inline void ndn_MetaInfo_initialize(struct ndn_MetaInfo *self)
{
  self->timestampMilliseconds = -1;
  self->type = ndn_ContentType_BLOB;
  self->freshnessPeriod = -1;
  ndn_NameComponent_initialize(&self->finalBlockId, 0, 0);
}

/**
 * @deprecated Use freshnessPeriod.
 */
static __inline int ndn_MetaInfo_getFreshnessSeconds(struct ndn_MetaInfo *self)
{
  return self->freshnessPeriod < 0 ? -1 : (int)round(self->freshnessPeriod / 1000.0);
}

/**
 * @deprecated Use freshnessPeriod.
 */
static __inline void ndn_MetaInfo_setFreshnessSeconds(struct ndn_MetaInfo *self, int freshnessSeconds)
{
  self->freshnessPeriod = freshnessSeconds < 0 ? -1.0 : (double)freshnessSeconds * 1000.0;
}

struct ndn_Data {
  struct ndn_Signature signature;
  struct ndn_Name name;
  struct ndn_MetaInfo metaInfo;
  struct ndn_Blob content;     /**< A Blob with a pointer to the content. */
};

/**
 * Initialize an ndn_Data struct with the pre-allocated nameComponents and keyNameComponents,
 * and defaults for all the values.
 * @param self A pointer to the ndn_Data struct.
 * @param nameComponents The pre-allocated array of ndn_NameComponent.
 * @param maxNameComponents The number of elements in the allocated nameComponents array.
 * @param keyNameComponents The pre-allocated array of ndn_NameComponent for the signature.keyLocator.
 * @param maxKeyNameComponents The number of elements in the allocated keyNameComponents array.
 */
static __inline void ndn_Data_initialize
  (struct ndn_Data *self, struct ndn_NameComponent *nameComponents, size_t maxNameComponents,
   struct ndn_NameComponent *keyNameComponents, size_t maxKeyNameComponents)
{
  ndn_Signature_initialize(&self->signature, keyNameComponents, maxKeyNameComponents);
  ndn_Name_initialize(&self->name, nameComponents, maxNameComponents);
  ndn_MetaInfo_initialize(&self->metaInfo);
  ndn_Blob_initialize(&self->content, 0, 0);
}

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
namespace ndn {

class SignatureLite : private ndn_Signature {
public:
  SignatureLite(ndn_NameComponent* keyNameComponents, size_t maxKeyNameComponents)
  {
    ndn_Signature_initialize(this, keyNameComponents, maxKeyNameComponents);
  }

  void
  clear() { ndn_Signature_clear(this); }

  ndn_SignatureType
  getType() const { return type; }

  const BlobLite&
  getDigestAlgorithm() const { return BlobLite::upCast(digestAlgorithm); }

  const BlobLite&
  getWitness() const { return BlobLite::upCast(witness); }

  const BlobLite&
  getSignature() const { return BlobLite::upCast(signature); }

  const PublisherPublicKeyDigestLite&
  getPublisherPublicKeyDigest() const { return PublisherPublicKeyDigestLite::upCast(publisherPublicKeyDigest); }

  PublisherPublicKeyDigestLite&
  getPublisherPublicKeyDigest() { return PublisherPublicKeyDigestLite::upCast(publisherPublicKeyDigest); }

  const KeyLocatorLite&
  getKeyLocator() const { return KeyLocatorLite::upCast(keyLocator); }

  KeyLocatorLite&
  getKeyLocator() { return KeyLocatorLite::upCast(keyLocator); }

  void
  setType(ndn_SignatureType type) { this->type = type; }

  void
  setDigestAlgorithm(const BlobLite& digestAlgorithm)
  {
    BlobLite::upCast(this->digestAlgorithm) = digestAlgorithm;
  }

  void
  setWitness(const BlobLite& witness)
  {
    BlobLite::upCast(this->witness) = witness;
  }

  void
  setSignature(const BlobLite& signature)
  {
    BlobLite::upCast(this->signature) = signature;
  }

  /**
   * Upcast the reference to the ndn_Signature struct to a SignatureLite.
   * @param signature A reference to the ndn_Signature struct.
   * @return The same reference as SignatureLite.
   */
  static SignatureLite&
  upCast(ndn_Signature& signature) { return *(SignatureLite*)&signature; }

  static const SignatureLite&
  upCast(const ndn_Signature& signature) { return *(SignatureLite*)&signature; }
};

class MetaInfoLite : private ndn_MetaInfo {
public:
  MetaInfoLite()
  {
    ndn_MetaInfo_initialize(this);
  }

  ndn_MillisecondsSince1970
  getTimestampMilliseconds() const { return timestampMilliseconds; }
  
  ndn_ContentType 
  getType() const { return type; }
  
  ndn_Milliseconds
  getFreshnessPeriod() const { return freshnessPeriod; }
  
  const NameLite::Component
  getFinalBlockId() const { return NameLite::Component::upCast(finalBlockId); }

  void
  setTimestampMilliseconds(ndn_MillisecondsSince1970 timestampMilliseconds)
  {
    this->timestampMilliseconds = timestampMilliseconds;
  }

  void
  setType(ndn_ContentType type) { this->type = type; }

  void
  setFreshnessPeriod(ndn_Milliseconds freshnessPeriod)
  {
    this->freshnessPeriod = freshnessPeriod;
  }

  void
  setFinalBlockId(const NameLite::Component& finalBlockId)
  {
    NameLite::Component::upCast(this->finalBlockId) = finalBlockId;
  }

  /**
   * Upcast the reference to the ndn_MetaInfo struct to a MetaInfoLite.
   * @param metaInfo A reference to the ndn_MetaInfo struct.
   * @return The same reference as MetaInfoLite.
   */
  static MetaInfoLite&
  upCast(ndn_MetaInfo& metaInfo) { return *(MetaInfoLite*)&metaInfo; }

  static const MetaInfoLite&
  upCast(const ndn_MetaInfo& metaInfo) { return *(MetaInfoLite*)&metaInfo; }
};


class DataLite : private ndn_Data {
public:
  DataLite(ndn_NameComponent* nameComponents, size_t maxNameComponents,
   ndn_NameComponent* keyNameComponents, size_t maxKeyNameComponents)
  {
    ndn_Data_initialize
      (this, nameComponents, maxNameComponents, keyNameComponents,
       maxKeyNameComponents);
  }

  const SignatureLite&
  getSignature() const { return SignatureLite::upCast(signature); }

  SignatureLite&
  getSignature() { return SignatureLite::upCast(signature); }

  const NameLite&
  getName() const { return NameLite::upCast(name); }

  NameLite&
  getName() { return NameLite::upCast(name); }

  const MetaInfoLite&
  getMetaInfo() const { return MetaInfoLite::upCast(metaInfo); }

  MetaInfoLite&
  getMetaInfo() { return MetaInfoLite::upCast(metaInfo); }

  const BlobLite&
  getContent() const { return BlobLite::upCast(content); }

  void
  setContent(const BlobLite& content)
  {
    BlobLite::upCast(this->content) = content;
  }

  /**
   * Upcast the reference to the ndn_Data struct to a DataLite.
   * @param data A reference to the ndn_Data struct.
   * @return The same reference as DataLite.
   */
  static DataLite&
  upCast(ndn_Data& data) { return *(DataLite*)&data; }

  static const DataLite&
  upCast(const ndn_Data& data) { return *(DataLite*)&data; }
};

}
#endif

#endif
