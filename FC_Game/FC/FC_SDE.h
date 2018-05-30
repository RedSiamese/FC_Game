#ifndef FC_SDE_H
#define FC_SDE_H

//#undef __cplusplus
//#undef WIN32

#define CATCH_TO_STRING(x) #x
#define CATCH(x) CATCH_TO_STRING(x)
#define INCLUDE_FILE_AND_LINE(string) __FILE__"(" CATCH(__LINE__) "): Freescale: "string

#pragma message( INCLUDE_FILE_AND_LINE("Software Development Environment in ") __FILE__ ) 
#pragma message( INCLUDE_FILE_AND_LINE("Last modified on ") __TIMESTAMP__ ) 

#ifdef WIN32

#define PID_USING_DOUBLE
#define K_MEANS_USING_LONG

#ifdef _DEBUG

//#define WIN_DEBUG
#pragma message(INCLUDE_FILE_AND_LINE("Using win debug!"))

#endif // _DEBUG

#define ___inline inline
#pragma message(INCLUDE_FILE_AND_LINE("Inline usable in release!"))

#else

#ifdef __STDC_VERSION__
#if __STDC_VERSION__>=199901L

#define ___inline inline
#pragma message(INCLUDE_FILE_AND_LINE("Inline usable!"))

#else

#define ___inline
#pragma message(INCLUDE_FILE_AND_LINE("Inline unusable!"))

#endif // __STDC_VERSION__>=199901L

#else
#ifdef __cplusplus

#define ___inline inline
#pragma message(INCLUDE_FILE_AND_LINE("Inline usable!"))

#else

#define ___inline
#pragma message(INCLUDE_FILE_AND_LINE("Inline unusable!"))

#endif // __cplusplus
#endif // __STDC_VERSION__

#define PID_USING_FLOAT
#define K_MEANS_USING_LONG

#endif // WIN32

typedef	signed long long						int64, *pint64, **int64_map;
typedef	unsigned long long						uint64, *puint64, **uint64_map;
typedef	signed long								int32, *pint32, **int32_map;
typedef	unsigned long							uint32, *puint32, **uint32_map;
typedef	signed short							int16, *pint16, **int16_map;
typedef	unsigned short							uint16, *puint16, **uint16_map;
typedef	signed char								int8, *pint8, **int8_map;
typedef	unsigned char							uint8, uchar, byte, *puint8, *puchar8, *pbyte, **byte_map;
typedef void									*pvoid;

#endif // FC_SDE_H