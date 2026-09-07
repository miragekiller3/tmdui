#ifndef _SAL_H
#define _SAL_H


#define __RPC__in           
#define __RPC__in_string
#define __RPC__in_opt_string
#define __RPC__deref_opt_in_opt
#define __RPC__opt_in_opt_string
#define __RPC__in_ecount(size) 
#define __RPC__in_ecount_full(size)
#define __RPC__in_ecount_full_string(size)
#define __RPC__in_ecount_part(size, length)
#define __RPC__in_ecount_full_opt(size)
#define __RPC__in_ecount_full_opt_string(size)
#define __RPC__inout_ecount_full_opt_string(size)
#define __RPC__in_ecount_part_opt(size, length)

#define __RPC__deref_in 
#define __RPC__deref_in_string
#define __RPC__deref_opt_in
#define __RPC__deref_in_opt
#define __RPC__deref_in_ecount(size) 
#define __RPC__deref_in_ecount_part(size, length) 
#define __RPC__deref_in_ecount_full(size) 
#define __RPC__deref_in_ecount_full_opt(size)
#define __RPC__deref_in_ecount_full_string(size)
#define __RPC__deref_in_ecount_full_opt_string(size)
#define __RPC__deref_in_ecount_opt(size) 
#define __RPC__deref_in_ecount_opt_string(size)
#define __RPC__deref_in_ecount_part_opt(size, length) 

// [out]
#define __RPC__out     
#define __RPC__out_ecount(size) 
#define __RPC__out_ecount_part(size, length) 
#define __RPC__out_ecount_full(size)
#define __RPC__out_ecount_full_string(size)

// [in,out] 
#define __RPC__inout                                   
#define __RPC__inout_string
#define __RPC__opt_inout
#define __RPC__inout_ecount(size)                     
#define __RPC__inout_ecount_part(size, length)    
#define __RPC__inout_ecount_full(size)          
#define __RPC__inout_ecount_full_string(size)          

// [in,unique] 
#define __RPC__in_opt       
#define __RPC__in_ecount_opt(size)   


// [in,out,unique] 
#define __RPC__inout_opt    
#define __RPC__inout_ecount_opt(size)  
#define __RPC__inout_ecount_part_opt(size, length) 
#define __RPC__inout_ecount_full_opt(size)     
#define __RPC__inout_ecount_full_string(size)

// [out] **
#define __RPC__deref_out   
#define __RPC__deref_out_string
#define __RPC__deref_out_opt 
#define __RPC__deref_out_opt_string
#define __RPC__deref_out_ecount(size) 
#define __RPC__deref_out_ecount_part(size, length) 
#define __RPC__deref_out_ecount_full(size)  
#define __RPC__deref_out_ecount_full_string(size)


// [in,out] **, second pointer decoration. 
#define __RPC__deref_inout    
#define __RPC__deref_inout_string
#define __RPC__deref_inout_opt 
#define __RPC__deref_inout_opt_string
#define __RPC__deref_inout_ecount_full(size)
#define __RPC__deref_inout_ecount_full_string(size)
#define __RPC__deref_inout_ecount_opt(size) 
#define __RPC__deref_inout_ecount_part_opt(size, length) 
#define __RPC__deref_inout_ecount_full_opt(size) 
#define __RPC__deref_inout_ecount_full_opt_string(size) 

// #define __RPC_out_opt    out_opt is not allowed in rpc

// [in,out,unique] 
#define __RPC__deref_opt_inout  
#define __RPC__deref_opt_inout_string
#define __RPC__deref_opt_inout_ecount(size)     
#define __RPC__deref_opt_inout_ecount_part(size, length) 
#define __RPC__deref_opt_inout_ecount_full(size) 
#define __RPC__deref_opt_inout_ecount_full_string(size)

#define __RPC__deref_out_ecount_opt(size) 
#define __RPC__deref_out_ecount_part_opt(size, length) 
#define __RPC__deref_out_ecount_full_opt(size) 
#define __RPC__deref_out_ecount_full_opt_string(size)

#define __RPC__deref_opt_inout_opt      
#define __RPC__deref_opt_inout_opt_string
#define __RPC__deref_opt_inout_ecount_opt(size)   
#define __RPC__deref_opt_inout_ecount_part_opt(size, length) 
#define __RPC__deref_opt_inout_ecount_full_opt(size) 
#define __RPC__deref_opt_inout_ecount_full_opt_string(size) 

#define __RPC_full_pointer  
#define __RPC_unique_pointer
#define __RPC_ref_pointer
#define __RPC_string    


#define __in
#define __in_opt
#define __in_deref()
#define __in_deref_opt()
#define __in_bcount(s)
#define __in_ecount(s)
#define __deref_out
#define __deref_opt_out
#define __deref_out_ecount(s)
#define __in_xcount(s)
#define __out
#define __out_opt
#define __out_deref()
#define __out_deref_opt()
#define __out_bcount(s)
#define __out_ecount(s)
#define __inout_ecount_opt(s)
#define __out_xcount(s)
#define __out_full()
#define __out_part()
#define INOUT()
#define __inout
#define __inout_opt
#define __inout_deref()
#define __inout_deref_opt()
#define __inout_bcount(s)
#define __inout_ecount(s)
#define __inout_xcount(s)
#define __inout_full()
#define __inout_part()

#define __opt()
#define __attribute__()
#define __gc()
#define __if_exists(x) __if_exists

#define _ecount(s)
#define _bcount(s)
#define _full(s)
#define _part(s, l)
#define __nullterminated()
#define __nullnullterminated()
#define __possibly_notnullterminated()
#define __checkReturn
#define __reserved()


#define __in_bcount_opt(s)
#define __in_ecount_opt(s)
#define __out_bcount_opt(s)
#define __out_ecount_opt(s)

#define _In_
#define _In_opt_
#define _In_reads_(s)
#define _In_reads_opt_(s)
#define _In_count_(s)
#define _In_range_(s1,s2)
#define _In_reads_bytes_(s)

#define _Out_
#define _Out_opt_
#define _Out_writes_bytes_to_(s1,s2) 
#define _Out_writes_all_opt_(s)
#define _Out_writes_to_opt_(s1,s2)
#define _Out_writes_to_(s1,s2)
#define _Out_writes_(s)
#define _Out_writes_all_(s)
#define _Out_writes_bytes_(s)

#define _Inout_
#define _Inout_updates_bytes_(s)

#define _Outptr_
#define _Outptr_result_maybenull_

#define _COM_Outptr_
#define _COM_Outptr_result_maybenull_
#define _COM_Outptr_opt_result_maybenull_
#define _Field_size_full_opt_(s)
#define _Field_size_opt_(s)

#define _Out_writes_z_(s)
#define _Out_writes_opt_(s)
#define _Outptr_result_bytebuffer_(s)
#define _Outptr_result_buffer_(s)
#define _Field_size_(s)
#define _In_z_
#define _In_opt_z_
#define _Field_z_
#define _Notnull_
#define _Outptr_result_z_


#define DECLSPEC_SELECTANY
#define DECLSPEC_NOVTABLE
#define COM_DECLSPEC_NOTHROW
#define FORCEINLINE inline

#define _D2D1_HELPER_H_ // don't include d2d1heler.h
#define _D2D1_1HELPER_H_ // don't include d2d1_1heler.h
//#define _D2D1_EFFECTS_  // don't include d2d1effects.h
#define SPECSTRINGS_H // don't include specstrings.h

#define nullptr 0

#define DECLARE_INTERFACE_IID_(iface, baseiface, iid)   interface DECLSPEC_UUID(iid) DECLSPEC_NOVTABLE iface : public baseiface

#pragma warning (disable : 4068)     // warning C4068: unknown pragma


#ifndef DEFINE_ENUM_FLAG_OPERATORS
#   define DEFINE_ENUM_FLAG_OPERATORS(x) TM_ENUM_2_FLAG(x)
#endif


// for UIAnimation.h
// #define __checkReturn


#endif // #ifndef _SAL_H
