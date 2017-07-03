/**
 * @file JSValue.h
 *
 * @author
 *
 * This file is a part of Awesomium, a library that makes it easy for 
 * developers to embed the web in their applications.
 *
 * Website: <http://www.awesomium.com>
 * 
 * Copyright (C) 2011 Khrona. All rights reserved. Awesomium is a 
 * trademark of Khrona.
 */

#ifndef __JSVALUE_H__
#define __JSVALUE_H__

#include <string>
#include <vector>
#include <map>
#include <Awesomium/PlatformUtils.h>

namespace Impl { struct VariantValue; }

namespace Awesomium 
{
	class WebView;
	class WebViewImpl;
	
	/**
	 * JSValue is a class that represents a Javascript value. It can be 
	 * initialized from and converted to several types: boolean, integer, 
	 * double, and string
	 */
	class _OSMExport JSValue
	{
		Impl::VariantValue* varValue;
	public:
		
		typedef std::map<std::wstring, JSValue> Object;
		typedef std::vector<JSValue> Array;
		
		/// Creates a null JSValue.
		JSValue();
		
		/// Creates a JSValue initialized with a boolean.
		JSValue(bool value);
		
		/// Creates a JSValue initialized with an integer.
		JSValue(int value);
		
		/// Creates a JSValue initialized with a double.
		JSValue(double value);
		
		/// Creates a JSValue initialized with an ASCII string.
		JSValue(const char* value);
		
		/// Creates a JSValue initialized with an ASCII string.
		JSValue(const std::string& value);
		
		/// Creates a JSValue initialized with a wide string.
		JSValue(const wchar_t* value);
		
		/// Creates a JSValue initialized with a wide string.
		JSValue(const std::wstring& value);
		
		/// Creates a JSValue initialized with an object.
		JSValue(const Object& value);
		
		/// Creates a JSValue initialized with an array.
		JSValue(const Array& value);
		
		JSValue(const JSValue& original);
		
		~JSValue();
		
		JSValue& operator=(const JSValue& rhs);
		
		/// Returns whether or not this JSValue is a boolean.
		bool isBoolean() const;
		
		/// Returns whether or not this JSValue is an integer.
		bool isInteger() const;
		
		/// Returns whether or not this JSValue is a double.
		bool isDouble() const;
		
		/// Returns whether or not this JSValue is a number (integer or double).
		bool isNumber() const;
		
		/// Returns whether or not this JSValue is a string.
		bool isString() const;
		
		/// Returns whether or not this JSValue is an array.
		bool isArray() const;
		
		/// Returns whether or not this JSValue is an object.
		bool isObject() const;
		
		/// Returns whether or not this JSValue is null.
		bool isNull() const;
		
		/// Returns this JSValue as a wide string (converting if necessary).
		const std::wstring& toString() const;
		
		/// Returns this JSValue as an integer (converting if necessary).
		int toInteger() const;
		
		/// Returns this JSValue as a double (converting if necessary).
		double toDouble() const;
		
		/// Returns this JSValue as a boolean (converting if necessary).
		bool toBoolean() const;
		
		/// Gets a reference to this JSValue's array value (will assert if not 
		/// an array type)
		Array& getArray();
		
		/// Gets a constant reference to this JSValue's array value (will assert 
		/// if not an array type)
		const Array& getArray() const;
		
		/// Gets a reference to this JSValue's object value (will assert if not 
		/// an object type)
		Object& getObject();
		
		/// Gets a constant reference to this JSValue's object value (will 
		/// assert if not an object type)
		const Object& getObject() const;
	};
	
	typedef std::vector<JSValue> JSArguments;
	
	/**
	 * FutureJSValue is a special wrapper around a JSValue that allows
	 * asynchronous retrieval of the actual value at a later time.
	 *
	 * If you are unfamiliar with the concept of a 'Future', please see:
	 * <http://en.wikipedia.org/wiki/Futures_and_promises>
	 */
	class _OSMExport FutureJSValue
	{
	public:
		FutureJSValue();
		~FutureJSValue();
		
		/**
		 * Retrieve the value now. If the value has not been computed,
		 * this will block the calling thread until it has.
		 */
		const JSValue& get();
		
		/**
		 * Retrieve the value now. If the value has not been computed,
		 * this will block the calling thread until it has or until the
		 * timeout is reached.
		 *
		 * @param	timeoutMs	The maximum time to wait, in milliseconds.
		 */
		const JSValue& getWithTimeout(int timeoutMs);
		
	protected:
		void init(WebView* source, int requestID);
		
		JSValue value;
		WebView* source;
		int requestID;
		
		friend class WebViewImpl;
	};
	
	std::string _OSMExport serializeJSValue(const JSValue& value);
	
	JSValue _OSMExport deserializeJSValue(const std::string& buffer);
	
}

#endif