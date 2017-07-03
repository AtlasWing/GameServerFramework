#ifndef BASE_NENCRYPT_H
#define BASE_NENCRYPT_H

#include "base/nSys.h"

namespace Tidy
{
	struct nDesEncData{
		/// @brief	des密钥
		DES_key_schedule keySchedule;
	};
	enum eEncMethod{
		eEncMethod_None =   0,
		eEncMethod_DES,
		eEncMethod_Private,
	};
	enum eEncdecResult{
		eEncdecResult_OK    =   0,
		eEncdecResult_NotSupport ,
		eEncdecResult_DataTooShort,
		eEncdecResult_NeedOutParam,
	};
	class nEncrypt
	{
		public:
			nEncrypt();
			~nEncrypt();

		public:
			static bool verifySign_MD5(const char *data, const DWORD dataSize, const char *sign, const DWORD signSize);
			static std::string calculateMD5(const char *data, const DWORD dataSize);
			static std::string encodeBase64(const char *data, const DWORD dataSize, bool enc);
			static bool calculateSHA1(const char *key, const DWORD keyLen, const char *data, const DWORD dataSize, std::string *digest);

		public:
			void setEncMethod(eEncMethod method);
			eEncMethod getEncMethod() const;
			eEncdecResult encdec(void *data, DWORD len, bool enc, std::string *out);
			void setDesKeyString(const char *keyStr);
			void setDesKey(const_DES_cblock *key);

		private:
			eEncdecResult encdecDes(unsigned char *data, DWORD len, bool enc);
			eEncdecResult encdecPrivate(unsigned char *data, DWORD len, bool enc, std::string *out);
			int encdecPrivate(unsigned char *data, DWORD len, bool enc);

		private:
			BYTE _method;
			nDesEncData *_des_enc_data;
	};
};

#endif
