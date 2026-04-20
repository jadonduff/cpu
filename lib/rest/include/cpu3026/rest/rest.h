#ifndef CPU3026_REST_REST_H
#define CPU3026_REST_REST_H

#include <boost/asio.hpp>
#include <thread>
#include <memory>
#include <cpu3026/vm/vm.h>

namespace cpu3026::rest {
	class rest_server {
		boost::asio::io_context* io_context;
		std::unique_ptr<std::thread> server_thread;
		cpu3026::virtual_machine* virtual_machine;
		unsigned short port;

		void runtime_thread();
	public:
		rest_server(boost::asio::io_context* io, cpu3026::virtual_machine* virtual_machine, unsigned short port);
		void start();
	};
}

#endif // CPU3026_REST_REST_H
