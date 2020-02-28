# DTEL-XHR - XMLHttpRequest for DTEL

DTEL-XHR is a C++11 header-only library that implements XMLHttpRequest for the [DTEL](https://github.com/RangelReale/dtel) javascript event loop library.

The library uses [Poco](https://pocoproject.org/) for the actual HTTP requests.

#### Example

```c++
    duk_context *ctx = duk_create_heap_default();
    
	EventLoop el(ctx);

	eventtarget::RegisterEventTarget(&el);
	auto CNHandler = console::RegisterConsole(&el);
	CNHandler->setWorker(make_intrusive<Console>());
	xhr::RegisterXMLHttpRequest(&el);
		
	if (duk_peval_string(el.ctx(), R"(	

var conn = new XMLHttpRequest();
conn.addEventListener("load", function(e) { 
	console.log("Received data from http request!"); 
	console.log(e.target.status+" -- "+e.target.statusText+" -- "+e.target.responseText.length+" bytes"); 
	/*console.log(e.target.responseText);*/ 
});
conn.addEventListener("error", function(e) { console.error("Error! " + e.message); } );
conn.addEventListener("readystatechange", function(e) { console.log("READY STATE CHANGE: ", e.target.readyState); } );
conn.open("GET", "http://edition.cnn.com/");
conn.send();

	)") != 0)
	{
		ThrowError(el.ctx(), -1);
	}
	
	std::thread t([&el] {
		std::this_thread::sleep_for(std::chrono::milliseconds(7000));
		std::cout << "** TERMINATING **" << std::endl;
		el.terminate();
	});

	el.run();

	t.join();

```

Output:

```
-- CONSOLE: log ** READY STATE CHANGE:  1
-- CONSOLE: log ** READY STATE CHANGE:  3
-- CONSOLE: log ** READY STATE CHANGE:  4
-- CONSOLE: log ** Received data from http request!
-- CONSOLE: log ** 200 -- OK -- 135135 bytes
** TERMINATING **
PRESS ANY KEY TO CONTINUE
```

### Author

Rangel Reale (http://github.com/RangelReale)
