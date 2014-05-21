/**
 * Copyright (C) 2014 Regents of the University of California.
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * A copy of the GNU General Public License is in the file COPYING.
 */

#include "tlv-name.h"
#include "tlv-key-locator.h"
#include "tlv-signature-info.h"
#include "tlv-data.h"

/**
 * This private function is called by ndn_TlvEncoder_writeTlv to write the TLVs in the body of the MetaInfo value.
 * @param context This is the ndn_MetaInfo struct pointer which was passed to writeTlv.
 * @param encoder the ndn_TlvEncoder which is calling this.
 * @return 0 for success, else an error code.
 */
static ndn_Error 
encodeMetaInfoValue(void *context, struct ndn_TlvEncoder *encoder)
{
  struct ndn_MetaInfo *metaInfo = (struct ndn_MetaInfo *)context;
  
  ndn_Error error;

  if (!((int)metaInfo->type < 0 || metaInfo->type == ndn_ContentType_BLOB)) {
    // Not the default, so we need to encode the type.
    if (metaInfo->type == ndn_ContentType_LINK || metaInfo->type == ndn_ContentType_KEY) {
      // The ContentType enum is set up with the correct integer for each NDN-TLV ContentType.
      if ((error = ndn_TlvEncoder_writeNonNegativeIntegerTlv
          (encoder, ndn_Tlv_ContentType, metaInfo->type)))
        return error;
    }
    else
      return NDN_ERROR_unrecognized_ndn_ContentType;
  }

  if ((error = ndn_TlvEncoder_writeOptionalNonNegativeIntegerTlvFromDouble
      (encoder, ndn_Tlv_FreshnessPeriod, metaInfo->freshnessPeriod)))
    return error;
  if (metaInfo->finalBlockID.value.value && 
      metaInfo->finalBlockID.value.length > 0) {
    // The FinalBlockID has an inner NameComponent.
    if ((error = ndn_TlvEncoder_writeTypeAndLength
         (encoder, ndn_Tlv_FinalBlockId, ndn_TlvEncoder_sizeOfBlobTlv
            (ndn_Tlv_NameComponent, &metaInfo->finalBlockID.value))))
      return error;
    if ((error = ndn_TlvEncoder_writeBlobTlv
         (encoder, ndn_Tlv_NameComponent, &metaInfo->finalBlockID.value)))
      return error;    
  }
    
  return NDN_ERROR_success;  
}

/* An DataValueContext is for passing the context to encodeDataValue so that we can include
 * signedPortionBeginOffset and signedPortionEndOffset.
 */
struct DataValueContext {
  struct ndn_Data *data;
  size_t *signedPortionBeginOffset;
  size_t *signedPortionEndOffset;
};

/**
 * This private function is called by ndn_TlvEncoder_writeTlv to write the TLVs in the body of the Data value.
 * @param context This is the DataValueContext struct pointer which was passed to writeTlv.
 * @param encoder the ndn_TlvEncoder which is calling this.
 * @return 0 for success, else an error code.
 */
static ndn_Error 
encodeDataValue(void *context, struct ndn_TlvEncoder *encoder)
{
  struct DataValueContext *dataValueContext = (struct DataValueContext *)context;
  struct ndn_Data *data = dataValueContext->data;
  ndn_Error error;
  size_t dummyBeginOffset, dummyEndOffset;
  
  *dataValueContext->signedPortionBeginOffset = encoder->offset;
  
  if ((error = ndn_encodeTlvName
       (&data->name, &dummyBeginOffset, &dummyEndOffset, encoder)))
    return error;
  if ((error = ndn_TlvEncoder_writeNestedTlv(encoder, ndn_Tlv_MetaInfo, encodeMetaInfoValue, &data->metaInfo, 0)))
    return error;
  if ((error = ndn_TlvEncoder_writeBlobTlv(encoder, ndn_Tlv_Content, &data->content)))
    return error;
  if ((error = ndn_encodeTlvSignatureInfo(&data->signature, encoder)))
    return error;  

  *dataValueContext->signedPortionEndOffset = encoder->offset;

  if ((error = ndn_TlvEncoder_writeBlobTlv(encoder, ndn_Tlv_SignatureValue, &data->signature.signature)))
    return error;  
  
  return NDN_ERROR_success;  
}

ndn_Error 
ndn_encodeTlvData
  (struct ndn_Data *data, size_t *signedPortionBeginOffset, size_t *signedPortionEndOffset, struct ndn_TlvEncoder *encoder)
{
  // Create the context to pass to encodeDataValue.
  struct DataValueContext dataValueContext;
  dataValueContext.data = data;
  dataValueContext.signedPortionBeginOffset = signedPortionBeginOffset;
  dataValueContext.signedPortionEndOffset = signedPortionEndOffset;
  
  return ndn_TlvEncoder_writeNestedTlv(encoder, ndn_Tlv_Data, encodeDataValue, &dataValueContext, 0);
}

static ndn_Error
decodeMetaInfo(struct ndn_MetaInfo *metaInfo, struct ndn_TlvDecoder *decoder)
{
  ndn_Error error;
  size_t endOffset;
  int gotExpectedType;

  if ((error = ndn_TlvDecoder_readNestedTlvsStart(decoder, ndn_Tlv_MetaInfo, &endOffset)))
    return error;

  // The ContentType enum is set up with the correct integer for each NDN-TLV ContentType.
  if ((error = ndn_TlvDecoder_readOptionalNonNegativeIntegerTlv
       (decoder, ndn_Tlv_ContentType, endOffset, (int *)&metaInfo->type)))
    return error;
  if ((int)metaInfo->type < 0)
    // Set to the actual value for the default.
    metaInfo->type = ndn_ContentType_BLOB;
  
  if ((error = ndn_TlvDecoder_readOptionalNonNegativeIntegerTlvAsDouble
       (decoder, ndn_Tlv_FreshnessPeriod, endOffset, &metaInfo->freshnessPeriod)))
    return error;
  
  if ((error = ndn_TlvDecoder_peekType
       (decoder, ndn_Tlv_FinalBlockId, endOffset, &gotExpectedType)))
    return error;    
  if (gotExpectedType) {
    size_t finalBlockIdEndOffset;
    if ((error = ndn_TlvDecoder_readNestedTlvsStart
         (decoder, ndn_Tlv_FinalBlockId, &finalBlockIdEndOffset)))
      return error;
    if ((error = ndn_TlvDecoder_readBlobTlv
         (decoder, ndn_Tlv_NameComponent, &metaInfo->finalBlockID.value)))
      return error;
    if ((error = ndn_TlvDecoder_finishNestedTlvs(decoder, finalBlockIdEndOffset)))
      return error;
  }
  else
    ndn_NameComponent_initialize(&metaInfo->finalBlockID, 0, 0);

  // Set fields not used by NDN-TLV to none.
  metaInfo->timestampMilliseconds = -1;
  
  if ((error = ndn_TlvDecoder_finishNestedTlvs(decoder, endOffset)))
    return error;

  return NDN_ERROR_success;    
}

ndn_Error 
ndn_decodeTlvData
  (struct ndn_Data *data, size_t *signedPortionBeginOffset, size_t *signedPortionEndOffset, struct ndn_TlvDecoder *decoder)
{
  ndn_Error error;
  size_t endOffset;
  if ((error = ndn_TlvDecoder_readNestedTlvsStart(decoder, ndn_Tlv_Data, &endOffset)))
    return error;
    
  *signedPortionBeginOffset = decoder->offset;

  if ((error = ndn_decodeTlvName(&data->name, decoder)))
    return error;
  if ((error = decodeMetaInfo(&data->metaInfo, decoder)))
    return error;
  if ((error = ndn_TlvDecoder_readBlobTlv(decoder, ndn_Tlv_Content, &data->content)))
    return error;
  if ((error = ndn_decodeTlvSignatureInfo(&data->signature, decoder)))
    return error;
  
  *signedPortionEndOffset = decoder->offset;
  
  if ((error = ndn_TlvDecoder_readBlobTlv(decoder, ndn_Tlv_SignatureValue, &data->signature.signature)))
    return error;  
      
  if ((error = ndn_TlvDecoder_finishNestedTlvs(decoder, endOffset)))
    return error;

  return NDN_ERROR_success;
}
