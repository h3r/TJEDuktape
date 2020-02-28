#include <dtel.h>
#include <dtel/lib/eventtarget/EventTarget.h>
#include <dtel/lib/console/Console.h>
#include <xhr/XMLHttpRequest.h>

#include <iostream>

using namespace dtel;

class MyEL : public EventLoop
{
public:
	MyEL(duk_context *ctx) : EventLoop(ctx) {}

	bool processException(const std::exception &e) override
	{
		std::cout << "@@@@@@@ EXCEPTION: " << e.what() << " @@@@@@@" << std::endl;
		return true;
	}
};

class Console : public console::ConsoleWorker
{
public:
	void clear() override
	{
		std::lock_guard<std::mutex> lock(_lock);
		std::cout << "-- CONSOLE CLEAR" << std::endl;
	}

	void output(const std::string &outputtype, const std::string &message) override
	{
		std::lock_guard<std::mutex> lock(_lock);
		std::cout << "-- CONSOLE: " << outputtype << " ** " << message << std::endl;
	}
private:
	std::mutex _lock;
};

void test_xhr(EventLoop &el)
{
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
}

int main(int argc, char *argv[])
{
	duk_context *ctx = duk_create_heap_default();

	{
		MyEL el(ctx);

		eventtarget::RegisterEventTarget(&el);

		auto CNHandler = console::RegisterConsole(&el);
		CNHandler->setWorker(make_intrusive<Console>());

		xhr::RegisterXMLHttpRequest(&el);

		test_xhr(el);

		std::thread t([&el] {
			std::this_thread::sleep_for(std::chrono::milliseconds(5000));
			std::cout << "** TERMINATING **" << std::endl;
			el.terminate();
		});

		el.run();

		t.join();
	}

	duk_destroy_heap(ctx);

	std::cout << "PRESS ANY KEY TO CONTINUE";
	std::cin.ignore();

	return 0;
}