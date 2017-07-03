/**
 * @file WebCore.h
 *
 * @brief The main header for the Awesomium C++ API.
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

#ifndef __AWESOMIUM_WEB_CORE_H__
#define __AWESOMIUM_WEB_CORE_H__

#include <Awesomium/PlatformUtils.h>
#include <Awesomium/WebView.h>

class Profile;

#if defined(__WIN32__) || defined(_WIN32)
#	if defined(_MSC_VER)
#		if (_MSC_VER < 1500 || _MSC_VER >= 1600)
#			error The current C++ API of Awesomium requires Microsoft Visual C++ 2008 (MSVC9) on Windows. Please use our C API instead (awesomium_capi.h).
#		endif
#	else
#		error The current C++ API of Awesomium requires Microsoft Visual C++ 2008 (MSVC9) on Windows. Please use our C API instead (awesomium_capi.h).
#	endif
#endif

namespace Awesomium {
	
	class WebViewImpl;
	class WebCoreImpl;
	class WebViewEvent;
	class HistoryQueryResult;
	class HistoryEntry;
	
	/**
	 * An enumeration of the three verbosity settings for the Awesomium Log.
	 */
	enum LogLevel
	{
		/// No log is created
		LOG_NONE,

		/// Logs only errors
		LOG_NORMAL,

		/// Logs everything
		LOG_VERBOSE		
	};
	
	/**
	* Configuration settings for the WebCore.
	*/
	class _OSMExport WebCoreConfig
	{
	public:
		/**
		 * Creates a default WebCoreConfig with the following default settings:
		 * 
		 * <pre>
		 *     enablePlugins           = false
		 *     enableJavascript        = true
		 *     enableDatabases         = false
		 *     packagePath             = (empty)
		 *     localePath              = (empty)
		 *     userDataPath            = (empty)
		 *     pluginPath              = (empty)
		 *     logPath                 = (empty)
		 *     logLevel                = LOG_NORMAL
		 *     forceSingleProcess      = false
		 *     childProcessPath        = (empty)
		 *     enableAutoDetectEncoding = true
		 *     acceptLanguageOverride  = (empty)
		 *     defaultCharsetOverride  = (empty)
		 *     userAgentOverride       = (empty)
		 *     proxyServer             = "auto"
		 *     proxyConfigScript       = (empty)
		 *     authServerWhitelist     = (empty)
		 *     saveCacheAndCookies     = false
		 *     maxCacheSize            = 0
		 *     disableSameOriginPolicy = false
		 *     disableWinMessagePump   = false
		 *     customCSS               = (empty)
		 * </pre>
		 */
		WebCoreConfig();
		
		/**
		 * Set whether or not to enable embedded plugins (e.g., Flash).
		 */
		void setEnablePlugins(bool enablePlugins);

		/**
		 * Set whether or not Javascript is enabled.
		 */
		void setEnableJavascript(bool enableJavascript);

		/**
		 * Set whether or not local HTML5 databases are enabled.
		 * This is DISABLED by default because it creates a local
		 * "database" folder in your user data path.
		 */
		void setEnableDatabases(bool enableDatabases);

		/**
		 * Set the path that contains the Awesomium SDK files. All other
		 * config paths will inherit from this path. This path is usually
		 * automatically determined at runtime but you can override it
		 * below. This path means different things on each platform:
		 *
		 * <pre>
		 *  - On Windows: the path that contains Awesomium.dll, icudt42.dll,
		 *                the locales folder, and AwesomiumProcess.exe
		 *
		 *  - On MacOSX:  the path that contains Awesomium.framework
		 *
		 *  - On Linux:   the path that contains libAwesomium.so, chrome.pak,
		 *                and the locales folder
		 * </pre>
		 */
		void setPackagePath(const std::wstring& packagePath);

		/**
		 * Set that path that contains the locale data (ex, en-US.dll on
		 * Windows). This is usually "./locales" by default.
		 */
		void setLocalePath(const std::wstring& localePath);
		
		/** 
		 * Set the path to the directory that will be used to store cache, 
		 * cookies, and other data. If an empty string is specified, this path 
		 * defaults to "./Default".
		 */
		void setUserDataPath(const std::wstring& userDataPath);
		
		/** 
		 * Set the path to the directory that will be used to store cache, 
		 * cookies, and other data. If an empty string is specified, this path 
		 * defaults to "./Default".
		 */
		void setUserDataPath(const std::string& userDataPath);
		
		/**
		 * Set an absolute path that will be included in the search for plugins. 
		 * This is useful if you wish to bundle certain plugins with your 
		 * application.
		 */
		void setPluginPath(const std::wstring& pluginPath);
		
		/**
		 * Set an absolute path that will be included in the search for plugins. 
		 * This is useful if you wish to bundle certain plugins with your 
		 * application.
		 */
		void setPluginPath(const std::string& pluginPath);
		
		/**
		 * Set the path to store the awesomium.log. If none is specified, the
		 * log will be stored in the working directory.
		 */
		void setLogPath(const std::wstring& logPath);
		
		/**
		 * Set the path to store the awesomium.log. If none is specified, the
		 * log will be stored in the working directory.
		 */
		void setLogPath(const std::string& logPath);
		
		/** 
		 * Set the logging level to use, this can be either LOG_NONE, 
		 * LOG_NORMAL, or LOG_VERBOSE.
		 */
		void setLogLevel(LogLevel logLevel);

		/**
		 * Sets whether or not all WebViews and should be forced to render inside
		 * the main process (we typically launch a separate child-process to
		 * render each WebView and plugin safely). This mode currently only works
		 * on Windows and automatically disables plugins and local databases.
		 */
		void setForceSingleProcess(bool forceSingleProcess);

		/**
		 * Set the path to the child-process that we will use to render WebViews
		 * and plugins (this will be AwesomiumProcess.exe if you leave this
		 * empty). You can specify your own executable to handle the rendering
		 * but you will need to make sure that it calls ChildProcessMain.
		 * 
		 * <pre>
		 * This config option can be used to bypass AwesomiumProcess completely
		 * so you can use the same executable for everything, for example:
		 *
		 * int main(int argc, char** argv)
		 * {
		 *     if(Awesomium::isChildProcess(GetModuleHandle(0)))
		 *         return Awesomium::ChildProcessMain(GetModuleHandle(0));
		 *
		 *     // Continue normal main method below.
		 *
		 *     Awesomium::WebCoreConfig config;
		 *
		 *     // You can specify "self" as a shortcut to 
		 *     // the path of this executable.
		 *     config.setChildProcessPath(L"self");
		 *
		 *     Awesomium::WebCore webCore(config);
		 *
		 *     // This will spawn a new child instance of your executable
		 *     // which will be handled by ChildProcessMain above.
		 *     WebView* webView = webCore.createWebView(512, 512);
		 *
		 *     // Rest of your application goes here.
		 *
		 *     return 0;
		 * }
		 * </pre>
		 */
		void setChildProcessPath(const std::wstring& childProcessPath);

		/**
		 * Whether or not the page encoding should be automatically
		 * detected. Default is true.
		 */
		void setEnableAutoDetectEncoding(bool autoDetectEncoding);

		/**
		 * Set this to override the accept-language header for all
		 * WebViews. For example: "en-US" for English and "fr-FR" for French
		 */
		void setAcceptLanguageOverride(const std::string& acceptLanguageString);

		/**
		 * Set this to override the default charset for all WebViews.
		 */
		void setDefaultCharsetOverride(const std::string& defaultCharsetString);
		
		/**
		 * Set the user agent string that will be used to override the default. 
		 * Leave this empty to use the default user agent.
		 */
		void setUserAgentOverride(const std::string& userAgentOverride);
		
		/**
		 * Set this to configure the proxy settings for all network requests.
		 * Specify "none" to disable all proxy use, specify "auto" to attempt
		 * to detect the proxy using system settings (e.g., via the 
		 * Internet Properties dialog on Windows or the Network panel of
		 * System Preferences on Mac OSX). Specify anything else to set
		 * manual proxy settings.
		 *
		 * <pre>
		 * For example:
		 * 
		 *    "none"                         -- No proxy.
		 *
		 *    "auto"                         -- Detect system proxy settings.
		 *                                      (Default).
		 *
		 *    "http=myproxy:80;ftp=myproxy2" -- Use HTTP proxy "myproxy:80"  
		 *                                      for http:// URLs, and HTTP proxy 
		 *                                      "myproxy2:80" for ftp:// URLs.
		 *
		 *    "myproxy:80"                   -- Use HTTP proxy "foopy:80" for
		 *                                      all URLs.
		 *
		 *    "socks4://myproxy"             -- Use SOCKS v4 proxy "foopy:1080" 
		 *                                      for all URLs.
		 * </pre>
		 */
		void setProxyServer(const std::string& proxyServer);
		
		/**
		 * Set the URL to the PAC (Proxy Auto-Config) Script to use.
		 * See <http://en.wikipedia.org/wiki/Proxy_auto-config> for more info.
		 */
		void setProxyConfigScript(const std::string& proxyConfigScript);

		/**
		 * Set the list of servers that Awesomium is allowed to silently 
		 * provide user credentials for, when challenged. We can authenticate
		 * the user to an Intranet server or proxy without prompting the user
		 * for a username or password by using cached credentials which are 
		 * established when the user initially logs in to the machine that 
		 * Awesomium is running on. This is supported for Negotiate and NTLM
		 * challenges only.
		 *
		 * <pre>
		 * The list is set using a comma-separated string of URLs. For example, 
		 * you can specify:
         *
		 *   config.setAuthServerWhitelist("*example.com,*foobar.com,*baz");
		 *
         * </pre>
		 */
		void setAuthServerWhitelist(const std::string& authServerWhitelist);
		
		/**
		 * Set whether or not cache and cookies should be saved to disk.
		 * This is useful for pers
		 */
		void setSaveCacheAndCookies(bool saveCacheAndCookies);
		
		/**
		 * Set the maximum disk space to be used by the disk cache, in bytes.
		 * Specify 0 to use no limit.
		 */
		void setMaxCacheSize(int maxCacheSize);
		
		/**
		 * Set whether or not the "same-origin" policy should be disabled.
		 * See <http://en.wikipedia.org/wiki/Same_origin_policy>
		 */
		void setDisableSameOriginPolicy(bool disableSameOriginPolicy);

		/**
		 * Set this to disable the Windows Message Pump (Peek/DispatchMsg).
		 * WebCore::update automatically pumps messages by default on the
		 * Windows platform but if your application maintains its own
		 * WndProc or Message Pump, you may wish to disable this.
		 */
		void setDisableWinMessagePump(bool disableWinMessagePump);
		
		/**
		 * Set a string of custom CSS to be included as the global default
		 * style for all pages. This is especially useful for customizing
		 * scrollbars and other look-and-feel elements.
		 */
		void setCustomCSS(const std::string& customCSS);

		/**
		 * Set a custom CSS file to be included as the global default style for 
		 * all pages. This is especially useful for customizing scrollbars and 
		 * other look-and-feel elements.
		 */
		void setCustomCSSFromFile(const std::string& customCSSFilePath);
	protected:

		bool enablePlugins;
		bool enableJavascript;
		bool enableDatabases;
		std::wstring packagePath;
		std::wstring localePath;
		std::wstring userDataPath;
		std::wstring pluginPath;
		std::wstring logPath;
		LogLevel logLevel;
		bool forceSingleProcess;
		std::wstring childProcessPath;
		bool enableAutoDetectEncoding;
		std::string acceptLanguageOverride;
		std::string defaultCharsetOverride;
		std::string userAgentOverride;
		std::string proxyServer;
		std::string proxyConfigScript;
		std::string authServerWhitelist;
		bool saveCacheAndCookies;
		int maxCacheSize;
		bool disableSameOriginPolicy;
		bool disableWinMessagePump;
		std::string customCSS;

		friend class ConfigPathResolver;
		friend class WebCoreImpl;
		friend class WebCore;
	};
	
	/**
	 * The WebCore is the "core" of Awesomium-- it manages the lifetime of 
	 * all WebViews (see Awesomium::WebView) and maintains useful services 
	 * like resource caching and network connections.
	 * 
	 * Generally, you should create an instance of the WebCore at the 
	 * beginning of your program and then destroy the instance at the end 
	 * of your program.
	 */
	class _OSMExport WebCore
	{
	public:
		/**
		 * Instantiates the WebCore singleton (you can access it later with
		 * WebCore::Get or GetPointer).
		 *
		 * @param	config	Various configuration settings. See WebCoreConfig 
		 *                  for more information.
		 */
		WebCore(const WebCoreConfig& config = WebCoreConfig());
		
		/**
		 * Destroys the WebCore singleton. (Also destroys any current WebViews)
		 */
		~WebCore();
		
		/**
		 * Retrieves the WebCore singleton.
		 *
		 * @note	This will assert if the singleton is not instantiated.
		 *
		 * @return	Returns a reference to the instance.
		 */
		static WebCore& Get();
		
		/**
		 * Retrieves the WebCore singleton.
		 *
		 * @return	Returns a pointer to the instance.
		 */
		static WebCore* GetPointer();
		
		/**
		 * Sets the base directory.
		 *
		 * @param	baseDirectory	The absolute path to your base directory. 
		 *                          The base directory is a location that holds
		 *                          all of your local assets. It will be used 
		 *                          for WebView::loadFile and WebView::loadHTML
		 *                          (to resolve relative URLs).
		 */
		void setBaseDirectory(const std::string& baseDirectory);
		
		/**
		 * Sets the base directory.
		 *
		 * @param	baseDirectory	The absolute path to your base directory. 
		 *                          The base directory is a location that holds
		 *                          all of your local assets. It will be used 
		 *                          for WebView::loadFile and WebView::loadHTML
		 *                          (to resolve relative URLs).
		 */
		void setBaseDirectory(const std::wstring& baseDirectory);
		
		/**
		 * Creates a new WebView.
		 *
		 * @param	width	The width of the WebView in pixels.
		 * @param	height	The height of the WebView in pixels.
		 * @param	viewSource	Enable this to view the HTML source of any web-page
		 *                      loaded into this WebView. Default is false.
		 *
		 * @return	Returns a pointer to the created WebView.
		 */
		WebView* createWebView(int width, 
							   int height,
							   bool viewSource = false);
		
		/**
		 * Sets a custom response page to use when a WebView encounters a 
		 * certain HTML status code from the server (like '404 - File not found').
		 *
		 * @param	statusCode	The status code this response page should be 
		 *                      associated with.
		 *						See <http://en.wikipedia.org/wiki/List_of_HTTP_status_codes>
		 *
		 * @param	filePath	The local page to load as a response, should be
		 *                      a path relative to the base directory.
		 */
		void setCustomResponsePage(int statusCode, 
								   const std::wstring& filePath);
		
		/**
		 * Updates the WebCore and allows it to conduct various operations such
		 * as updating the render buffer of each WebView, destroying any
		 * WebViews that are queued for destruction, and invoking any queued
		 * WebViewListener events.
		 */
		void update();
		
		/**
		 * Retrieves the base directory.
		 *
		 * @return	Returns the current base directory.
		 */
		const std::wstring& getBaseDirectory() const;
		
		/**
		 * Returns whether or not plugins are enabled.
		 */
		bool arePluginsEnabled() const;
		
		/**
		 * Clear the disk cache and media cache.
		 */
		void clearCache();
		
		/**
		 * Clear all cookies.
		 */
		void clearCookies();
		
		/**
		 * Sets a cookie for a certain URL.
		 * 
		 * @param	url	The URL to set the cookie on.
		 *
		 * @param	cookieString	The cookie string, for example:
		 *                          <pre> "key1=value1; key2=value2" </pre>
		 *
		 * @param	isHTTPOnly	Whether or not this cookie is HTTP-only.
		 *
		 * @param	forceSessionCookie	Whether or not to force this as a 
		 *	                            session cookie.
		 *
		 * @return	Returns true if the cookie was successfully set, false
		 *          otherwise.
		 */
		void setCookie(const std::string& url, 
					   const std::string& cookieString, 
					   bool isHTTPOnly = false, 
					   bool forceSessionCookie = false);

		/**
		 * Gets all cookies for a certain URL.
		 *
		 * @param	url	The URL whose cookies will be retrieved.
		 *
		 * @param	excludeHTTPOnly	Whether or not to exclude HTTP-only
		 *                          cookies from the result.
		 *
		 * @return	Returns the cookie string.
		 */
		const std::string& getCookies(const std::string& url, 
									  bool excludeHTTPOnly = true);

		/**
		 * Deletes a certain cookie on a certain URL.
		 * 
		 * @param	url	The URL that we will be deleting cookies on.
		 *
		 * @param	cookieName	The name of the cookie that will be deleted.
		 */
		void deleteCookie(const std::string& url, 
						  const std::string& cookieName);

		/**
		 * Set whether or not the printer dialog should be suppressed or not.
		 * Set this to "true" to hide printer dialogs and print immediately
		 * using the OS's default printer when WebView::print is called.
		 * Default is "false" if you never call this.
		 *
		 * @param	suppress	Whether or not the printer dialog should be 
		 *                      suppressed.
		 */
		void setSuppressPrinterDialog(bool suppress);
		
		/**
		 * Query the on-disk history database.
		 *
		 * @param	fullTextQuery	All results returned should match the
		 *                          specified text (either in the page title or
		 *                          in the actual text of the page itself).
		 *                          Specify an empty string to match anything.
		 *
		 * @param	numDaysAgo	Limit results to a specified number of days ago.
		 *
		 * @param	maxCount	Limit results to a maximum count. Specify 0 to
		 *                      use no limit.
		 *
		 * @note	You must enable "SaveCacheAndCookies" (see WebCoreConfig) for
		 *          this method to work (otherwise no results will be returned).
		 *
		 * @return	Returns an instance of HistoryQueryResult containing the results
		 *          of the query. You must call "HistoryQueryResult::destroy" once
		 *          you are finished using the instance.
		 */
		HistoryQueryResult* queryHistory(const std::wstring& fullTextQuery, 
												 int numDaysAgo, int maxCount);

	protected:
		
		Profile* getProfile();
		bool resolveFutureJSValue(WebView* view, int requestID, JSValue& value, 
								  int timeoutMs);
		void enqueueWebViewEvent(WebViewEvent* webViewEvent);
		const std::string& getCustomCSS();
		
		static WebCore* instance;
		WebCoreImpl* webCoreImpl;
		std::wstring baseDirectory;
		bool pluginsEnabled;
		std::string customCSS;
		std::string getCookiesCache;
		int cookieResultIdCounter;
		
		friend class WebViewImpl;
		friend class FutureJSValue;
	};

#if defined(__WIN32__) || defined(_WIN32)
	/**
	 * Returns whether or not this process is an Awesomium Child Process.
	 * @see WebCoreConfig::setChildProcessPath
	 */
	bool _OSMExport isChildProcess(HINSTANCE hInstance);

	/**
	 * Invokes the main method for an Awesomium Child Process.
	 * @see WebCoreConfig::setChildProcessPath
	 */
	int _OSMExport ChildProcessMain(HINSTANCE hInstance);
#else
	/**
	 * Returns whether or not this process is an Awesomium Child Process.
	 * @see WebCoreConfig::setChildProcessPath
	 */
	bool _OSMExport isChildProcess(int argc, char **argv);

	/**
	 * Invokes the main method for an Awesomium Child Process.
	 * @see WebCoreConfig::setChildProcessPath
	 */
	int _OSMExport ChildProcessMain(int argc, char **argv);
#endif
	
	/**
	 * A countainer class that encapsulates an array of history entries.
	 * See WebCore::queryHistory
	 */
	class HistoryQueryResult
	{
	public:
		/// You must destroy this instance after you are done using it.
		virtual void destroy() = 0;
		
		/// Get the number of entries
		virtual size_t size() = 0;
		
		/// Get a certain history entry, you must call HistoryEntry::destroy
		/// on the returned instance once you're done using it.
		/// Will return null if the entry does not exist at the index.
		virtual HistoryEntry* getEntryAtIndex(size_t idx) = 0;
	protected:
		virtual ~HistoryQueryResult() {}
	};
	
	/**
	 * Represents a unique page URL in the history database.
	 */
	class HistoryEntry
	{
	public:
		/// You must destroy this instance after you are done using it.
		virtual void destroy() = 0;
		
		/// Get the associated URL of the page.
		virtual const std::string& url() = 0;
		
		/// Get the associated title of the page (if any).
		virtual const std::wstring& title() = 0;
		
		/// Get the time the last visit occurred. (in seconds since epoch)
		virtual double visitTime() = 0;
		
		/// Get the number of times this page has been visited.
		virtual int visitCount() = 0;
	protected:
		virtual ~HistoryEntry() {}
	};
}

/**
 * @mainpage Awesomium C++ API
 *
 * @section intro_sec Introduction
 *
 * Hi there, welcome to the Awesomium C++ API docs! Awesomium is a software 
 * library that makes it easy to put the web in your applications. Whether 
 * that means embedded web browsing, rendering pages as images, streaming 
 * pages over the net, or manipulating web content live for some other 
 * purpose, Awesomium does it all.
 *
 * If this is your first time exploring the API, we recommend
 * starting with Awesomium::WebCore and Awesomium::WebView.
 *
 * For more help and tips with the API, please visit our Knowledge Base
 *     <http://support.awesomium.com/faqs>
 *
 * @section usefullinks_sec Useful Links
 * - Awesomium Main: <http://www.awesomium.com>
 * - Support Home: <http://support.awesomium.com>
 * 
 * @section copyright_sec Copyright
 * This documentation is copyright (C) 2011 Khrona. All rights reserved. 
 * Awesomium is a trademark of Khrona.
 */

#endif