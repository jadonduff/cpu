#include <cpu3026/rest/rest.h>
#include <iostream>
#include <stdexcept>
#include <array>
#include <unordered_map>
#include <functional>

using namespace cpu3026::rest;
using boost::asio::ip::tcp;

std::unordered_map<std::string, std::function<http_response_packet(http_request_packet&, cpu3026::virtual_machine&)>> request_map{
	{
		"/cpu3026/v1/cli", [](http_request_packet&, cpu3026::virtual_machine&) -> http_response_packet {
			
		}
	}
};

rest_server::rest_server(boost::asio::io_context* io, cpu3026::virtual_machine* virtual_machine, unsigned short port) :
	io_context(io),
	virtual_machine(virtual_machine),
	port(port)
{

}

struct http_request_packet {
	std::string_view method;
	std::string_view path;
	std::string_view body;

	template<typename Buffer>
	static inline http_request_packet parse(Buffer& buffer, size_t count) {
		http_request_packet o;
		size_t start = 0;
		size_t end = 0;
		for (; end < count && buffer[end] != ' '; end++);
		o.method = std::string_view{buffer.data() + start, buffer.data() + end};

		for (; end < count && buffer[end] == ' '; end++);
		start = end; // start of next word

		for (; end < count && buffer[end] != ' '; end++);
		o.path = std::string_view{buffer.data() + start, buffer.data() + end};

		for (; end < count - 3 && !std::strncmp(buffer.data()+end,"\r\n\r\n",4); end++);
		end += 4;
		start = end;
		end = count;

		o.body = std::string_view{buffer.data() + start, buffer.data() + end};

		return o;
	}
};

char* stpncpy(char* dest, const char* src, size_t count) {
	while ((count--) > 0 && (*dest++ = *src++) != '\0');
	return --dest; // Returns pointer to the '\0'
}

struct http_response_packet {
	size_t response_code;
	std::string body;

	template<typename Buffer>
	void write(Buffer& buffer) {
		void* ptr = buffer.data();
		ptr = stpncpy(ptr, "HTTP/1.1 ", (size_t)(buffer.end() - ptr));
		std::string rc = std::to_string(response_code);
		ptr = stpncpy(ptr, rc.c_str(), (size_t)(buffer.end() - ptr));
		if (body.empty()) {
			ptr = stpncpy(ptr, "\r\n\r\n", (size_t)(buffer.end() - ptr));
		}
		else {
			ptr = stpncpy(ptr, "\r\nContent-Type: application/json\r\n\r\n", (size_t)(buffer.end() - ptr));
			ptr = stpncpy(ptr, body.c_str(), (size_t)(buffer.end() - ptr));
		}
	}
};


void rest_server::runtime_thread() {
	try {
		tcp::acceptor acceptor{*io_context, tcp::endpoint(tcp::v4(), port)};
		while (true) {
			tcp::socket socket(*io_context);
			acceptor.accept(socket);
			boost::system::error_code ignored_error;
			std::array<char, 4096> buffer;
			size_t count = socket.read_some(buffer);

			auto request = http_request_packet::parse(buffer, count);
			http_response_packet response;
			if (auto v = request_map.find(std::string(request.path)); v != request_map.end()) {
				response = v->second(request, *virtual_machine);
			}
			else {
				response = {
					404,
					""
				};
			}
			response.write(buffer);
			socket.write_some(buffer);
			socket.close();
		}
	}
	catch (std::exception& e) {
		std::cerr << "REST SERVER: " << e.what() << std::endl;
	}
}

void rest_server::start() {
	server_thread = std::make_unique<std::thread>(std::thread{rest_server::runtime_thread, this});
}