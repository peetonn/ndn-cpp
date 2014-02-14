/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013-2014 Regents of the University of California.
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#include <stdexcept>
#include <ndn-cpp/common.hpp>
#include "c/interest.h"
#include <ndn-cpp/interest.hpp>

using namespace std;

namespace ndn {
  
void 
Interest::set(const struct ndn_Interest& interestStruct) 
{
  name_.get().set(interestStruct.name);
  setMinSuffixComponents(interestStruct.minSuffixComponents);
  setMaxSuffixComponents(interestStruct.maxSuffixComponents);
  
  publisherPublicKeyDigest_.get().set(interestStruct.publisherPublicKeyDigest);
  keyLocator_.get().set(interestStruct.keyLocator);
  
  exclude_.get().set(interestStruct.exclude);
  setChildSelector(interestStruct.childSelector);
  answerOriginKind_ = interestStruct.answerOriginKind; 
  setScope(interestStruct.scope);
  setInterestLifetimeMilliseconds(interestStruct.interestLifetimeMilliseconds);
  // Set the nonce last so that getNonceChangeCount_ is set correctly.
  nonce_ = Blob(interestStruct.nonce);
  // Set getNonceChangeCount_ so that the next call to getNonce() won't clear nonce_.
  getNonceChangeCount_ = getChangeCount();
}

void 
Interest::get(struct ndn_Interest& interestStruct) const 
{
  name_.get().get(interestStruct.name);
  interestStruct.minSuffixComponents = minSuffixComponents_;
  interestStruct.maxSuffixComponents = maxSuffixComponents_;
  publisherPublicKeyDigest_.get().get(interestStruct.publisherPublicKeyDigest);
  keyLocator_.get().get(interestStruct.keyLocator);
  exclude_.get().get(interestStruct.exclude);
  interestStruct.childSelector = childSelector_;
  interestStruct.answerOriginKind = answerOriginKind_;
  interestStruct.scope = scope_;
  interestStruct.interestLifetimeMilliseconds = interestLifetimeMilliseconds_;
  getNonce().get(interestStruct.nonce);
}

string 
Interest::toUri() const
{
  ostringstream selectors;

  if (minSuffixComponents_ >= 0)
    selectors << "&ndn.MinSuffixComponents=" << minSuffixComponents_;
  if (maxSuffixComponents_ >= 0)
    selectors << "&ndn.MaxSuffixComponents=" << maxSuffixComponents_;
  if (childSelector_ >= 0)
    selectors << "&ndn.ChildSelector=" << childSelector_;
  if (answerOriginKind_ >= 0)
    selectors << "&ndn.AnswerOriginKind=" << answerOriginKind_;
  if (scope_ >= 0)
    selectors << "&ndn.Scope=" << scope_;
  if (interestLifetimeMilliseconds_ >= 0)
    selectors << "&ndn.InterestLifetime=" << interestLifetimeMilliseconds_;
  if (publisherPublicKeyDigest_.get().getPublisherPublicKeyDigest().size() > 0) {
    selectors << "&ndn.PublisherPublicKeyDigest=";
    Name::toEscapedString(*publisherPublicKeyDigest_.get().getPublisherPublicKeyDigest(), selectors);
  }
  if (getNonce().size() > 0) {
    selectors << "&ndn.Nonce=";
    Name::toEscapedString(*getNonce(), selectors);
  }
  if (exclude_.get().size() > 0)
    selectors << "&ndn.Exclude=" << exclude_.get().toUri();

  ostringstream result;

  result << name_.get().toUri();
  string selectorsString(selectors.str());
  if (selectorsString.size() > 0) {
    // Replace the first & with ?.
    result << "?";
    result.write(&selectorsString[1], selectorsString.size() - 1);
  }
  
  return result.str();  
}

}

