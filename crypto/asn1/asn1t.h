/* asn1t.h */
/* Written by Dr Stephen N Henson (shenson@bigfoot.com) for the OpenSSL
 * project 2000.
 */
/* ====================================================================
 * Copyright (c) 2000 The OpenSSL Project.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer. 
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * 3. All advertising materials mentioning features or use of this
 *    software must display the following acknowledgment:
 *    "This product includes software developed by the OpenSSL Project
 *    for use in the OpenSSL Toolkit. (http://www.OpenSSL.org/)"
 *
 * 4. The names "OpenSSL Toolkit" and "OpenSSL Project" must not be used to
 *    endorse or promote products derived from this software without
 *    prior written permission. For written permission, please contact
 *    licensing@OpenSSL.org.
 *
 * 5. Products derived from this software may not be called "OpenSSL"
 *    nor may "OpenSSL" appear in their names without prior written
 *    permission of the OpenSSL Project.
 *
 * 6. Redistributions of any form whatsoever must retain the following
 *    acknowledgment:
 *    "This product includes software developed by the OpenSSL Project
 *    for use in the OpenSSL Toolkit (http://www.OpenSSL.org/)"
 *
 * THIS SOFTWARE IS PROVIDED BY THE OpenSSL PROJECT ``AS IS'' AND ANY
 * EXPRESSED OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE OpenSSL PROJECT OR
 * ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 * ====================================================================
 *
 * This product includes cryptographic software written by Eric Young
 * (eay@cryptsoft.com).  This product includes software written by Tim
 * Hudson (tjh@cryptsoft.com).
 *
 */
#ifndef HEADER_ASN1T_H
#define HEADER_ASN1T_H

#include <stddef.h>
#include <openssl/asn1.h>

/* ASN1 template defines, structures and functions */

#ifdef  __cplusplus
extern "C" {
#endif

/* Macros to aid ASN1 template writing */

#define ASN1_ITEM_TEMPLATE(tname) \
	const static ASN1_TEMPLATE tname##_item_tt 

#define ASN1_ITEM_TEMPLATE_END(tname) \
	;\
	const ASN1_ITEM tname##_it = { \
		ASN1_ITYPE_PRIMITIVE,\
		-1,\
		&tname##_item_tt,\
		0,\
		NULL,\
		0,\
		#tname \
	}


/* This is a ASN1 type which just embeds a template */
 
/* This pair helps declare a SEQUENCE. We can do:
 *
 * 	ASN1_SEQUENCE(stname) = {
 * 		... SEQUENCE components ...
 * 	} ASN1_SEQUENCE_END(stname);
 *
 * 	This will produce an ASN1_ITEM called stname_it
 *	for a structure called stname.
 *
 * 	If you want the same structure but a different
 *	name then use:
 *
 * 	ASN1_SEQUENCE(itname) = {
 * 		... SEQUENCE components ...
 * 	} ASN1_SEQUENCE_END_name(stname, itname);
 *
 *	This will create an item called itname_it using
 *	a structure called stname.
 */

#define ASN1_SEQUENCE(tname) \
	const static ASN1_TEMPLATE tname##_seq_tt[] 

#define ASN1_SEQUENCE_END(stname) ASN1_SEQUENCE_END_name(stname, stname)

#define ASN1_SEQUENCE_END_name(stname, tname) \
	;\
	const ASN1_ITEM tname##_it = { \
		ASN1_ITYPE_SEQUENCE,\
		V_ASN1_SEQUENCE,\
		tname##_seq_tt,\
		sizeof(tname##_seq_tt) / sizeof(ASN1_TEMPLATE),\
		NULL,\
		sizeof(stname),\
		#stname \
	}

#define ASN1_SEQUENCE_cb(tname, cb) \
	const static ASN1_AUX tname##_aux = {NULL, 0, 0, 0, cb}; \
	ASN1_SEQUENCE(tname)

#define ASN1_SEQUENCE_ref(tname, cb, lck) \
	const static ASN1_AUX tname##_aux = {NULL, ASN1_AFLG_REFCOUNT, offsetof(tname, references), lck, cb}; \
	ASN1_SEQUENCE(tname)

#define ASN1_SEQUENCE_END_cb(stname, tname) ASN1_SEQUENCE_END_ref(stname, tname)

#define ASN1_SEQUENCE_END_ref(stname, tname) \
	;\
	const ASN1_ITEM tname##_it = { \
		ASN1_ITYPE_SEQUENCE,\
		V_ASN1_SEQUENCE,\
		tname##_seq_tt,\
		sizeof(tname##_seq_tt) / sizeof(ASN1_TEMPLATE),\
		&tname##_aux,\
		sizeof(stname),\
		#stname \
	}


/* This pair helps declare a CHOICE type. We can do:
 *
 * 	ASN1_CHOICE(chname) = {
 * 		... CHOICE options ...
 * 	ASN1_CHOICE_END(chname);
 *
 * 	This will produce an ASN1_ITEM called chname_it
 *	for a structure called chname. The structure
 *	definition must look like this:
 *	typedef struct {
 *		int type;
 *		union {
 *			ASN1_SOMETHING *opt1;
 *			ASN1_SOMEOTHER *opt2;
 *		} value;
 *	} chname;
 *	
 *	the name of the selector must be 'type'.
 * 	to use an alternative selector name use the
 *      ASN1_CHOICE_END_selector() version.
 */

#define ASN1_CHOICE(tname) \
	const static ASN1_TEMPLATE tname##_ch_tt[] 

#define ASN1_CHOICE_END(stname) ASN1_CHOICE_END_name(stname, stname)

#define ASN1_CHOICE_END_name(stname, tname) ASN1_CHOICE_END_selector(stname, tname, type)

#define ASN1_CHOICE_END_selector(stname, tname, selname) \
	;\
	const ASN1_ITEM tname##_it = { \
		ASN1_ITYPE_CHOICE,\
		offsetof(stname,selname) ,\
		tname##_ch_tt,\
		sizeof(tname##_ch_tt) / sizeof(ASN1_TEMPLATE),\
		NULL,\
		sizeof(stname),\
		#stname \
	}

/* This helps with the template wrapper form of ASN1_ITEM */

#define ASN1_EX_TEMPLATE_TYPE(flags, tag, name, type) { \
	(flags), (tag), 0,\
	#name, &(type##_it) }

/* These help with SEQUENCE or CHOICE components */

/* used to declare other types */

#define ASN1_EX_TYPE(flags, tag, stname, field, type) { \
	(flags), (tag), offsetof(stname, field),\
	#field, &(type##_it) }

/* used when the structure is combined with the parent */

#define ASN1_EX_COMBINE(flags, tag, type) { \
	(flags)|ASN1_TFLG_COMBINE, (tag), 0, NULL, &(type##_it) }

/* implicit and explicit helper macros */

#define ASN1_IMP_EX(stname, field, type, tag, ex) \
		ASN1_EX_TYPE(ASN1_TFLG_IMPLICIT | ex, tag, stname, field, type)

#define ASN1_EXP_EX(stname, field, type, tag, ex) \
		ASN1_EX_TYPE(ASN1_TFLG_EXPLICIT | ex, tag, stname, field, type)

/* Any defined by macros: the field used is in the table itself */

#define ASN1_ADB_OBJECT(tblname) { ASN1_TFLG_ADB_OID, -1, 0, #tblname, &(tblname##_adb) }
#define ASN1_ADB_INTEGER(tblname) { ASN1_TFLG_ADB_INT, -1, 0, #tblname, &(tblname##_adb) }

/* Plain simple type */
#define ASN1_SIMPLE(stname, field, type) ASN1_EX_TYPE(0,0, stname, field, type)

/* OPTIONAL simple type */
#define ASN1_OPT(stname, field, type) ASN1_EX_TYPE(ASN1_TFLG_OPTIONAL, 0, stname, field, type)

/* IMPLICIT tagged simple type */
#define ASN1_IMP(stname, field, type, tag) ASN1_IMP_EX(stname, field, type, tag, 0)

/* IMPLICIT tagged OPTIONAL simple type */
#define ASN1_IMP_OPT(stname, field, type, tag) ASN1_IMP_EX(stname, field, type, tag, ASN1_TFLG_OPTIONAL)

/* Same as above but EXPLICIT */

#define ASN1_EXP(stname, field, type, tag) ASN1_EXP_EX(stname, field, type, tag, 0)
#define ASN1_EXP_OPT(stname, field, type, tag) ASN1_EXP_EX(stname, field, type, tag, ASN1_TFLG_OPTIONAL)

/* SEQUENCE OF type */
#define ASN1_SEQUENCE_OF(stname, field, type) \
		ASN1_EX_TYPE(ASN1_TFLG_SEQUENCE_OF, 0, stname, field, type)

/* OPTIONAL SEQUENCE OF */
#define ASN1_SEQUENCE_OF_OPT(stname, field, type) \
		ASN1_EX_TYPE(ASN1_TFLG_SEQUENCE_OF|ASN1_TFLG_OPTIONAL, 0, stname, field, type)

/* Same as above but for SET OF */

#define ASN1_SET_OF(stname, field, type) \
		ASN1_EX_TYPE(ASN1_TFLG_SET_OF, 0, stname, field, type)

#define ASN1_SET_OF_OPT(stname, field, type) \
		ASN1_EX_TYPE(ASN1_TFLG_SET_OF|ASN1_TFLG_OPTIONAL, 0, stname, field, type)

/* Finally compound types of SEQUENCE, SET, IMPLICIT, EXPLICIT and OPTIONAL */

#define ASN1_IMP_SET_OF(stname, field, type, tag) \
			ASN1_IMP_EX(stname, field, type, tag, ASN1_TFLG_SET_OF)

#define ASN1_EXP_SET_OF(stname, field, type, tag) \
			ASN1_EXP_EX(stname, field, type, tag, ASN1_TFLG_SET_OF)

#define ASN1_IMP_SET_OF_OPT(stname, field, type, tag) \
			ASN1_IMP_EX(stname, field, type, tag, ASN1_TFLG_SET_OF|ASN1_TFLG_OPTIONAL)

#define ASN1_EXP_SET_OF_OPT(stname, field, type, tag) \
			ASN1_EXP_EX(stname, field, type, tag, ASN1_TFLG_SET_OF|ASN1_TFLG_OPTIONAL)

#define ASN1_IMP_SEQUENCE_OF(stname, field, type, tag) \
			ASN1_IMP_EX(stname, field, type, tag, ASN1_TFLG_SEQUENCE_OF)

#define ASN1_IMP_SEQUENCE_OF_OPT(stname, field, type, tag) \
			ASN1_IMP_EX(stname, field, type, tag, ASN1_TFLG_SEQUENCE_OF|ASN1_TFLG_OPTIONAL)

#define ASN1_EXP_SEQUENCE_OF(stname, field, type, tag) \
			ASN1_EXP_EX(stname, field, type, tag, ASN1_TFLG_SEQUENCE_OF)

#define ASN1_EXP_SEQUENCE_OF_OPT(stname, field, type, tag) \
			ASN1_EXP_EX(stname, field, type, tag, ASN1_TFLG_SEQUENCE_OF|ASN1_TFLG_OPTIONAL)

/* Macros for the ASN1_ADB structure */

#define ASN1_ADB(name) \
	const static ASN1_ADB_TABLE name##_adbtbl[] 

#define ASN1_ADB_END(name, flags, field, app_table, def, none) \
	;\
	const static ASN1_ADB name##_adb = {\
		flags,\
		offsetof(name, field),\
		app_table,\
		name##_adbtbl,\
		sizeof(name##_adbtbl) / sizeof(ASN1_ADB_TABLE),\
		def,\
		none\
	}

#define ADB_ENTRY(val, template) {val, template}

#define ASN1_ADB_TEMPLATE(name) \
	const static ASN1_TEMPLATE name##_tt 

/* This is the ASN1 template structure that defines
 * a wrapper round the actual type. It determines the
 * actual position of the field in the value structure,
 * various flags such as OPTIONAL and the field name.
 */

struct ASN1_TEMPLATE_st {
unsigned long flags;		/* Various flags */
long tag;			/* tag, not used if no tagging */
unsigned long offset;		/* Offset of this field in structure */
#ifndef NO_ASN1_FIELD_NAMES
char *field_name;		/* Field name */
#endif
const void *item;		/* Relevant ASN1_ITEM or ASN1_ADB */
};


typedef struct ASN1_ADB_TABLE_st ASN1_ADB_TABLE;
typedef struct ASN1_ADB_st ASN1_ADB;

struct ASN1_ADB_st {
	unsigned long flags;	/* Various flags */
	unsigned long offset;	/* Offset of selector field */
	STACK_OF(ASN1_ADB_TABLE) **app_items; /* Application defined items */
	const ASN1_ADB_TABLE *tbl;	/* Table of possible types */
	long tblcount;		/* Number of entries in tbl */
	const ASN1_TEMPLATE *default_tt;  /* Type to use if no match */
	const ASN1_TEMPLATE *null_tt;  /* Type to use if selector is NULL */
};

struct ASN1_ADB_TABLE_st {
	long value;		/* NID for an object or value for an int */
	const ASN1_TEMPLATE tt;		/* item for this value */
};

/* template flags */

/* Field is optional */
#define ASN1_TFLG_OPTIONAL	(0x1)

/* Field is a SET OF */
#define ASN1_TFLG_SET_OF	(0x1 << 1)

/* Field is a SEQUENCE OF */
#define ASN1_TFLG_SEQUENCE_OF	(0x2 << 1)

#define ASN1_TFLG_SK_MASK	(0x3 << 1)

/* These flags mean the tag should be taken from the
 * tag field. If EXPLICIT then the underlying type
 * is used for the inner tag.
 */

/* IMPLICIT tagging */
#define ASN1_TFLG_IMPTAG	(0x1 << 3)


/* EXPLICIT tagging, inner tag from underlying type */
#define ASN1_TFLG_EXPTAG	(0x2 << 3)

#define ASN1_TFLG_TAG_MASK	(0x3 << 3)

/* context specific IMPLICIT */
#define ASN1_TFLG_IMPLICIT	ASN1_TFLG_IMPTAG|ASN1_TFLG_CONTEXT

/* context specific EXPLICIT */
#define ASN1_TFLG_EXPLICIT	ASN1_TFLG_EXPTAG|ASN1_TFLG_CONTEXT

/* If tagging is in force these determine the
 * type of tag to use. Otherwise the tag is
 * determined by the underlying type. These 
 * values reflect the actual octet format.
 */

/* Universal tag */ 
#define ASN1_TFLG_UNIVERSAL	(0x0<<6)
/* Application tag */ 
#define ASN1_TFLG_APPLICATION	(0x1<<6)
/* Context specific tag */ 
#define ASN1_TFLG_CONTEXT	(0x2<<6)
/* Private tag */ 
#define ASN1_TFLG_PRIVATE	(0x3<<6)

#define ASN1_TFLG_TAG_CLASS	(0x3<<6)

/* These are for ANY DEFINED BY type. In this case
 * the 'item' field points to an ASN1_ADB structure
 * which contains a table of values to decode the
 * relevant type
 */

#define ASN1_TFLG_ADB_MASK	(0x3<<8)

#define ASN1_TFLG_ADB_OID	(0x1<<8)

#define ASN1_TFLG_ADB_INT	(0x1<<9)

/* This flag means a parent structure is passed
 * instead of the field: this is useful is a
 * SEQUENCE is being combined with a CHOICE for
 * example. Since this means the structure and
 * item name will differ we need to use the
 * ASN1_CHOICE_END_name() macro for example.
 */

#define ASN1_TFLG_COMBINE	(0x1<<10)

/* This is the actual ASN1 item itself */

struct ASN1_ITEM_st {
char itype;			/* The item type, primitive, SEQUENCE, CHOICE or extern */
long utype;			/* underlying type */
const ASN1_TEMPLATE *templates;	/* If SEQUENCE or CHOICE this contains the contents */
long tcount;			/* Number of templates if SEQUENCE or CHOICE */
const void *funcs;		/* functions that handle this type */
long size;			/* Structure size (usually)*/
#ifndef NO_ASN1_FIELD_NAMES
const char *sname;		/* Structure name */
#endif
};

/* These are values for the itype field and
 * determine how the type is interpreted.
 *
 * For PRIMITIVE types the underlying type
 * determines the behaviour if items is NULL.
 *
 * Otherwise templates must contain a single 
 * template and the type is treated in the
 * same way as the type specified in the template.
 *
 * For SEQUENCE types the templates field points
 * to the members, the size field is the
 * structure size.
 *
 * For CHOICE types the templates field points
 * to each possible member (typically a union)
 * and the 'size' field is the offset of the
 * selector.
 *
 * The 'funcs' field is used for application
 * specific functions. 
 *
 * For COMPAT types the funcs field gives a
 * set of functions that handle this type, this
 * supports the old d2i, i2d convention.
 *
 * The EXTERN type uses a new style d2i/i2d.
 * The new style should be used where possible
 * because it avoids things like the d2i IMPLICIT
 * hack.
 *
 * MSTRING is a multiple string type, it is used
 * for a CHOICE of character strings where the
 * actual strings all occupy an ASN1_STRING
 * structure. In this case the 'utype' field
 * has a special meaning, it is used as a mask
 * of acceptable types using the B_ASN1 constants.
 *
 */

#define ASN1_ITYPE_PRIMITIVE	0x0

#define ASN1_ITYPE_SEQUENCE	0x1

#define ASN1_ITYPE_CHOICE	0x2

#define ASN1_ITYPE_COMPAT	0x3

#define ASN1_ITYPE_EXTERN	0x4

#define ASN1_ITYPE_MSTRING	0x5

/* Cache for ASN1 tag and length, so we
 * don't keep re-reading it for things
 * like CHOICE
 */

struct ASN1_TLC_st{
	char valid;	/* Values below are valid */
	int ret;	/* return value */
	long plen;	/* length */
	int ptag;	/* class value */
	int pclass;	/* class value */
	int hdrlen;	/* header length */
};

/* Typedefs for ASN1 functions */

typedef ASN1_VALUE * ASN1_new_func(void);
typedef void ASN1_free_func(ASN1_VALUE *a);
typedef ASN1_VALUE * ASN1_d2i_func(ASN1_VALUE **a, unsigned char ** in, long length);
typedef int ASN1_i2d_func(ASN1_VALUE * a, unsigned char **in);

typedef int ASN1_ex_d2i(ASN1_VALUE **val, unsigned char **in, long len, const ASN1_ITEM *it,
					int tag, int aclass, char opt, ASN1_TLC *ctx);

typedef int ASN1_ex_i2d(ASN1_VALUE *val, unsigned char **out, const ASN1_ITEM *it, int tag, int aclass);
typedef int ASN1_ex_new_func(ASN1_VALUE **val, const ASN1_ITEM *it);
typedef void ASN1_ex_free_func(ASN1_VALUE *val, const ASN1_ITEM *it);

typedef struct ASN1_COMPAT_FUNCS_st {
	ASN1_new_func *asn1_new;
	ASN1_free_func *asn1_free;
	ASN1_d2i_func *asn1_d2i;
	ASN1_i2d_func *asn1_i2d;
} ASN1_COMPAT_FUNCS;

typedef struct ASN1_EXTERN_FUNCS_st {
	void *app_data;
	ASN1_ex_new_func *asn1_ex_new;
	ASN1_ex_free_func *asn1_ex_free;
	ASN1_ex_d2i *asn1_ex_d2i;
	ASN1_ex_i2d *asn1_ex_i2d;
} ASN1_EXTERN_FUNCS;

/* This is the ASN1_AUX structure: it handles various
 * miscellaneous requirements. For example the use of
 * reference counts and an informational callback.
 *
 * The "informational callback" is called at various
 * points during the ASN1 encoding and decoding. It can
 * be used to provide minor customisation of the structures
 * used. This is most useful where the supplied routines
 * *almost* do the right thing but need some extra help
 * at a few points. If the callback returns zero then
 * it is assumed a fatal error has occurred and the 
 * main operation should be abandoned.
 *
 * If major changes in the default behaviour are required
 * then an external type is more appropriate.
 */

typedef int ASN1_aux_cb(int operation, ASN1_VALUE **in, const ASN1_ITEM *it);

typedef struct ASN1_AUX_st {
	void *app_data;
	int flags;
	int ref_offset;		/* Offset of reference value */
	int ref_lock;		/* Lock type to use */
	ASN1_aux_cb *asn1_cb;
} ASN1_AUX;

/* Flags in ASN1_AUX */

/* Use a reference count */
#define ASN1_AFLG_REFCOUNT	1

/* operation values for asn1_cb */

#define ASN1_OP_NEW_PRE		0
#define ASN1_OP_NEW_POST	1
#define ASN1_OP_FREE_PRE	2
#define ASN1_OP_FREE_POST	3
#define ASN1_OP_D2I_PRE		4
#define ASN1_OP_D2I_POST	5
#define ASN1_OP_I2D_PRE		6
#define ASN1_OP_I2D_POST	7

/* Macro to implement a primitive type */
#define IMPLEMENT_ASN1_TYPE(stname) IMPLEMENT_ASN1_TYPE_name(stname, stname)
#define IMPLEMENT_ASN1_TYPE_name(stname, itname) const ASN1_ITEM itname##_it = \
				{ ASN1_ITYPE_PRIMITIVE, V_##itname, NULL, 0, NULL, sizeof(stname), #itname};
/* Macro to implement a multi string type */
#define IMPLEMENT_ASN1_MSTRING(itname, mask) const ASN1_ITEM itname##_it = \
				{ ASN1_ITYPE_MSTRING, mask, NULL, 0, NULL, sizeof(ASN1_STRING), #itname};

/* Macro to implement an ASN1_ITEM in terms of old style funcs */

#define IMPLEMENT_COMPAT_ASN1(sname) IMPLEMENT_COMPAT_ASN1_type(sname, V_ASN1_SEQUENCE)

#define IMPLEMENT_COMPAT_ASN1_type(sname, tag) \
	static const ASN1_COMPAT_FUNCS sname##_ff = { \
		(ASN1_new_func *)sname##_new, \
		(ASN1_free_func *)sname##_free, \
		(ASN1_d2i_func *)d2i_##sname, \
		(ASN1_i2d_func *)i2d_##sname, \
	}; \
	ASN1_ITEM const sname##_it = { \
		ASN1_ITYPE_COMPAT, \
		tag, \
		NULL, \
		0, \
		&sname##_ff, \
		0, \
		#sname \
	}

#define IMPLEMENT_EXTERN_ASN1(sname, tag, fptrs) \
	const ASN1_ITEM sname##_it = { \
		ASN1_ITYPE_EXTERN, \
		tag, \
		NULL, \
		0, \
		&fptrs, \
		0, \
		#sname \
	};

/* Macro to implement standard functions in terms of ASN1_ITEM structures */

#define IMPLEMENT_ASN1_FUNCTIONS(stname) IMPLEMENT_ASN1_FUNCTIONS_fname(stname, stname, stname)

#define IMPLEMENT_ASN1_FUNCTIONS_name(stname, itname) IMPLEMENT_ASN1_FUNCTIONS_fname(stname, itname, itname)

#define IMPLEMENT_ASN1_FUNCTIONS_fname(stname, itname, fname) \
	stname *fname##_new(void) \
	{ \
		return (stname *)ASN1_item_new(&itname##_it); \
	} \
	void fname##_free(stname *a) \
	{ \
		ASN1_item_free((ASN1_VALUE *)a, &itname##_it); \
	} \
	stname *d2i_##fname(stname **a, unsigned char **in, long len) \
	{ \
		return (stname *)ASN1_item_d2i((ASN1_VALUE **)a, in, len, &itname##_it);\
	} \
	int i2d_##fname(stname *a, unsigned char **out) \
	{ \
		return ASN1_item_i2d((ASN1_VALUE *)a, out, &itname##_it);\
	} 

/* external definitions for primitive types */

extern const ASN1_ITEM ASN1_BOOLEAN_it;
extern const ASN1_ITEM ASN1_OBJECT_it;
extern const ASN1_ITEM ASN1_ANY_it;
extern const ASN1_ITEM ASN1_SEQUENCE_it;

int asn1_do_lock(ASN1_VALUE *pval, int op, const ASN1_ITEM *it);

#ifdef  __cplusplus
}
#endif
#endif
