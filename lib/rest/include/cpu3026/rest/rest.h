#ifndef CPU3026_REST_REST_H
#define CPU3026_REST_REST_H

#include <boost/asio.hpp>

namespace cpu3026::rest {
	class rest_server {
		boost::asio::io_context* io_context;
		boost::asio::ip::tcp::acceptor acceptor;
	public:
		rest_server(boost::asio::io_context* io, unsigned short port);
		void start();
	};
}

#endif // CPU3026_REST_REST_H
