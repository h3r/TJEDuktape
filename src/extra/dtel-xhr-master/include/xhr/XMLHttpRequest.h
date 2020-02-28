#pragma once

#include <dtel.h>
#include <dtel/lib/eventtarget/EventTarget.h>

#include <duktape.h>

#include <Poco/URI.h>
#include <Poco/Net/HTTPRequest.h>
#include <Poco/Net/HTTPResponse.h>
#include <Poco/Net/HTTPClientSession.h>
#include <Poco/StreamCopier.h>

#include <thread>
#include <sstream>

using namespace dtel;

namespace xhr {

namespace detail {

	static const char* PROP_XHRDATA = "\xFF" "DTEL_XHR_XHRDATA";
	static const char* PROP_EVENTLOOP = "\xFF" "DTEL_XHR_EVENTLOOP";
	static const char* PROP_REFS = "\xFF" "DTEL_XHR_REFS";

	class ReadyStateChangeEvent : public Event
	{
	public:
		enum {
			UNSENT = 0,
			OPENED = 1,
			HEADERS_RECEIVED = 2,
			LOADING = 3,
			DONE = 4
		} readystate_t;

		ReadyStateChangeEvent(Ref::Ptr ref, int readystate) :
			Event(), _ref(ref), _readystate(readystate)
		{}

		void apply(duk_context *ctx)
		{
			// get ref of XMLHttpRequest object
			_ref->push(ctx);

			// set the data on the object
			duk_push_int(ctx, _readystate);
			duk_put_prop_string(ctx, -2, "readyState");

			// pop ref
			duk_pop(ctx);

			// call dispatch on the "readystatechange" event
			//eventtarget::Event::Ptr evt("readystatechange", "Event");
			auto evt(make_intrusive<eventtarget::Event>("readystatechange", "Event"));
			eventtarget::EventTarget_dispatchEvent(ctx, _ref, evt);
		}

		void release(duk_context *ctx)
		{

		}

	private:
		Ref::Ptr _ref;
		int _readystate;
	};

	class LoadStartEvent : public Event
	{
	public:
		LoadStartEvent(Ref::Ptr ref) :
			Event(), _ref(ref)
		{}

		void apply(duk_context *ctx)
		{
			// call dispatch on the "loadstart" event
			auto evt = make_intrusive<eventtarget::Event>("loadstart", "Event");
			eventtarget::EventTarget_dispatchEvent(ctx, _ref, evt);
		}

		void release(duk_context *ctx)
		{

		}

	private:
		Ref::Ptr _ref;
	};

	class LoadEvent : public Event
	{
	public:
		LoadEvent(Ref::Ptr ref, const std::string &data, int status, const std::string &statusText) : 
			Event(), _ref(ref), _data(data), _status(status), _statustext(statusText)
		{}

		void apply(duk_context *ctx)
		{
			// get ref of XMLHttpRequest object
			_ref->push(ctx);

			// set the data on the object
			duk_push_null(ctx);
			duk_put_prop_string(ctx, -2, "response");

			duk_push_lstring(ctx, _data.c_str(), _data.length());
			duk_put_prop_string(ctx, -2, "responseText");

			duk_push_string(ctx, "text");
			duk_put_prop_string(ctx, -2, "responseType");

			duk_push_int(ctx, _status);
			duk_put_prop_string(ctx, -2, "status");

			duk_push_lstring(ctx, _statustext.c_str(), _statustext.length());
			duk_put_prop_string(ctx, -2, "statusText");

			// pop ref
			duk_pop(ctx);

			// call dispatch on the "load" event
			auto evt = make_intrusive<eventtarget::Event>("load", "Event", _ref);
			eventtarget::EventTarget_dispatchEvent(ctx, _ref, evt);

			// call dispatch on the "loadend" event
			auto evtend = make_intrusive<eventtarget::Event>("loadend", "Event", _ref);
			eventtarget::EventTarget_dispatchEvent(ctx, _ref, evtend);
		}

		void release(duk_context *ctx)
		{

		}

	private:
		Ref::Ptr _ref;
		std::string _data;
		int _status;
		std::string _statustext;
	};

	class ErrorEvent : public Event
	{
	public:
		ErrorEvent(Ref::Ptr ref, const std::string &message) :
			Event(), _ref(ref), _message(message)
		{}

		void apply(duk_context *ctx)
		{
			// call 'dispatchEvent' on the "error" event
			auto evt = make_intrusive<eventtarget::Event>("error", "ErrorEvent", _ref);
			evt->eventInit.properties["message"] = _message;
			eventtarget::EventTarget_dispatchEvent(ctx, _ref, evt);

			// call dispatch on the "loadend" event
			auto evtend = make_intrusive<eventtarget::Event>("loadend", "Event", _ref);
			eventtarget::EventTarget_dispatchEvent(ctx, _ref, evtend);
		}

		void release(duk_context *ctx)
		{

		}

	private:
		Ref::Ptr _ref;
		std::string _message;
	};

	inline EventLoop *eventloop_from_xhr(duk_context *ctx)
	{
		duk_push_global_object(ctx);
		duk_get_prop_string(ctx, -1, "XMLHttpRequest");
		duk_get_prop_string(ctx, -1, PROP_EVENTLOOP);
		EventLoop *ret = static_cast<EventLoop*>(duk_get_pointer(ctx, -1));
		duk_pop_3(ctx);

		return ret;
	}

	class XHRData
	{
	public:
		XHRData()
		{}

		std::string method;
		std::string url;
	};

	class SendRequestTask : public Task
	{
	public:
		SendRequestTask(EventLoop *eventloop, XHRData data, Ref::Ptr ref) :
			Task(), _eventloop(eventloop), _data(data), _ref(ref)
		{

		}

		void run()
		{
			try
			{
				// post loadstart event
				_eventloop->postEvent(new LoadStartEvent(_ref));

				Poco::URI uri(_data.url);
				Poco::Net::HTTPClientSession session(uri.getHost(), uri.getPort());
				Poco::Net::HTTPRequest request(_data.method, uri.getPathAndQuery());

				Poco::Net::HTTPResponse response;

				session.sendRequest(request);

				// post ready change event
				_eventloop->postEvent(new detail::ReadyStateChangeEvent(_ref, detail::ReadyStateChangeEvent::LOADING));

				std::istream &rs = session.receiveResponse(response);

				std::string ret;
				Poco::StreamCopier::copyToString(rs, ret);

				// post ready change event
				_eventloop->postEvent(new detail::ReadyStateChangeEvent(_ref, detail::ReadyStateChangeEvent::DONE));

				// post load event
				_eventloop->postEvent(new LoadEvent(_ref, ret, response.getStatus(), response.getReason()));
			}
			catch (Poco::Exception &e)
			{
				// post error event
				_eventloop->postEvent(new ErrorEvent(_ref, e.displayText()));
			}
			catch (std::exception &e)
			{
				_eventloop->postEvent(new ErrorEvent(_ref, e.what()));
			}
		}
	private:
		EventLoop *_eventloop;
		XHRData _data;
		Ref::Ptr _ref;
	};

	inline duk_ret_t r_xhrdata_finalizer(duk_context *ctx)
	{
		// 0 = object to finalize
		duk_get_prop_string(ctx, 0, PROP_XHRDATA);
		if (duk_is_pointer(ctx, -1) != 0)
		{
			XHRData* p = static_cast<XHRData*>(duk_get_pointer(ctx, -1));
			delete p;
		}
		duk_pop(ctx);
		duk_del_prop_string(ctx, 0, PROP_XHRDATA);
		return 0;
	}

	inline XHRData *xhrdata_from_this(duk_context *ctx)
	{
		duk_push_this(ctx);
		duk_require_object_coercible(ctx, -1);

		duk_get_prop_string(ctx, -1, PROP_XHRDATA);
		XHRData *ret = static_cast<XHRData*>(duk_get_pointer(ctx, -1));
		duk_pop_2(ctx); // "this", prop
		return ret;
	}

	// Initializes a XMLHttpRequest object
	inline duk_ret_t r_xmlhttprequest_init(duk_context *ctx)
	{
		duk_require_object_coercible(ctx, 0); // "this"

		// add pointer to XHRData
		XHRData *ret = new XHRData;
		duk_push_pointer(ctx, ret);
		duk_put_prop_string(ctx, 0, PROP_XHRDATA);

		// set finalizer
		duk_push_c_function(ctx, &r_xhrdata_finalizer, 1);
		duk_set_finalizer(ctx, 0);

		return 0;
	}

	inline duk_ret_t r_xmlhttprequest_open(duk_context *ctx)
	{
		duk_idx_t args = duk_get_top(ctx);
		if (args < 2)
			duk_error(ctx, DUK_ERR_ERROR, "open requires at least 2 parameters");

		XHRData *data = xhrdata_from_this(ctx);
		EventLoop *eventloop = eventloop_from_xhr(ctx);

		data->method.assign(duk_to_string(ctx, 0));
		data->url.assign(duk_to_string(ctx, 1));

		duk_push_this(ctx);
		// post readystatechange event
		eventloop->postEvent(new detail::ReadyStateChangeEvent(new Ref(ctx), detail::ReadyStateChangeEvent::OPENED));
		// Ref() already pops
		return 0;
	}

	inline duk_ret_t r_xmlhttprequest_send(duk_context *ctx)
	{
		XHRData *data = xhrdata_from_this(ctx);
		EventLoop *eventloop = eventloop_from_xhr(ctx);

		duk_push_this(ctx);
		// post download task
		eventloop->postTask(new detail::SendRequestTask(eventloop, *data, new Ref(ctx)));
		// Ref() already pops
		return 0;
	}

}

inline void RegisterXMLHttpRequest(EventLoop *eventloop)
{
	duk_context *ctx = eventloop->ctx();
	ResetStackOnScopeExit r(ctx);

	// must have EventTarget registered
	if (!eventtarget::IsEventTarget(ctx))
		throw Exception("EventTarget is not registered");

	if (duk_peval_string(ctx, R"(

var XMLHttpRequestEventTarget = function() {
	EventTarget.call(this);

	var defevt = function(obj, name) {
		Object.defineProperty(obj, "on"+name, {
			get: function() { return this["_on"+name]; },
			set: function(val) { 
				if (this["_on"+name] != undefined)
					this.removeEventListener(name, this["_on"+name]);
				this["_on"+name] = val; 
				this.addEventListener(name, val); 
			}
		});
	}

	defevt(this, "abort");
	defevt(this, "error");
	defevt(this, "load");
	defevt(this, "loadstart");
	defevt(this, "progress");
	defevt(this, "timeout");
	defevt(this, "loadend");
};

XMLHttpRequestEventTarget.prototype = Object.create(EventTarget.prototype);
XMLHttpRequestEventTarget.prototype.constructor = XMLHttpRequestEventTarget;


	)") != 0)
	{
		ThrowError(ctx, -1);
	};
	duk_pop(ctx);

	if (duk_peval_string(ctx, R"(

var XMLHttpRequest = function() {
	XMLHttpRequestEventTarget.call(this);

	this.readyState = 0;
	this.response = null;
	this.responseText = null;
	this.responseType = null;
	this.status = 0;
	this.statusText = "";
	this.timeout = 1000;
	this.upload = null;
	this.withCredentials = null;

	var defevt = function(obj, name) {
		Object.defineProperty(obj, "on"+name, {
			get: function() { return this["_on"+name]; },
			set: function(val) { 
				if (this["_on"+name] != undefined)
					this.removeEventListener(name, this["_on"+name]);
				this["_on"+name] = val; 
				this.addEventListener(name, val); 
			}
		});
	}

	defevt(this, "readystatechange");

	__XHR_XMLHttpRequest_Init(this);
}

XMLHttpRequest.prototype = Object.create(XMLHttpRequestEventTarget.prototype);
XMLHttpRequest.prototype.constructor = XMLHttpRequest;

XMLHttpRequest.prototype.abort = function() {
	// TODO
};

XMLHttpRequest.prototype.getAllResponseHeaders = function() {
	// TODO
	return "";
};

XMLHttpRequest.prototype.getResponseHeader = function(name) {
	// TODO
	return "";
};

XMLHttpRequest.prototype.overrideMimeType = function() {
	// TODO
};

XMLHttpRequest.prototype.setRequestHeader = function(header, value) {
	// TODO
};

	)") != 0)
	{
		ThrowError(ctx, -1);
	};
	duk_pop(ctx);

	// register the refs array on the stash if don't exists
	duk_push_heap_stash(ctx);
	if (duk_has_prop_string(ctx, -1, detail::PROP_REFS) == 0)
	{
		duk_push_array(ctx);
		duk_put_prop_string(ctx, -2, detail::PROP_REFS);
	}
	duk_pop(ctx);

	// native methods
	duk_push_global_object(ctx);

	//
	// __XHR_XMLHttpRequest_Init(this)
	//
	duk_push_c_function(ctx, &detail::r_xmlhttprequest_init, 1);
	duk_put_prop_string(ctx, -2, "__XHR_XMLHttpRequest_Init");

	//
	// XMLHttpRequest
	//
	duk_get_prop_string(ctx, -1, "XMLHttpRequest");
	// put EventLoop in XMLHttpRequest
	duk_push_pointer(ctx, eventloop);
	duk_put_prop_string(ctx, -2, detail::PROP_EVENTLOOP);

	// put functions in prototype
	duk_get_prop_string(ctx, -1, "prototype");

	// open
	duk_push_c_function(ctx, &detail::r_xmlhttprequest_open, DUK_VARARGS);
	duk_put_prop_string(ctx, -2, "open");

	// send
	duk_push_c_function(ctx, &detail::r_xmlhttprequest_send, DUK_VARARGS);
	duk_put_prop_string(ctx, -2, "send");

	// pop prototype, obj and global
	duk_pop_3(ctx);
}

}