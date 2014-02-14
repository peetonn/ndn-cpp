/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013-2014 Regents of the University of California.
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_KEY_LOCATOR_HPP
#define NDN_KEY_LOCATOR_HPP

#include <vector>
#include "c/key-types.h"
#include "name.hpp"
#include "util/change-counter.hpp"

struct ndn_KeyLocator;

namespace ndn {
  
class KeyLocator {
public:
  KeyLocator()
  : type_((ndn_KeyLocatorType)-1), keyNameType_((ndn_KeyNameType)-1), changeCount_(0)
  {
  }
  
  /**
   * Clear the keyData and set the type to none.
   */
  void 
  clear()
  {
    type_ = (ndn_KeyLocatorType)-1;
    keyNameType_ = (ndn_KeyNameType)-1;
    keyData_.reset();
    setKeyName(Name());
    ++changeCount_;
  }
  
  /**
   * Set the keyLocatorStruct to point to the values in this key locator, without copying any memory.
   * WARNING: The resulting pointers in keyLocatorStruct are invalid after a further use of this object which could reallocate memory.
   * @param keyLocatorStruct a C ndn_KeyLocator struct where the name components array is already allocated.
   */
  void 
  get(struct ndn_KeyLocator& keyLocatorStruct) const;
  
  /**
   * Clear this key locator, and set the values by copying from the ndn_KeyLocator struct.
   * @param keyLocatorStruct a C ndn_KeyLocator struct
   */
  void 
  set(const struct ndn_KeyLocator& keyLocatorStruct);

  ndn_KeyLocatorType 
  getType() const { return type_; }
  
  const Blob& 
  getKeyData() const { return keyData_; }

  const Name& 
  getKeyName() const { return keyName_.get(); }
  
  Name& 
  getKeyName() { return keyName_.get(); }

  /**
   * @deprecated The use of a digest attached to the KeyName is deprecated.  KEY_LOCATOR_DIGEST is supported
   * as a keyLocatorType.
   */
  ndn_KeyNameType 
  DEPRECATED_IN_NDN_CPP getKeyNameType() const { return keyNameType_; }

  void 
  setType(ndn_KeyLocatorType type) 
  { 
    type_ = type; 
    ++changeCount_;
  }
    
  void 
  setKeyData(const Blob& keyData) 
  { 
    keyData_ = keyData; 
    ++changeCount_;
  }

  void
  setKeyName(const Name &keyName) 
  { 
    keyName_.set(keyName); 
    ++changeCount_;
  }
  
  /**
   * @deprecated The use of a digest attached to the KeyName is deprecated.  
   * If you need a publisher public key digest, set the keyLocatorType to KEY_LOCATOR_DIGEST and set the key data
   * to the digest.
   */
  void 
  DEPRECATED_IN_NDN_CPP setKeyNameType(ndn_KeyNameType keyNameType) 
  { 
    keyNameType_ = keyNameType; 
    ++changeCount_;
  }

  /**
   * Get the change count, which is incremented each time this object (or a child object) is changed.
   * @return The change count.
   */
  uint64_t 
  getChangeCount() const
  {
    if (keyName_.checkChanged())
      // A child object has changed, so update the change count.
      // This method can be called on a const object, but we want to be able to update the changeCount_.
      ++const_cast<KeyLocator*>(this)->changeCount_;
    
    return changeCount_;    
  }

private:
  ndn_KeyLocatorType type_; /**< -1 for none */
  Blob keyData_; /**< An array for the key data as follows:
    *   If type_ is ndn_KeyLocatorType_KEY, the key data.
    *   If type_ is ndn_KeyLocatorType_CERTIFICATE, the certificate data. 
    *   If type_ is ndn_KeyLocatorType_KEY_LOCATOR_DIGEST, the digest data. 
    *   If type_ is ndn_KeyLocatorType_KEYNAME and keyNameType_ is ndn_KeyNameType_PUBLISHER_PUBLIC_KEY_DIGEST, the publisher public key digest. 
    *   If type_ is ndn_KeyLocatorType_KEYNAME and keyNameType_ is ndn_KeyNameType_PUBLISHER_CERTIFICATE_DIGEST, the publisher certificate digest. 
    *   If type_ is ndn_KeyLocatorType_KEYNAME and keyNameType_ is ndn_KeyNameType_PUBLISHER_ISSUER_KEY_DIGEST, the publisher issuer key digest. 
    *   If type_ is ndn_KeyLocatorType_KEYNAME and keyNameType_ is ndn_KeyNameType_PUBLISHER_ISSUER_CERTIFICATE_DIGEST, the publisher issuer certificate digest. 
                                */
  ChangeCounter<Name> keyName_; /**< The key name (only used if type_ is ndn_KeyLocatorType_KEYNAME.) */
  /** @deprecated The use of a digest attached to the KeyName is deprecated. */
  ndn_KeyNameType keyNameType_; /**< The type of data for keyName_, -1 for none. (only used if type_ is ndn_KeyLocatorType_KEYNAME.) */
  uint64_t changeCount_;
};
  
}

#endif
