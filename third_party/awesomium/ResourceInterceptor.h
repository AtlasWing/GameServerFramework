/**
 * @file ResourceInterceptor.h
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

#ifndef __AWESOMIUM_RESOURCE_INTERCEPTOR_H__
#define __AWESOMIUM_RESOURCE_INTERCEPTOR_H__

#include <string>
#include <sstream>
#include <Awesomium/PlatformUtils.h>

#if __LP64__
typedef long		int64;
#else
typedef long long	int64;
#endif

namespace Awesomium {
	
class WebView;
class ResourceResponse;
struct ResourceResponseMetrics;
class UploadElement;
class ResourceRequest;

/**
 * The ResourceInterceptor class is used to intercept requests and responses 
 * for resources via WebView::setResourceInterceptor. This can be used for
 * responding to resource requests using your own custom resource loading
 * scheme or for tracking resource loads.
 */
class _OSMExport ResourceInterceptor
{
public:
	/**
	 * Override this method to intercept requests for resources. You can use 
	 * this to modify requests before they are sent, respond to requests using 
	 * your own custom resource-loading back-end, or to monitor requests for 
	 * tracking purposes.
	 *
	 * @param	caller	The WebView instance that invoked the request.
	 *
	 * @param	request		The resource request.
	 *
	 * @return	Return a new ResourceResponse (see ResourceResponse::Create)
	 *          to override the response, otherwise, return NULL to allow 
	 *          normal behavior.
	 */
	virtual Awesomium::ResourceResponse* onRequest(Awesomium::WebView* caller, 
									   Awesomium::ResourceRequest* request) {
		return NULL;
	}
	
	/**
	 * Override this method to intercept responses to requests. You can use
	 * this for tracking/statistics purposes.
	 *
	 * @param	caller		The WebView instance that invoked the response.
	 *
	 * @param	url			The URL of the response.
	 *
	 * @param	statusCode	The HTTP status code of the response. For non-HTTP
	 *						responses, 200 represents "success" and 404
	 *						represents "failure".
	 *
	 * @param	metrics		Various informative metrics about the response.
	 */
	virtual void onResponse(Awesomium::WebView* caller, 
							const std::string& url, int statusCode, 
							const Awesomium::ResourceResponseMetrics& metrics) {}
};
	
/**
 * The ResourceRequest class represents a request for a URL resource. You can
 * get information about the request or modify it (change GET to POST, modify
 * headers, etc.).
 */
class _OSMExport ResourceRequest
{
public:

	/// Cancel this request.
	virtual void cancel() = 0;

	/// Get the URL associated with this request.
	virtual const std::string& getURL() = 0;
	
	/// Get the HTTP method (usually "GET" or "POST")
	virtual const std::string& getMethod() = 0;
	
	/// Set the HTTP method
	virtual void setMethod(const std::string& method) = 0;
	
	/// Get the referrer
	virtual const std::string& getReferrer() = 0;
	
	/// Set the referrer
	virtual void setReferrer(const std::string& referrer) = 0;
	
	/// Get extra headers for the request
	virtual const std::string& getExtraHeaders() = 0;
	
	/**
	 * Override extra headers for the request, delimited by /r/n (CRLF).
	 *
	 * Format should be:
	 *   Name: Value/r/nName: Value/r/nName: Value
	 *
	 * Headers should NOT end in /r/n (CRLF)	 
	 */
	virtual void setExtraHeaders(const std::string& headers) = 0;
	
	/**
	 * Append an extra header to the request.
	 *
	 * @param	name	Name of the header
	 * @param	value	Value of the header
	 */
	virtual void appendExtraHeader(const std::string& name,
								   const std::string& value) = 0;
	
	/// Get the number of upload elements (essentially, batches of POST data).
	virtual size_t getNumUploadElements() = 0;
	
	/// Get a certain upload element (returned instance is owned by this class)
	virtual const UploadElement* getUploadElement(size_t idx) = 0;
	
	/// Clear all upload elements
	virtual void clearUploadElements() = 0;
	
	/// Append a file for POST data (adds a new UploadElement)
	virtual void appendUploadFilePath(const std::wstring& filePath) = 0;
	
	/// Append a string of bytes for POST data (adds a new UploadElement)
	virtual void appendUploadBytes(const std::string& bytes) = 0;
};
	
/**
 * The UploadElement class represents a batch of "upload" data sent along
 * with the ResourceRequest. This data is usually sent with a POST request.
 */
class _OSMExport UploadElement
{
public:
	/// Whether or not this UploadElement is a file
	virtual bool isFilePath() const = 0;
	
	/// Whether or not this UploadElement is a string of bytes
	virtual bool isBytes() const = 0;
	
	/// Get the string of bytes associated with this UploadElement
	virtual const std::string& getBytes() const = 0;
	
	/// Get the file path associated with this UploadElement
	virtual const std::wstring& getFilePath() const = 0;
};

/**
 * The ResourceResponse class is simply a wrapper around a raw block of data and 
 * a specified mime-type. It can be used with ResourceInterceptor::onRequest 
 * to return a custom resource for a certain resource request.
 */
class _OSMExport ResourceResponse
{
public:
	/**
	 * Create a ResourceResponse from a raw block of data. (Data is not owned,
	 * a copy is made of the supplied buffer.)
	 * 
	 * @param	numBytes	Size (in bytes) of the memory buffer.
	 *
	 * @param	buffer		Raw memory buffer to be copied.
	 *
	 * @param	mimeType	The mime-type of the data.
	 *                      See <http://en.wikipedia.org/wiki/Internet_media_type>
	 */
	static ResourceResponse* Create(size_t numBytes, unsigned char* buffer, 
									const std::string& mimeType);

	/**
	 * Create a ResourceResponse from a file on disk.
	 *
	 * @param	filePath	The path to the file.
	 */
	static ResourceResponse* Create(const std::wstring& filePath);

	ResourceResponse(size_t numBytes, unsigned char* buffer, 
					 const std::string& mimeType);
	ResourceResponse(const std::wstring& filePath);

	~ResourceResponse();
	
	size_t numBytes;
	unsigned char* buffer;
    std::string mimeType;
	std::wstring filePath;

	friend class WebCoreImpl;
};
	
/**
 * Various informative statistics about a certain resource response.
 */
struct _OSMExport ResourceResponseMetrics
{
	/// Whether or not the resource was cached
	bool wasCached;
	
	/// The time of the request, in milliseconds
	int64 requestTimeMs;
	
	/// The time that the response began, in milliseconds
	int64 responseTimeMs;
	
	/// The expected content size given by the server
	int64 expectedContentSize;
	
	/// The mime-type of the response
	std::string mimeType;
};

}

#endif