
//  (C) Copyright Edward Diener 2012,2013
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#if !defined(NDNBOOST_TTI_HAS_FUNCTION_HPP)
#define NDNBOOST_TTI_HAS_FUNCTION_HPP

#include <ndnboost/config.hpp>
#include <ndnboost/function_types/property_tags.hpp>
#include <ndnboost/mpl/vector.hpp>
#include <ndnboost/preprocessor/cat.hpp>
#include <ndnboost/tti/detail/dfunction.hpp>
#include <ndnboost/tti/gen/has_function_gen.hpp>

/*

  The succeeding comments in this file are in doxygen format.

*/

/** \file
*/

/// Expands to a metafunction which tests whether a member function or a static member function with a particular name and signature exists.
/**

    trait = the name of the metafunction within the tti namespace.
    
    name  = the name of the inner member.

    generates a metafunction called "trait" where 'trait' is the macro parameter.
    
              template<class NDNBOOST_TTI_TP_T,class NDNBOOST_TTI_TP_R,class NDNBOOST_TTI_TP_FS,class NDNBOOST_TTI_TP_TAG>
              struct trait
                {
                static const value = unspecified;
                typedef mpl::bool_<true-or-false> type;
                };

              The metafunction types and return:
    
                NDNBOOST_TTI_TP_T   = the enclosing type in which to look for our 'name'.
                
                NDNBOOST_TTI_TP_R   = the return type of the function
                
                NDNBOOST_TTI_TP_FS  = (optional) the parameters of the function as a ndnboost::mpl forward sequence
                          if function parameters are not empty.
                
                NDNBOOST_TTI_TP_TAG = (optional) a ndnboost::function_types tag to apply to the function
                          if the need for a tag exists.
                
                returns = 'value' is true if the 'name' exists, 
                          with the appropriate static member function type,
                          otherwise 'value' is false.
                          
*/
#define NDNBOOST_TTI_TRAIT_HAS_FUNCTION(trait,name) \
  NDNBOOST_TTI_DETAIL_TRAIT_HAS_FUNCTION(trait,name) \
  template<class NDNBOOST_TTI_TP_T,class NDNBOOST_TTI_TP_R,class NDNBOOST_TTI_TP_FS = ndnboost::mpl::vector<>,class NDNBOOST_TTI_TP_TAG = ndnboost::function_types::null_tag> \
  struct trait \
    { \
    typedef typename \
    NDNBOOST_PP_CAT(trait,_detail_hf)<NDNBOOST_TTI_TP_T,NDNBOOST_TTI_TP_R,NDNBOOST_TTI_TP_FS,NDNBOOST_TTI_TP_TAG>::type type; \
    NDNBOOST_STATIC_CONSTANT(bool,value=type::value); \
    }; \
/**/

/// Expands to a metafunction which tests whether a member function or a static member function with a particular name and signature exists.
/**

    name  = the name of the inner member.

    generates a metafunction called "has_function_name" where 'name' is the macro parameter.
    
              template<class NDNBOOST_TTI_TP_T,class NDNBOOST_TTI_TP_R,class NDNBOOST_TTI_TP_FS,class NDNBOOST_TTI_TP_TAG>
              struct trait
                {
                static const value = unspecified;
                typedef mpl::bool_<true-or-false> type;
                };

              The metafunction types and return:
    
                NDNBOOST_TTI_TP_T   = the enclosing type in which to look for our 'name'.
                
                NDNBOOST_TTI_TP_R   = the return type of the function
                
                NDNBOOST_TTI_TP_FS  = (optional) the parameters of the function as a ndnboost::mpl forward sequence
                          if function parameters are not empty.
                
                NDNBOOST_TTI_TP_TAG = (optional) a ndnboost::function_types tag to apply to the function
                          if the need for a tag exists.
                
                returns = 'value' is true if the 'name' exists, 
                          with the appropriate function type,
                          otherwise 'value' is false.
                          
*/
#define NDNBOOST_TTI_HAS_FUNCTION(name) \
  NDNBOOST_TTI_TRAIT_HAS_FUNCTION \
  ( \
  NDNBOOST_TTI_HAS_FUNCTION_GEN(name), \
  name \
  ) \
/**/

#endif // NDNBOOST_TTI_HAS_FUNCTION_HPP
