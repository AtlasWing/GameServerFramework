#include <openssl/hmac.h>

#include "base/nEncrypt.h"
#include "base/nMisc.h"
#include "base/Tidy.h"
#include "base/nLogger.h"

namespace Tidy
{
	nEncrypt::nEncrypt():
		_method(eEncMethod_None),
		_des_enc_data(NULL)
	{
	}

	nEncrypt::~nEncrypt()
	{
		SAFE_DELETE(_des_enc_data);
	}

	void nEncrypt::setDesKeyString(const char *keyStr)
	{
		assert_debug(_method == eEncMethod_DES || _method == eEncMethod_Private, "[%u]", _method);
		DES_cblock key;
		DES_string_to_key(keyStr, &key);
		setDesKey(&key);
	}

	void nEncrypt::setDesKey(const_DES_cblock *key)
	{
		assert_debug(_method == eEncMethod_DES || _method == eEncMethod_Private, "[%u]", _method);
		::DES_set_key(key, &_des_enc_data->keySchedule);
	}

	eEncdecResult nEncrypt::encdecDes(unsigned char *data, DWORD len, bool enc)
	{
		if(len < 8 || (len % 8) != 0){
			assert_fail("数据不足");
			return eEncdecResult_DataTooShort;
		}
		DWORD offset = 0; 
		while((offset+8) <= len) {       
			DES_encrypt1((DES_LONG*)(data+offset), &_des_enc_data->keySchedule, enc);
			offset += 8;
		}

		return eEncdecResult_OK;
	}

	eEncdecResult nEncrypt::encdecPrivate(unsigned char *data, DWORD len, bool enc, std::string *out)
	{
		if(out == NULL){
			return eEncdecResult_NeedOutParam;
		}
		size_t oldLen = len;
		eEncdecResult result = eEncdecResult_OK;
		do{
			if(enc == true){
				//8 字节对齐
				len = (len/8 + 1) * 8;
				unsigned char *expandData = (unsigned char*)malloc(len);
				memset(expandData, 0, len);
				memcpy(expandData, data, oldLen);
				result = encdecDes(expandData, len, true);
				if(result == eEncdecResult_OK){
					*out = nEncrypt::encodeBase64((char*)expandData, len, true);
				}
				free(expandData);
			}
			else{
				*out = nEncrypt::encodeBase64((char*)data, len, false);
				assert_debug((out->size() % 8) == 0, "[%u]", out->size());
				result = encdecDes((unsigned char*)out->c_str(), out->size(), false);
			}
		}
		while(0);
		return result;
	}

	void nEncrypt::setEncMethod(eEncMethod m)
	{
		assert_debug(_method == eEncMethod_None, "[%u]", _method);
		_method = m;
		if(_method == eEncMethod_DES){
			_des_enc_data = new nDesEncData();
		}
		else if(_method == eEncMethod_Private){
			_des_enc_data = new nDesEncData();
			// 可以考虑取本机的mac地址
			setDesKeyString("sUI8+kjx5iE~ld0Og80sn1edQQ~");
		}
	}

	eEncMethod nEncrypt::getEncMethod() const
	{
		return static_cast<eEncMethod>(_method);
	}

	eEncdecResult nEncrypt::encdec(void *data, DWORD len, bool enc, std::string *out)
	{
		int needEnc = (enc > 0) ? 1 : 0;
		unsigned char *rawData = reinterpret_cast<unsigned char *>(data);
		switch(_method){
			case eEncMethod_DES:
				{
					return encdecDes(rawData, len, needEnc);
				}
				break;
			case eEncMethod_Private:
				{
					return encdecPrivate(rawData, len, needEnc, out);
				}
				break;
		}
		assert_fail("不支持%u", _method);
		return eEncdecResult_NotSupport;
	}

	bool nEncrypt::verifySign_MD5(const char *data, const DWORD dataSize, const char *sign, const DWORD signSize)
	{
		unsigned char digest[MD5_DIGEST_LENGTH];
		unsigned char digestStr[MD5_DIGEST_LENGTH*2];
		if(signSize != (sizeof(digestStr))){
			return false;
		}
		MD5_CTX md5Ctx;
		MD5_Init(&md5Ctx);
		MD5_Update(&md5Ctx, reinterpret_cast<const unsigned char*>(data), dataSize);
		MD5_Final(digest, &md5Ctx);
		//MD5(reinterpret_cast<const unsigned char*>(data), dataSize, digest);
		for(DWORD i = 0; i < sizeof(digest); ++ i){
			snprintf((char*)&digestStr[2 * i], 3, "%02x", (DWORD)digest[i]);
		}
		if(memcmp(digestStr, sign, sizeof(digestStr)) != 0){
			return false;
		}

		return true;
	}

	std::string nEncrypt::calculateMD5(const char *data, const DWORD dataSize)
	{
		unsigned char digest[MD5_DIGEST_LENGTH];
		unsigned char digestStr[MD5_DIGEST_LENGTH*2];
		MD5_CTX md5Ctx;
		MD5_Init(&md5Ctx);
		MD5_Update(&md5Ctx, reinterpret_cast<const unsigned char*>(data), dataSize);
		MD5_Final(digest, &md5Ctx);
		DWORD len = 0;
		//MD5(reinterpret_cast<const unsigned char*>(data), dataSize, digest);
		for(DWORD i = 0; i < sizeof(digest); ++ i){
			len += snprintf((char*)&digestStr[2 * i], 3, "%02x", (DWORD)digest[i]);
		}
		return std::string(reinterpret_cast<char*>(digestStr), len);
	}

	std::string nEncrypt::encodeBase64(const char *data, const DWORD dataSize, bool enc)
	{
		// 转换到base64后的大小
		DWORD encodeArraySize = (dataSize -1)/3*4 +4;
		unsigned char encodeStr[encodeArraySize];
		int encodeStrSize = 0;
		if(enc == true){
			encodeStrSize = EVP_EncodeBlock(encodeStr, reinterpret_cast<const unsigned char*>(data), dataSize);
		}
		else{
			encodeStrSize = EVP_DecodeBlock(encodeStr, reinterpret_cast<const unsigned char*>(data), dataSize);
			DWORD i = dataSize;
			while(data[--i] == '='){
				-- encodeStrSize;
			}
		}
		return std::string(reinterpret_cast<char*>(encodeStr), encodeStrSize);
	}

	bool nEncrypt::calculateSHA1(const char *key, const DWORD keyLen, const char *data, const DWORD dataSize, std::string *digest)
	{
		char digestBuffer[20];
		DWORD digestLen = 0;
		if(HMAC(EVP_sha1(), reinterpret_cast<const unsigned char*>(key), keyLen, reinterpret_cast<const unsigned char*>(data), dataSize, reinterpret_cast<unsigned char*>(digestBuffer), &digestLen) == NULL){
			assert_fail("hmac失败");
			return false;
		}
		digest->assign(digestBuffer, digestLen);
		return true;
	}
};
